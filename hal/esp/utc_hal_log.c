/******************************************************************
 * 2019 (C)   UTCloud Copyright. All Rights Reserved.
 * File name    : utc_hal_log.c
 * Author       : 
 * Version      : V1.0
 * Date         : 2019.11.25
 * Description  :
 * Function List:
 * History      :
*******************************************************************/


/*---------------------------------- Include ---------------------------------*/
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "utc_hal_log.h"

/*---------------------------------- Define  ---------------------------------*/
//#ifndef CONFIG_LOG_UART_BAUD
//#define CONFIG_LOG_UART_BAUD 115200 /* 串口初始化波特率*/
//#endif
//#define PRINT_BUF_SIZE       (512)
//#define UART_MAX_SEND_SIZE   (64)

#define UTC_HAL_LOG_OPEN         1
#define UTC_HAL_LOG_CLOSE        0
/*---------------------------------- Variable Define -------------------------*/
static s32 g_init_flag = UTC_HAL_LOG_CLOSE;
static u32 g_log_level = TRLEV_NONE;

static trace_level_t g_module_levels[TRMOD_COUNT] = {
    TRLEV_ERROR, //LOG_UART = 1,
    TRLEV_ERROR, //TRMOD_SMART_CONFIG = 1,
    TRLEV_ERROR, //TRMOD_TIMER
};

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
s32 utc_hal_log_open(void)
{
    s32 ret = OS_OK;
    /**
     * UART Configuration (default):
     * com1,115200,8,n 1
     */
    if (g_init_flag == UTC_HAL_LOG_CLOSE) {
        ret = OS_OK;
        g_init_flag = UTC_HAL_LOG_OPEN;
    }

    return ret;
}

/*******************************************************************************
* Function    : utc_hal_log_close
* Description : 字符打印关闭函数
* Input       :
*
* Output      :
* Return      : OS_OK：关闭成功
*               other：关闭失败
*******************************************************************************/
s32 utc_hal_log_close(void)
{
    g_init_flag = UTC_HAL_LOG_CLOSE;
    return OS_OK;
}


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
s32 utc_hal_log(u32 module, u32 level, const char *p_fmt, ...)
{
	s32 ret = 0;    

    if(g_init_flag == UTC_HAL_LOG_CLOSE){
        return -1;
    }
    
	if ((module < TRMOD_COUNT) && (g_module_levels[module] >= (int)level)) { 
        va_list args;       //定义一个va_list类型的变量，用来储存单个参数
        va_start(args,p_fmt); //使args指向可变参数的第一个参数
        vprintf(p_fmt,args);  //必须用vprintf等带V的
        va_end(args);       //结束可变参数的获取
           fflush(stdout);
	}

    return ret;
}

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
s32 utc_hal_log_hex(u32 module, u32 level, s8 *p_name, s8 *p_buf, u32 len)
{
    u16 i = 0;

    if(g_init_flag == UTC_HAL_LOG_CLOSE){
        return -1;
    }

    if ((module < TRMOD_COUNT) && (g_module_levels[module] >= (int)level)) {
        if (p_name != NULL) {
            printf("%s:\r\n", p_name);
        }

        for (i = 0; i < len; ++i) {
            printf("%02x ", (p_buf[i])&0xff);

            if ((i + 1) % 16 == 0) {
                printf("\r\n");
            }
        }

        printf("\r\n");
    }

    return len;
}

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
s32 utc_hal_log_set_level(u32 module, u32 level)
{
	if (module >= TRMOD_COUNT)
    {
        utc_hal_log(TRMOD_UART, 0, "[%s-%d]trace module id:%d is invalid.\n", __FUNCTION__, __LINE__, module);

		return ERROR_UTC_HAL_LOG_SETLEVEL;
    }

    g_module_levels[module] = (trace_level_t)level;

    return OS_OK;
}

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
s32 utc_hal_log_set_out(u32 out)
{
    return OS_OK;
}

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
s32 utc_hal_log_set_default_level (void)
{
	unsigned int i;

	for(i = 0;i < TRMOD_COUNT; i++)
	{
		g_module_levels[i] = TRLEV_ERROR;
	}

	return OS_OK;
}


