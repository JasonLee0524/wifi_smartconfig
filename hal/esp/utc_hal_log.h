/******************************************************************
 * 2019 (C)   UTCloud Copyright. All Rights Reserved.
 * File name    : utc_hal_log.h
 * Author       : 
 * Version      : V1.0
 * Date         : 2019.11.25
 * Description  :
 * Function List:
 * History      :
*******************************************************************/

#ifndef __UTC_HAL_LOG_H__
#define __UTC_HAL_LOG_H__

/*---------------------------------- Include ---------------------------------*/
#include "global.h"

/*---------------------------------- Define  ---------------------------------*/
#define PRINT_BUF_SIZE (512)
#define LOG_VERSION50

/*---------------------------------- Variable Define -------------------------*/
typedef enum
{
    LOG_UART,
    LOG_BLUETOOTH,
    LOG_USB
}print_out_t;

#define DEBUG_AUX LOG_PORT     /*调试串口定义*/

typedef enum               //trace module ids
{
    TRMOD_UART = 1,
    TRMOD_SMART_CONFIG,
    TRMOD_TIMER,
    TRMOD_COUNT
}trace_module_t;


//打印等级
/*syslog  等级
 * TRLEV_NONE     0   无条件执行 
 * LOG_EMERG      1   紧急状态
 * TRLEV_ALERT    2   必须立即修复状态
 * TRLEV_CRITICAL 3   严重状态
 * TRLEV_ERROR    4   出错状态
 * TRLEV_WARNING  5   警告状态
 * TRLEV_NOTIFY   6   正常，但重要的状态
 * TRLEV_INFO     7   信息性消息
 * TRLEV_DEBUG    8   调试消息
 */

typedef enum  trace_level_s//trace level
{
    TRLEV_NONE      = 0,   //无条件执行
    TRLEV_EMERG     = 1,   //紧急状态
    TRLEV_ALERT     = 2,   //必须立即修复状态
    TRLEV_CRITICAL  = 3,   //严重状态
    TRLEV_ERROR     = 4,   //出错状态
    TRLEV_WARNING   = 5,   //警告状态
    TRLEV_NOTIFY    = 6,   //正常，但重要的状态
    TRLEV_INFO      = 7,   //信息性消息
    TRLEV_DEBUG     = 8,   //调试消息
    TRLEV_CALL      = 9,
    TRLEV_ISR       = 0x10,
    TRLEV_DSR       = 0x20,
}trace_level_t;
    
typedef enum{
    ERROR_UTC_HAL_LOG = ERROR_LOG_BASE,
    ERROR_UTC_HAL_LOGHEX,
    ERROR_UTC_HAL_LOG_SETLEVEL,

}error_log_e;

/*---------------------------------- Function declare ------------------------*/
/*******************************************************************************
* Function    : utc_hal_log_open
* Description : 字符打印打开函数
* Input       :
*
* Output      :
* Return      : OS_OK：打开成功
*               other：打开失败
*******************************************************************************/
s32 utc_hal_log_open(void);

/*******************************************************************************
* Function    : utc_hal_log_close
* Description : 字符打印关闭函数
* Input       :
*
* Output      :
* Return      : OS_OK：关闭成功
*               other：关闭失败
*******************************************************************************/
s32 utc_hal_log_close(void);


/*******************************************************************************
* Function    : utc_hal_log
* Description : 字符打印函数
* Input       : u32  module --  驱动层打印模块
*				32   level  --  驱动层打印级别
*				s8   * p_fmt,... --  驱动层打印内容
* Output      : none
* Return      : OS_OK   打开成功
*  			    other     打开失败
* Others      :
*******************************************************************************/
s32 utc_hal_log(u32 module, u32 level, const char *p_fmt, ...);

/*******************************************************************************
* Function    : utc_hal_log_hex
* Description : HEX打印函数
* Input       : u32  module --  驱动层打印模块
*				32   level  --  驱动层打印级别
*				s8   p_name - 缓冲区名字
*				s8   p_buf  --打印内容首地址
*				u32  len --  打印内容长度
* Output      : none
* Return      : OS_OK   打开成功
*  			    other     打开失败
* Others      :
*******************************************************************************/
s32 utc_hal_log_hex(u32 module, u32 level, s8 *p_name, s8 *p_buf, u32 len);

/*******************************************************************************
* Function    : utc_hal_log_set_level
* Description : 字符打印函数
* Input       : u32  module --  驱动层打印模块
*				32   level  --  驱动层打印级别
*
* Output      : none
* Return      : OS_OK   打开成功
*  			    other     打开失败
* Others      :
*******************************************************************************/
s32 utc_hal_log_set_level(u32 module, u32 level);

/*******************************************************************************
* Function    : utc_hal_log_set_out
* Description : 字符打印函数
* Input       : u32  module --  驱动层打印模块
*				32   level  --  驱动层打印级别
* Output      : none
* Return      : OS_OK   打开成功
*  			    other     打开失败
* Others      :
*******************************************************************************/
s32 utc_hal_log_set_out(u32 out);

/*******************************************************************************
* Function    : utc_hal_log_set_default_level
* Description : 字符打印函数
* Input       : u32  module --  驱动层打印模块
*				32   level  --  驱动层打印级别
* Output      : none
* Return      : OS_OK   打开成功
*  			    other     打开失败
* Others      :
*******************************************************************************/
s32 utc_hal_log_set_default_level (void);

#endif // UTC_HAL_LOG_H
