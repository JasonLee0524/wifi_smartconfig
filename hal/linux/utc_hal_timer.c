#include <stdio.h>
#include <string.h>
#include<signal.h>
#include <pthread.h> 
#include <errno.h> 
#include <sys/time.h>  
#include <unistd.h> 


#include "global.h"
#include "utc_hal_log.h"
#include "utc_hal_timer.h"

static u32 gSysTimerClk;
static u32 gSystemTick = 0;
static u8  gServiceSysTickTimer = 0;
p_system_tick_handle_t gSysTickFunc = NULL;

static inline void timer_1ms_handle(void)
{  
    if(gSysTimerClk)
	{
		gSysTimerClk--;
	}
    if(gSysTickFunc)
    {
        gSysTickFunc();
    }

	gSystemTick++;
}

pthread_mutex_t mutex;

static void *thread_timer_1ms_handle(void *arg)
{
    
    struct timeval tv;
    u32 time_old = 0;
    u32 time_new = 0;

    gettimeofday(&tv,NULL);
    time_old = tv.tv_sec*1000 + tv.tv_usec/1000;
    time_new = time_old;
    pthread_cleanup_push((void *)pthread_mutex_unlock,(void *)&mutex);
    //提供函数回调保护
    while(1){
        gettimeofday(&tv,NULL);
        time_new = tv.tv_sec*1000 + tv.tv_usec/1000;
        if(time_new - time_old >= 1){
            timer_1ms_handle();
            utc_hal_log(TRMOD_TIMER, TRLEV_DEBUG, "thread1 is running\n");
            time_old = time_new;
        }
//        usleep(1000);
    }
    pthread_cleanup_pop(0);
}


s32 timer_init()
{
    pthread_t thid1;
    pthread_mutex_init(&mutex,NULL);
    pthread_create(&thid1,NULL,(void*)thread_timer_1ms_handle,NULL);

    return OS_OK;
}


/*******************************************************************************
* Function    : utc_hal_system_tick_register
* Description : 1ms系统时钟处理
* Input       : typedef void (*SysTickHandle_t)(void);
* Output      : none
* Return      : OS_OK		注册成功
*  			    other       注册失败
* Others      :
*******************************************************************************/
s32 utc_hal_system_tick_register(p_system_tick_handle_t handler)
{
    if (NULL != handler) {
        gSysTickFunc = handler;
    }

    if (1 == gServiceSysTickTimer) {
        return OS_OK;
    }
    gServiceSysTickTimer = 1;

    timer_init();
//    pthread_exit(NULL); 

    return OS_OK;
}

/*******************************************************************************
* Function    : utc_system_tick_current
* Description : 系统启动后经历的ms数
* Input       : none
* Output      : none
* Return      : >0		ms数
*  			    <0      失败
* Others      :
*******************************************************************************/
s32 utc_hal_system_tick_current(void)
{
	return gSystemTick;
}

/*******************************************************************************
* Function    : utc_hal_set_system_tick
* Description : 设置系统tick时间
* Input       : ms
* Output      : none
* Return      : none
*
* Others      :
*******************************************************************************/
void utc_hal_set_system_tick(u32 ms)
{
	gSysTimerClk = ms;
}


/*******************************************************************************
* Function    : DRV_DelayMs
* Description : 毫秒延时ms
* Input       : Ms
* Output      : none
* Return      : none
*
* Others      :
*******************************************************************************/
void utc_hal_delay_ms(u32 ms)
{
    struct timeval tval;

    tval.tv_sec=ms/1000;

    tval.tv_usec=(ms*1000)%1000000;

    select(0,NULL,NULL,NULL,&tval);


}


/*******************************************************************************
* Function    : utc_hal_delay_us
* Description : 微秒延时us
* Input       : Ms
* Output      : none
* Return      : none
*
* Others      :
*******************************************************************************/
void utc_hal_delay_us(u32 us)
{
	usleep(us);
}
