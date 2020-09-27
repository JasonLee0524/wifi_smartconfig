/******************************************************************
 * 2019 (C)   utyun Copyright. All Rights Reserved.
 * File name    : global.h
 * Author       : zengxiong
 * Version      : V1.0
 * Date         : 2019.10.23
 * Description  : header file for global
 * Function List:
 * History      :
*******************************************************************/
#ifndef __GLOBAL_H__
#define __GLOBAL_H__


/*****************************************************************
 *	char      --  占用1个字节内存
 *	short     --  占用2字节内存
 *	int       --  占用4字节内存
 *	long      --  占用4字节内存
 *	long int  --  占用4字节内存
 *	float     --  占用4字节内存
 *	double    --  占用8字节内存
*****************************************************************/
typedef unsigned char       u8;             /* 无符号8位变量 -- 0~255一字节 */
typedef unsigned short      u16;            /* 16位短整型无符号变量 -- 0~65535两字节 */
typedef unsigned int        u32;            /* 32位长整型无符号变量 -- 0~(2^32-1)四字节 */
typedef unsigned long       u64;            /* 64位长整型无符号变量 -- 0~(2^64-1)八字节 */

typedef signed   char       s8;             /* 有符号8位变量 -- 128~127 */
typedef signed   short      s16;            /* 16位短整型有符号变量 -- -32768~32767两字节 */
typedef signed   int        s32;            /* [(-2^32)/2]~ [(-2^32)/2-1]四字节 */
typedef signed long long    s64;            /* [(-2^64)/2]~ [(-2^64)/2-1]八字节 */

typedef volatile unsigned char  vu8;         /* 易变的8位无符号变量 */
typedef volatile unsigned short vu16;
typedef volatile unsigned int   vu32;

typedef volatile signed   char  vs8;         /* 易变的8位有符号变量 */
typedef volatile signed   short vs16;
typedef volatile signed   int   vs32;

typedef unsigned char     const uc8;         /* 只读的8位无符号变量 */
typedef unsigned short    const uc16;        /* Read Only */
typedef unsigned int      const uc32;        /* Read Only */

typedef signed   char     const sc8;         /* 只读的8位有符号变量 */
typedef signed   short    const sc16;        /* Read Only */
typedef signed   int      const sc32;        /* Read Only */

typedef volatile unsigned char  const vuc8;  /* 易变只读的8位无符变量 */
typedef volatile unsigned short const vuc16; /* Read Only */
typedef volatile unsigned int   const vuc32; /* Read Only */

typedef volatile signed   char  const vsc8;  /* 易变只读8位有符号变量 */
typedef volatile signed   short const vsc16; /* Read Only */
typedef volatile signed   int   const vsc32; /* Read Only */

/* POINTER defines a generic pointer type */
typedef u8 *POINTER;

/* UINT2 defines a two byte word */
typedef u16 UINT2;

/* UINT4 defines a four byte word */
typedef u32 UINT4;

typedef s32 INT4;

/* BYTE defines a unsigned character */
typedef u8 BYTE;

/* internal signed value */
typedef s32 signeddigit;


#define OS_TRUE           (1)
#define OS_FALSE          (0)
#define OS_OK             (0)

#undef  NULL
#ifdef  __cplusplus
#define NULL              (0)
#else
#define NULL              ((void *)0)
#endif


/*****************************************************************
 *  OS错误代码分区表
 *  1.RSA:                0xFF000100  --  0xFF0001FF
 *****************************************************************/
#define ERROR_RSA_BASE                  (s32)0xFF000100
#define ERROR_LOG_BASE                  (s32)0xFF000100
#define ERROR_SMART_CONFIG_BASE         (s32)0xFF000200

/*****************************************************************
 *  各模块内错误详细代码定义采用枚举方式定义，例如:
 *  enum{
 *      ERROR_NFC_OPEN	=  ERROR_NFC_BASE,
 *      ERROR_NFC_CLOSE,
 *      ……
 *  }
 *****************************************************************/

#endif/*__GLOBAL_H__*/
