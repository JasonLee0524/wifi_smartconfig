/******************************************************************
 * 2019 (C)   UTCloud Copyright. All Rights Reserved.
 * File name    : utc_hal_util.h
 * Author       : 
 * Version      : V1.0
 * Date         : 2019.11.25
 * Description  :
 * Function List:
 * History      :
*******************************************************************/

#ifndef __UTC_HAL_UTIL_H__
#define __UTC_HAL_UTIL_H__

/*---------------------------------- Include ---------------------------------*/
#include "global.h"

/*---------------------------------- Define  ---------------------------------*/


/*---------------------------------- Variable Define -------------------------*/


/*---------------------------------- Function declare ------------------------*/


/*******************************************************************************
* Function    : utc_hal_crc32
* Description : 计算crc32
* Input       : src   数据
                count 数据长度
* Output      : no
* Return      : 
*******************************************************************************/
u32 utc_hal_crc32(const u8 * src, u16 count);

#endif //__UTC_HAL_UTIL_H__

