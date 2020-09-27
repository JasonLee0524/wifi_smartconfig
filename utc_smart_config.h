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

#ifndef __UTC_SMART_CONFIG_H__
#define __UTC_SMART_CONFIG_H__

/*---------------------------------- Include ---------------------------------*/
#include "global.h"

/*---------------------------------- Define  ---------------------------------*/


/*---------------------------------- Variable Define -------------------------*/
typedef enum{
    ERROR_SMART_CONFIG_AP_PARAM = ERROR_SMART_CONFIG_BASE,
    ERROR_SMART_CONFIG_AP_WAIT,
    ERROR_SMART_CONFIG_AP_TIMEOUT,
    ERROR_SMART_CONFIG_WIFI_NAME,
}error_smart_config_e;


/*---------------------------------- Function declare ------------------------*/
/*******************************************************************************
* Function    : utc_smart_config_open
* Description : 开启配网接口
* Input       : u8 *p_wifi_name: wifi name
* Output      : none                
* Return      : OS_OK: 成功
*  			    other: 失败
* Others      : 
*******************************************************************************/
s32 utc_smart_config_open(u8 *p_wifi_name);

/*******************************************************************************
* Function    : utc_smart_config_close
* Description : 关闭配网接口
* Input       : u8 *p_wifi_name: wifi name
* Output      : none                
* Return      : OS_OK: 成功
*  			    other: 失败
* Others      : 
*******************************************************************************/
s32 utc_smart_config_close(u8 *p_wifi_name);

/*******************************************************************************
* Function    : utc_smart_config_get_ap
* Description : 获取配网结果接口
* Input       : none
* Output      : u8  *p_ssid     : ssid buf
*               u16 *p_ssid_len ：ssid len
*               u8  *p_psk      : psk  buf
*               u16 *p_psk_len  : psk  len                
* Return      : OS_OK                         : 配网成功
*  			    ERROR_SMART_CONFIG_AP_WAIT    ：等待配网结果
*               ERROR_SMART_CONFIG_AP_TIMEOUT : 配网超时
* Others      : 
*******************************************************************************/
s32 utc_smart_config_get_ap(u8 *p_ssid, u16 *p_ssid_len, u8 *p_psk, u16 *p_psk_len);
#endif //__UTC_SMART_CONFIG_H__

