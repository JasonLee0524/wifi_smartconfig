/******************************************************************
 * 2019 (C)   UTCloud Copyright. All Rights Reserved.
 * File name    : utc_task_timer.h
 * Author       : 
 * Version      : V1.0
 * Date         : 2019.11.25
 * Description  :
 * Function List:
 * History      :
*******************************************************************/
#ifndef __UTC_TASK_TIMER_H__
#define __UTC_TASK_TIMER_H__

/*---------------------------------- Include ---------------------------------*/
#include "global.h"
#include "utc_hal_timer.h"

/*---------------------------------- Define  ---------------------------------*/
#define TIMER_TASK_NUM   15

/*---------------------------------- Variable Define -------------------------*/
typedef struct tag_task_timer_ctrl
{
    u8 eloop;                    //标识一直运行
	u8 mark;                     // 标识时间是否到了
	u8 run;                      // 程序运行标志，1代表运行，0代表不运行
	u32 count;                   // 计数值，单位ms，递减计数
	u32 itv_time;                // 时间间隔，单位ms，初始值和count一样
	p_task_timer_func_t p_func;  // 定时器执行函数
	void *param;
}task_timer_ctrl_t,*p_task_timer_ctrl_t;

/*---------------------------------- Function declare ------------------------*/
/*******************************************************************************
* Function    : utc_task_timer_init
* Description : 任务调度定时器使能
* Input       : none
* Output      : none                
* Return      : OS_OK: 成功
*  			    other: 失败
* Others      : 
*******************************************************************************/
s32 utc_task_timer_init(void);

/*******************************************************************************
* Function    : utc_task_timer_init
* Description : 任务调度定时器失能
* Input       : none
* Output      : none                
* Return      : OS_OK: 成功
*  			    other: 失败
* Others      : 
*******************************************************************************/
s32 utc_task_timer_deinit(void);

/*******************************************************************************
* Function    : utc_task_timer_create
* Description : 创建调度任务定时器
* Input       : u32 itv_time               ：调度执行间隔
*               p_task_timer_func_t p_func ：调度执行函数
*               void *param                ：调度执行函数的入参
* Output      : none             
* Return      : task_timer_handle_t        ：调度任务句柄
* Others      : 
*******************************************************************************/
task_timer_handle_t utc_task_timer_create(u32 itv_time, \
                                         p_task_timer_func_t p_func,void *param);

/*******************************************************************************
* Function    : utc_task_timer_start
* Description : 启动调度任务定时器
* Input       : const task_timer_handle_t ：调度任务句柄  
* Output      : none                
* Return      : OS_OK: 成功
*  			    other: 失败
* Others      : 
*******************************************************************************/
s32 utc_task_timer_start(const task_timer_handle_t handle);

/*******************************************************************************
* Function    : utc_task_timer_reset
* Description : 重置调度任务定时器
* Input       : const task_timer_handle_t ：调度任务句柄  
* Output      : none             
* Return      : OS_OK: 成功
*  			    other: 失败
* Others      : 
*******************************************************************************/
s32 utc_task_timer_reset(task_timer_handle_t handle);

/*******************************************************************************
* Function    : utc_task_timer_stop
* Description : 停止调度任务定时器
* Input       : const task_timer_handle_t ：调度任务句柄  
* Output      : none             
* Return      : OS_OK: 成功
*  			    other: 失败
* Others      : 
*******************************************************************************/
s32 utc_task_timer_stop(task_timer_handle_t handle);

/*******************************************************************************
* Function    : utc_task_timer_reset
* Description : 删除调度任务定时器
* Input       : const task_timer_handle_t ：调度任务句柄  
* Output      : none             
* Return      : OS_OK: 成功
*  			    other: 失败
* Others      : 
*******************************************************************************/
s32 utc_task_timer_delete(task_timer_handle_t handle);

/*******************************************************************************
* Function    : utc_task_timer_reset
* Description : 任务调度器执行接口(需要在while(1)循环中执行)
* Input       : none 
* Output      : none             
* Return      : OS_OK: 成功
*  			    other: 失败
* Others      : 
*******************************************************************************/
s32 utc_task_timer_process(void);



#endif // TASK_TIMER_H

