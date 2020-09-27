#include <stdio.h>
#include <string.h>
#include "utc_hal_log.h"
#include "utc_hal_timer.h"

static u32 gSysTimerClk;
static u32 gSystemTick = 0;
static u8  gServiceSysTickTimer = 0;
p_system_tick_handle_t gSysTickFunc = NULL;
static 	u8 g_threadtimer = 1;

void closeThreadTimer(){
        g_threadtimer = 0;
}

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

static void *thread_timer_1ms_handle(void *arg)
{
    //提供函数回调保护
    while(g_threadtimer){
        timer_1ms_handle();
        utc_hal_log(TRMOD_TIMER, TRLEV_DEBUG, "thread1 is running\n");
#ifdef CONFIG_IDF_TARGET_ESP8266
        usleep(500);
#else
       usleep(1000);
#endif
    }
    vTaskDelete(NULL);
    return NULL;
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
    utc_hal_log(TRMOD_TIMER, TRLEV_DEBUG, "utc_hal_system_tick_register\n");
    xTaskCreate((void (*)(void *))thread_timer_1ms_handle, "timer", 2048, NULL, 5, NULL);


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
    usleep(1000*ms);
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
