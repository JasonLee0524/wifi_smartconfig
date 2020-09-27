/******************************************************************
 * 2019 (C)   UTCloud Copyright. All Rights Reserved.
 * File name    : utc_smart_config.h
 * Author       : 
 * Version      : V1.0
 * Date         : 2019.11.25
 * Description  :
 * Function List:
 * History      :
*******************************************************************/

#ifndef __UTC_HAL_SMART_CONFIG_H__
#define __UTC_HAL_SMART_CONFIG_H__

/*---------------------------------- Include ---------------------------------*/
#include "global.h"
#include "utc_hal_timer.h"

/*---------------------------------- Define  ---------------------------------*/
#define SMART_CONFIG_SOCKET_MAX_LENGTH    1024

/*---------------------------------- Variable Define -------------------------*/
typedef void (*sys_tick_handle_t)(void);
typedef int (*parse_ieee80211_frame_cb_t)(u8 * buf, u16 len, u16 channel, s32 socket);
typedef u16 (*parse_ieee80211_radio_cb_t)(u8 *p_packet_recv, u16 packet_recv_len, u16 *p_channel);

/*---------------------------------- Function declare ------------------------*/
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
                                parse_ieee80211_radio_cb_t parse_ieee80211_radio_cb);

/*******************************************************************************
* Function    : utc_hal_smart_config_close
* Description : 关闭配网HAL接口
* Input       : none
* Output      : none                
* Return      : OS_OK：成功
*  			    other：失败
* Others      : 
*******************************************************************************/
s32 utc_hal_smart_config_close(void);

/*******************************************************************************
* Function    : utc_hal_smart_config_hal_switch_channel
* Description : 配网切换信道HAL接口
* Input       : u32 channel
* Output      : none                
* Return      : OS_OK：成功
*  			    other：失败
* Others      : 
*******************************************************************************/
s32 utc_hal_smart_config_hal_switch_channel(u32 channel);

/*******************************************************************************
* Function    : utc_hal_smart_config_hal_total_num_channel
* Description : 配网最大信道值HAL接口
* Input       : none
* Output      : none                
* Return      : 最大信道值
* Others      : 
*******************************************************************************/
u16 utc_hal_smart_config_hal_total_num_channel(void);

/*******************************************************************************
* Function    : utc_hal_smart_config_freq_2_channel
* Description : 配网频段转信道HAL接口
* Input       : u32 freq: 频段
* Output      : none                
* Return      : channel：信道值
* Others      : 
*******************************************************************************/
u32 utc_hal_smart_config_freq_2_channel(s32 freq);

#endif //__UTC_SMART_CONFIG_H__

