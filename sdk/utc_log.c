/******************************************************************
 * 2019 (C)   UTCloud Copyright. All Rights Reserved.
 * File name    : utc_log.h
 * Author       : 
 * Version      : V1.0
 * Date         : 2019.11.25
 * Description  :
 * Function List:
 * History      :
*******************************************************************/
#include "utc_log.h"


/*******************************************************************************
* Function    : utc_log_open
* Description : utc_log_open
* Input       : none		
* Output      : none                
* Return      : MPOS_OK   打开成功
*  			    other     打开失败
* Others      :
*******************************************************************************/
s32 utc_log_open(void)
{
	return utc_hal_log_open();
}

/*******************************************************************************
* Function    : utc_log_close
* Description : utc_log_close
* Input       : none		
* Output      : none                
* Return      : MPOS_OK   打开成功
*  			    other     打开失败
* Others      :
*******************************************************************************/
s32 utc_log_close(void)
{
	return utc_hal_log_close();
}

/*******************************************************************************
* Function    : utc_log
* Description : 字符打印函数
* Input       : u32  module --  打印模块
*				32   level  --  打印级别
*				s8   * pFmt,... --  打印内容
* Output      : none                
* Return      : MPOS_OK   打开成功
*  			    other     打开失败
* Others      :
*******************************************************************************/
//s32 utc_log (u32 module, u32 level,  s8 *pFmt,...)
//{
//	s32 ret;
//    
//	ret = utc_hal_log(module, level, pFmt);
//
//	return ret;
//}

/*******************************************************************************
* Function    : utc_log_hex
* Description : HEX打印函数
* Input       : u32  module --  打印模块
*				32   level  --  打印级别
*				s8   pName - 缓冲区名字
*				s8   p_buf  --打印内容首地址
*				u32  len --  打印内容长度   
* Output      : none                
* Return      : MPOS_OK   打开成功
*  			    other     打开失败
* Others      :
*******************************************************************************/
s32 utc_log_hex(u32 module, u32 level, s8 *pName,s8 *p_buf, u32 len)
{
	return utc_hal_log_hex(module, level, (s8 *)pName, (s8 *)p_buf, len);
}

/*******************************************************************************
* Function    :utc_log_set_level
* Description : 设置打印机别
* Input       : u32  module --  打印模块
*				32   level  --  打印级别
*				
* Output      : none                
* Return      : MPOS_OK   打开成功
*  			    other     打开失败
* Others      :
*******************************************************************************/
s32 utc_log_set_level (u32 module, u32 level)
{
	int ret;
	
	ret = utc_hal_log_set_level (module, level);

	return ret;
}

/*******************************************************************************
* Function    : utc_log_set_out
* Description : 设置输出的通道
* Input       : nene		
* Output      : none                
* Return      : MPOS_OK   打开成功
*  			    other     打开失败
* Others      :
*******************************************************************************/
s32 utc_log_set_out (u32 out)
{
	int ret;

	ret = utc_hal_log_set_out(out);

	return ret;
}

/*******************************************************************************
* Function    : utc_log_set_default_level
* Description : 默认级别设置接口
* Input       : none		
* Output      : none                
* Return      : MPOS_OK   打开成功
*  			    other     打开失败
* Others      :
*******************************************************************************/
s32 utc_log_set_default_level(void)
{
	int ret;

	ret = utc_hal_log_set_default_level();

	return ret;
}