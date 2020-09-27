/******************************************************************
 * 2014 (C)   SMIT Copyright. All Rights Reserved.
 * File name    : DrvTimer.h
 * Author       :
 * Version      : V1.0
 * Date         : 2016.05.03
 * Description  :
 * Function List:
 * History      :
*******************************************************************/
#ifndef __UTC_HAL_TIMER_H__
#define __UTC_HAL_TIMER_H__

/*---------------------------------- Include ---------------------------------*/
#include "smart_config_os.h"

/*---------------------------------- Define  ---------------------------------*/
typedef void (*p_task_timer_func_t)(void *param);
typedef void (*p_system_tick_handle_t)(void);
typedef void* task_timer_handle_t;

#define TIME_0MS                 0
#define TIME_1MS                 1
#define TIME_10MS                10
#define TIME_300MS               300
#define TIME_20S                 20000


/*---------------------------------- Variable Define -------------------------*/

/*---------------------------------- Function declare ------------------------*/
/*******************************************************************************
* Function    : utc_hal_system_tick_register
* Description : 1ms系统时钟处理
* Input       : typedef void (*SysTickHandle_t)(void);
* Output      : none
* Return      : OS_OK		注册成功
*  			    other       注册失败
* Others      :
*******************************************************************************/
s32 utc_hal_system_tick_register(p_system_tick_handle_t handler);

/*******************************************************************************
* Function    : utc_system_tick_current
* Description : 系统启动后经历的ms数
* Input       : none
* Output      : none
* Return      : >0		ms数
*  			    <0      失败
* Others      :
*******************************************************************************/
s32 utc_hal_system_tick_current(void);

/*******************************************************************************
* Function    : utc_hal_set_system_tick
* Description : 设置系统tick时间
* Input       : ms
* Output      : none
* Return      : none
*
* Others      :
*******************************************************************************/
void utc_hal_set_system_tick(u32 ms);

/*******************************************************************************
* Function    : DRV_DelayMs
* Description : 毫秒延时ms
* Input       : Ms
* Output      : none
* Return      : none
*
* Others      :
*******************************************************************************/
void utc_hal_delay_ms(u32 ms);

/*******************************************************************************
* Function    : utc_hal_delay_us
* Description : 微秒延时us
* Input       : Ms
* Output      : none
* Return      : none
*
* Others      :
*******************************************************************************/
void utc_hal_delay_us(u32 us);


void closeThreadTimer();
#endif //__DRV_TIMER_H__

