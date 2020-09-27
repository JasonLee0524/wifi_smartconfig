/******************************************************************
 * 2019 (C)   UTCloud Copyright. All Rights Reserved.
 * File name    : utc_hal_smart_config.c
 * Author       : 
 * Version      : V1.0
 * Date         : 2019.11.25
 * Description  :
 * Function List:
 * History      :
*******************************************************************/


/*---------------------------------- Include ---------------------------------*/
#include <pthread.h>
#include <poll.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/if_packet.h>
#include <netinet/in.h>			/* for function htons */
#include <linux/if_ether.h>		/* for ETH_P_ALL */
#include <linux/wireless.h>
#include <net/if_arp.h>			/*ARPHRD_IEEE80211_RADIOTAP */
#include "iface.h"
#include "utc_hal_log.h"
#include "utc_hal_util.h"
#include "utc_hal_smart_config.h"


/*---------------------------------- Define  ---------------------------------*/

/*---------------------------------- Variable Define -------------------------*/
/// Identify each physical interface
typedef enum FHandleVal
{
  INTERFACE_SMART_CONFIG = 0,
//  INTERFACE_UDP_LTE,
  INTERFACE_MAX_VAL
}eFHandleVal;


static u8 g_service_smart_config = 0;
static parse_ieee80211_frame_cb_t p_parse_80211_packet_frame_func;
static parse_ieee80211_radio_cb_t p_parse_ieee80211_radio_func;
static u8 *gp_device = NULL;
static struct pollfd FDesc[INTERFACE_MAX_VAL];
static pthread_mutex_t mutex;


/// Handle poll input types
#define POLL_INPUT (POLLIN  | POLLPRI)
/// Handle poll error types
#define POLL_ERROR (POLLERR | POLLHUP | POLLNVAL)
/// Handle pool output types
#define POLL_OUTPUT (POLLOUT)

/*---------------------------------- Function declare ------------------------*/

static void packet_receive(s32 socket_fd)
{
    s32 ret = OS_OK;
    u8  paclet_buf[SMART_CONFIG_SOCKET_MAX_LENGTH] = {0};
    u16 packet_head_len = 0;
    u32 packet_channel = 0;
    u16 packet_recv_len = 0;
    u16 channel = 0;    
    u32 fcs = 0;
    u32 crc = 0;
    u8  *p = NULL;

//    DBG_LOG();
    packet_recv_len = recv(socket_fd, paclet_buf, sizeof(paclet_buf), 0);
	if (packet_recv_len < 0) 
    {
		return;
	}

    
	packet_head_len = (paclet_buf[3]<<8)| paclet_buf[2];
//    utc_hal_log(TRMOD_SMART_CONFIG, TRLEV_DEBUG, "%s-%d , packet_head_len:%d,packet_recv_len: %d\r\n", __FUNCTION__, __LINE__, packet_head_len, packet_recv_len);
    p_parse_ieee80211_radio_func(paclet_buf, packet_recv_len, &channel);
    if(packet_head_len >= packet_recv_len)
    {
        return;
    }    
//    utc_hal_log_hex(TRMOD_SMART_CONFIG, TRLEV_INFO, "packet_receive", paclet_buf, packet_recv_len);
    
//    utc_hal_log(TRMOD_SMART_CONFIG, TRLEV_DEBUG, "%s-%d , packet_head_len:%d,channel: %d\r\n", __FUNCTION__, __LINE__, packet_head_len, channel);
    
//    channel = (paclet_buf[19]<<8)|paclet_buf[18];
//            printf("------%s-%d, channel: %x(%x-%x)\r\n", __FUNCTION__, __LINE__, channel, paclet_buf[19], paclet_buf[18]);
//    utc_hal_log(TRMOD_SMART_CONFIG, TRLEV_INFO, "packet_recv_len %d , packet_head_len: %d\r\n", packet_head_len, packet_recv_len);
    pthread_mutex_lock(&mutex); 
//    p = &paclet_buf[packet_head_len];
//    if(NULL != (u32 *)(p+packet_recv_len-packet_head_len-4) && NULL != (u32 *)(p+packet_recv_len-packet_head_len-4)){
//        fcs = *(u32 *)(p+packet_recv_len-packet_head_len-4);
//        crc = utc_hal_crc32(p, packet_recv_len-packet_head_len-4);
//        if (fcs == crc) 
        {
            p_parse_80211_packet_frame_func(&paclet_buf[packet_head_len], packet_recv_len-packet_head_len, channel, socket_fd);
        }
//    }
    pthread_mutex_unlock(&mutex);
}

void parse_80211_frame_handler(void *p_param)
{
    s32 Data = poll(FDesc, INTERFACE_MAX_VAL, 5000);//

//    DBG_LOG();

    // poll status check
    if (Data < 0)
    {
        // error
        utc_hal_log(TRMOD_SMART_CONFIG, TRLEV_ERROR, "Poll error %d '%s'\n", errno, strerror(errno));
    }
    else if (Data == 0)
    {
        // timeout
        utc_hal_log(TRMOD_SMART_CONFIG, TRLEV_WARNING,"Poll timeout\n");
        return;
    }

    // Receive DCP UDP
    if (FDesc[INTERFACE_SMART_CONFIG].revents & POLL_ERROR)
    {
        utc_hal_log(TRMOD_SMART_CONFIG, TRLEV_NOTIFY,"Poll error on UDP (revents 0x%02x)\n",
        FDesc[INTERFACE_SMART_CONFIG].revents);
    }
    if (FDesc[INTERFACE_SMART_CONFIG].revents & POLL_INPUT)
    {
        packet_receive(FDesc[INTERFACE_SMART_CONFIG].fd);

    }
}

