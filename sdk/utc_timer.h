/******************************************************************
 * 2019 (C)   UTCloud Copyright. All Rights Reserved.
 * File name    : utc_timer.h
 * Author       : 
 * Version      : V1.0
 * Date         : 2019.11.25
 * Description  :
 * Function List:
 * History      :
*******************************************************************/
#ifndef __UTC_TIMER_H__
#define __UTC_TIMER_H__

/*---------------------------------- Include ---------------------------------*/
#include "global.h"
#include "utc_hal_timer.h"

/*---------------------------------- Define  ---------------------------------*/

/*---------------------------------- Variable Define -------------------------*/

/*---------------------------------- Function declare ------------------------*/
/*******************************************************************************
* Function    : utc_system_tick_register
* Description : 1ms系统时钟处理
* Input       : typedef void (*sys_tick_handle_t)(void);
* Output      : none                
* Return      : OS_OK		注册成功
*  			    other       注册失败
* Others      : 
*******************************************************************************/
s32 utc_system_tick_register(task_timer_handle_t handler);

/*******************************************************************************
* Function    : utc_system_tick_current
* Description : 系统启动后经历的ms数
* Input       : none
* Output      : none                
* Return      : >0		ms数
*  			    <0      失败
* Others      : 
*******************************************************************************/
s32 utc_system_tick_current(void);

/*******************************************************************************
* Function    : utc_delay_ms
* Description : 毫秒延时ms
* Input       : Ms
* Output      : none                
* Return      : none
*  			    
* Others      : 
*******************************************************************************/
void utc_delay_ms(u32 ms);

/*******************************************************************************
* Function    : utc_delay_us
* Description : 微秒延时Ms
* Input       : Ms
* Output      : none                
* Return      : none
*  			    
* Others      : 
*******************************************************************************/
void utc_delay_us(u32 ms);

#endif //__UTC_TIMER_H__

