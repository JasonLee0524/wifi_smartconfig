/******************************************************************
 * 2019 (C)   UTCloud Copyright. All Rights Reserved.
 * File name    : utc_task_timer.c
 * Author       : 
 * Version      : V1.0
 * Date         : 2019.11.25
 * Description  :
 * Function List:
 * History      :
*******************************************************************/
#include "utc_log.h"
#include "utc_timer.h"
#include "utc_task_timer.h"


task_timer_ctrl_t g_task_timer_ctrl[TIMER_TASK_NUM];

static void task_timer_1ms_func(void)
{
	u8 i;
	p_task_timer_ctrl_t p_timer_ctrl = NULL;

	for(i = 0; i < TIMER_TASK_NUM; i++)
	{
		p_timer_ctrl = &g_task_timer_ctrl[i];
        
//       printf("%s-%d: g_task_timer_ctrl[%d].run: %x\r\n", __FUNCTION__, __LINE__, i, p_timer_ctrl->run);
		if(p_timer_ctrl->run == 1)// 已启动定时器
		{            
			if(p_timer_ctrl->count)
			{
				p_timer_ctrl->count--;
				if(p_timer_ctrl->count == 0)
				{
					// 恢复定时器初值
					p_timer_ctrl->count = p_timer_ctrl->itv_time;
					// 标记定时器计时到
					p_timer_ctrl->mark = 1;
				}
			}
		}
	}
}

/*******************************************************************************
* Function    : utc_task_timer_init
* Description : 任务调度定时器使能
* Input       : none
* Output      : none                
* Return      : OS_OK: 成功
*  			    other: 失败
* Others      : 
*******************************************************************************/
s32 utc_task_timer_init(void)
{
	utc_system_tick_register(task_timer_1ms_func);

	return OS_OK;
}

/*******************************************************************************
* Function    : utc_task_timer_init
* Description : 任务调度定时器失能
* Input       : none
* Output      : none                
* Return      : OS_OK: 成功
*  			    other: 失败
* Others      : 
*******************************************************************************/
s32 utc_task_timer_deinit(void)
{
	return OS_OK;
}


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
task_timer_handle_t utc_task_timer_create(u32 itv_time, p_task_timer_func_t p_func,void *param)
{
	u8 i;
	p_task_timer_ctrl_t p_timer_ctrl = NULL;

	for(i = 0; i < TIMER_TASK_NUM; i++)
	{
		p_timer_ctrl = &g_task_timer_ctrl[i];
		if(p_timer_ctrl->itv_time == 0)
		{
			p_timer_ctrl->itv_time = itv_time;
			p_timer_ctrl->count = itv_time;
			p_timer_ctrl->mark = 0;
			p_timer_ctrl->p_func = p_func;
			p_timer_ctrl->param = param;
			p_timer_ctrl->run = 0;
            if(!itv_time)
            {
                p_timer_ctrl->eloop = 1;
            }

			return p_timer_ctrl;
		}
	}    
    utc_log(TRMOD_TIMER, TRLEV_ERROR, "%s-%d : timer create failed: i: %d\r\n", __FUNCTION__, __LINE__, i);

	return NULL;
}


/*******************************************************************************
* Function    : utc_task_timer_start
* Description : 启动调度任务定时器
* Input       : const task_timer_handle_t ：调度任务句柄  
* Output      : none                
* Return      : OS_OK: 成功
*  			    other: 失败
* Others      : 
*******************************************************************************/
s32 utc_task_timer_start(const task_timer_handle_t handle)
{
	p_task_timer_ctrl_t p_timer_ctrl = NULL;

	if(handle == NULL)
	{
        utc_log(TRMOD_TIMER, TRLEV_ERROR, "%s-%d : handle is null\r\n", __FUNCTION__, __LINE__);
		return -1;
	}
    p_timer_ctrl = (p_task_timer_ctrl_t)handle;
	p_timer_ctrl->run = 1;    

	return OS_OK;
}

/*******************************************************************************
* Function    : utc_task_timer_reset
* Description : 重置调度任务定时器
* Input       : const task_timer_handle_t ：调度任务句柄  
* Output      : none             
* Return      : OS_OK: 成功
*  			    other: 失败
* Others      : 
*******************************************************************************/
s32 utc_task_timer_reset(task_timer_handle_t handle)
{
	p_task_timer_ctrl_t p_timer_ctrl = NULL;

	if(handle == NULL)
	{
        utc_log(TRMOD_TIMER, TRLEV_ERROR, "%s-%d : handle is null\r\n", __FUNCTION__, __LINE__);
		return -1;
	}
    p_timer_ctrl = (p_task_timer_ctrl_t)handle;

	p_timer_ctrl->count = p_timer_ctrl->itv_time;

	return OS_OK;
}


/*******************************************************************************
* Function    : utc_task_timer_stop
* Description : 停止调度任务定时器
* Input       : const task_timer_handle_t ：调度任务句柄  
* Output      : none             
* Return      : OS_OK: 成功
*  			    other: 失败
* Others      : 
*******************************************************************************/
s32 utc_task_timer_stop(task_timer_handle_t handle)
{
	p_task_timer_ctrl_t p_timer_ctrl = NULL;

	if(handle == NULL)
	{
        utc_log(TRMOD_TIMER, TRLEV_ERROR, "%s-%d : handle is null\r\n", __FUNCTION__, __LINE__);
		return -1;
	}
    p_timer_ctrl = (p_task_timer_ctrl_t)handle;

	p_timer_ctrl->run = 0;

	return OS_OK;
}


/*******************************************************************************
* Function    : utc_task_timer_reset
* Description : 删除调度任务定时器
* Input       : const task_timer_handle_t ：调度任务句柄  
* Output      : none             
* Return      : OS_OK: 成功
*  			    other: 失败
* Others      : 
*******************************************************************************/
s32 utc_task_timer_delete(task_timer_handle_t handle)
{
	p_task_timer_ctrl_t p_timer_ctrl = NULL;

	if(handle == NULL)
	{
        utc_log(TRMOD_TIMER, TRLEV_ERROR, "%s-%d : handle is null\r\n", __FUNCTION__, __LINE__);
		return -1;
	}
    p_timer_ctrl = (p_task_timer_ctrl_t)handle;

	p_timer_ctrl->itv_time = 0;
	p_timer_ctrl->count = 0;
	p_timer_ctrl->mark = 0;
	p_timer_ctrl->p_func = NULL;
	p_timer_ctrl->param = NULL;
	p_timer_ctrl->run = 0;

	return OS_OK;
}

/*******************************************************************************
* Function    : utc_task_timer_reset
* Description : 任务调度器执行接口(需要在while(1)循环中执行)
* Input       : none 
* Output      : none             
* Return      : OS_OK: 成功
*  			    other: 失败
* Others      : 
*******************************************************************************/
s32 utc_task_timer_process(void)
{
	u8 i;
	p_task_timer_ctrl_t p_task_timer_ctrl = NULL;

	for(i = 0; i < TIMER_TASK_NUM; i++)
	{
		p_task_timer_ctrl = &g_task_timer_ctrl[i];
		if(p_task_timer_ctrl->mark == 1 || p_task_timer_ctrl->eloop == 1)
		{
			if(p_task_timer_ctrl->p_func)
			{
				p_task_timer_ctrl->p_func(p_task_timer_ctrl->param);
			}
			p_task_timer_ctrl->mark = 0;
		}
	}

	return OS_OK;
}

