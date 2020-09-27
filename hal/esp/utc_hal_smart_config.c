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
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "utc_hal_log.h"
#include "utc_hal_smart_config.h"
#include "utc_hal_timer.h"
/*---------------------------------- Define  ---------------------------------*/

/*---------------------------------- Variable Define -------------------------*/
/// Identify each physical interface
typedef enum FHandleVal
{
  INTERFACE_SMART_CONFIG = 0,
//  INTERFACE_UDP_LTE,
  INTERFACE_MAX_VAL
}eFHandleVal;

static parse_ieee80211_frame_cb_t p_parse_80211_packet_frame_func;


struct ieee80211_channel {
	u16 channel;
	u16 center_freq;
};

#define CHAN2G(_channel, _freq) {           \
    .channel       = (_channel),           \
    .center_freq        = (_freq),          \
}

static struct ieee80211_channel channels[] = {
	CHAN2G(1, 2412),
	CHAN2G(2, 2417),
	CHAN2G(3, 2422),
	CHAN2G(4, 2427),
	CHAN2G(5, 2432),
	CHAN2G(6, 2437),
	CHAN2G(7, 2442),
	CHAN2G(8, 2447),
	CHAN2G(9, 2452),
	CHAN2G(10, 2457),
	CHAN2G(11, 2462),
	CHAN2G(12, 2467),
	CHAN2G(13, 2472),
	CHAN2G(14, 2484),
};
int convert_freq_to_channel(u16 freq)
{
	int i;
	int max_channel_nums = sizeof(channels) / sizeof(struct ieee80211_channel);

	for(i = 0; i < max_channel_nums; i++)
	{
		if(freq <= channels[i].center_freq)
			break;
	}

	return channels[i].channel;
}


static void  wifi_sniffer_packet_handler(void* buff, wifi_promiscuous_pkt_type_t type)
{
		if(type != WIFI_PKT_DATA){
			return;
		}
		const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buff;
		int len = 0;
#ifdef CONFIG_IDF_TARGET_ESP8266
		len = ppkt->rx_ctrl.sig_mode ? ppkt->rx_ctrl.HT_length : ppkt->rx_ctrl.legacy_length;
#else
		len = ppkt->rx_ctrl.sig_len;
#endif
		if(len >150){
			return;
		}
//	  utc_hal_log(TRMOD_SMART_CONFIG, TRLEV_WARNING,"wifi_sniffer_packet_handler ");
	uint8_t g_channel = ppkt->rx_ctrl.channel ;
	if(g_channel >1){
			//去头数据
			if(NULL == p_parse_80211_packet_frame_func){
				return;
			}
		  p_parse_80211_packet_frame_func(ppkt->payload,len,g_channel,0);
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

	 if (!parse_80211_frame_cb) {
		  utc_hal_log(TRMOD_SMART_CONFIG, TRLEV_WARNING,"utc_hal_smart_config_open error");
        return OS_FALSE;
    }
   p_parse_80211_packet_frame_func = parse_80211_frame_cb;
 	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous_rx_cb(wifi_sniffer_packet_handler));

#ifdef CONFIG_IDF_TARGET_ESP8266
    extern void esp_wifi_set_promiscuous_data_len(uint32_t);
    esp_wifi_set_promiscuous_data_len(512);
#endif
   
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
	 if (!p_parse_80211_packet_frame_func) {
        return;
    }

    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(false));
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous_rx_cb(NULL));

    p_parse_80211_packet_frame_func = NULL;

	closeThreadTimer();

	return OS_OK;
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
// 	printf("12312,channel = %d\r\n",channel);
   if (esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE) != ESP_OK) {
        utc_hal_log(TRMOD_SMART_CONFIG, TRLEV_WARNING,"esp_wifi_set_channel channel %d", channel);
        return OS_FALSE;
    }
	// printf("12312-----------\r\n");
   return  OS_OK;
}
/*******************************************************************************
* Function    : utc_hal_smart_config_hal_total_num_channel
* Description : 配网最大信道值HAL接口
* Input       : none
* Output      : none                
* Return      : 最大信道值
* Others      : 
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
    return convert_freq_to_channel(freq);
}