/*******************************************************************************
* Function    : utc_hal_smart_config_open
* Description : 开启配网HAL接口
* Input       : u8 *p_device                                       ：wifi接口
*               parse_ieee80211_frame_cb_t parse_80211_frame_cb    ：数据解析回调
*               parse_ieee80211_radio_cb_t parse_ieee80211_radio_cb：解析radio头回调
* Output      : p_task_timer_func_t *pp_parse                      ：接收数据回调                
* Return      : OS_OK：成功
*  			    other：失败
* Others      : 
*******************************************************************************/
s32 utc_hal_smart_config_open(u8 *p_device, \
                                parse_ieee80211_frame_cb_t parse_80211_frame_cb, \
                                p_task_timer_func_t *pp_parse,\
                                parse_ieee80211_radio_cb_t parse_ieee80211_radio_cb)
{
    s32 ret = OS_OK;
    s32 sock_fd;
	s32  protocol;

    if(g_service_smart_config){
        goto out;
    }
    g_service_smart_config = 1;

    
    gp_device = p_device;
    
    utc_hal_log(TRMOD_SMART_CONFIG, TRLEV_INFO,"device: %s\r\n", gp_device);

    pthread_mutex_init(&mutex, NULL);

	protocol = htons(ETH_P_ALL);
	/*protocol = htons(ARPHRD_IEEE80211); */
	/*protocol = htons(ARPHRD_IEEE80211_RADIOTAP); */

	sock_fd = socket(PF_PACKET, SOCK_RAW, protocol);
	if (sock_fd == -1) {
		utc_hal_log(TRMOD_SMART_CONFIG, TRLEV_ERROR, "[%s-%d] socket error\r\n", __FUNCTION__, __LINE__);
		ret =  sock_fd;
        goto out;
	}

	if (iface_set_monitor_mode(sock_fd, gp_device) < 0) {
		utc_hal_log(TRMOD_SMART_CONFIG, TRLEV_ERROR, "can not set monitor mode");
		close(sock_fd);
		ret = -1;
        goto out;
	}

	if (iface_socket_bind(sock_fd, gp_device, protocol) < 0) {
		utc_hal_log(TRMOD_SMART_CONFIG, TRLEV_ERROR, "can not bind socket fd:%d", sock_fd);
		close(sock_fd);
		ret= -1;
        goto out;
	}

    FDesc[INTERFACE_SMART_CONFIG].fd = sock_fd;
          if (FDesc[INTERFACE_SMART_CONFIG].fd < 0) exit(1);

    // setup the poll events
     FDesc[INTERFACE_SMART_CONFIG].events  = POLL_INPUT;

    p_parse_80211_packet_frame_func = parse_80211_frame_cb;
    p_parse_ieee80211_radio_func = parse_ieee80211_radio_cb;

    *pp_parse = parse_80211_frame_handler;    

out:    
	return ret;
}

/*******************************************************************************
* Function    : utc_hal_smart_config_close
* Description : 关闭配网HAL接口
* Input       : none
* Output      : none                
* Return      : OS_OK：成功
*  			    other：失败
* Others      : 
*******************************************************************************/
s32 utc_hal_smart_config_close(void)
{   
    s32 ret = OS_OK;
    
    if(!g_service_smart_config){
        goto out;
    }
    g_service_smart_config = 0;
    
    if (iface_set_managed_mode(FDesc[INTERFACE_SMART_CONFIG].fd, gp_device) < 0) {
		utc_hal_log(TRMOD_SMART_CONFIG, TRLEV_ERROR, "%s-%d can not set managed mode", __FUNCTION__, __LINE__);
	}

    if(FDesc[INTERFACE_SMART_CONFIG].fd) 
    {
        close(FDesc[INTERFACE_SMART_CONFIG].fd);
    }
    p_parse_80211_packet_frame_func = NULL;
    p_parse_ieee80211_radio_func = NULL;
    
    gp_device = NULL;
out:    
	return ret;
}

/*******************************************************************************
* Function    : utc_hal_smart_config_hal_switch_channel
* Description : 配网切换信道HAL接口
* Input       : u32 channel
* Output      : none                
* Return      : OS_OK：成功
*  			    other：失败
* Others      : 
*******************************************************************************/
s32 utc_hal_smart_config_hal_switch_channel(u32 channel)
{
    s32 ret = OS_OK;
    
    ret = iface_set_freq_index(FDesc[INTERFACE_SMART_CONFIG].fd, gp_device, channel);
}

/*******************************************************************************
* Function    : utc_hal_smart_config_hal_total_num_channel
* Description : 配网最大信道值HAL接口
* Input       : none
* Output      : none                
* Return      : 最大信道值
* Others      : 国内2.5GHz WIFI只支持1-13信道
*******************************************************************************/
u16 utc_hal_smart_config_hal_total_num_channel(void)
{
    return 13;
}


/*******************************************************************************
* Function    : utc_hal_smart_config_freq_2_channel
* Description : 配网频段转信道HAL接口
* Input       : u32 freq: 频段
* Output      : none                
* Return      : channel：信道值
* Others      : 
*******************************************************************************/
u32 utc_hal_smart_config_freq_2_channel(s32 freq)
{
    return  iface_get_freq_index(freq);
}



