/**************************
 * 功能模块开关专用头文件 * 
 **************************/
#ifndef __TYPE_H
#define __TYPE_H

//#include <stm32f10x_lib.h>
#include "stm32f10x.h"

/* 功能开关 */
//此处不用usart2
#define		My_Sys_Tick			//利用系统心跳作定时延时
//#define		My_Watch_Dog		//定义看门狗
//#define		My_RTC				//用了RTC
#define		My_Usb				//USB
#define		My_Usart1			//定义使用了串口1,有相关初始化	   串口1不走了
//#define		My_Usart2			//定义使用了串口2,有相关初始化
//#define		My_Usart3			//定义使用了串口3,有相关初始化
//#define		My_Tim1		 		//定义使用了定时器1,有相关初始化
#define		My_Tim2		 		//定义使用了定时器2,有相关初始化
#define		My_Tim3		 		//定义使用了定时器3,有相关初始化
//#define		My_Tim4		 		//定义使用了定时器3,有相关初始化
//#define		My_Tim4		 		//定义使用了定时器4,有相关初始化
//#define		My_Tim5		 		//定义使用了定时器5,有相关初始化
#define		My_Tda8007	 		//定义使用了TDA8007,有相关初始化
#define		My_RC632		 	//定义使用了RC632,有相关初始化
//#define		My_FlashE2		 	//定义Flash作E2用的初始化
#define		My_Exti_8007	 		//定义TDA8007外部中断3				
//#define		My_Exti_632	 		//定义RC632外部中断3				 
#define		My_CloCard			//上电关所有卡，并初始输出相关IO

#define		My_Debug			//上电关所有卡，并初始输出相关IO
//#define		My_ABC				//农行协议，否则走c030之类自己的定义的协议

#define		My_SmartCard		//定义使用了SAM3的接口USART3
#define		My_SmartCard2		//定义使用了SAM2的接口USART2

/* 错误码 */
/*
#define		CMD_SUCC		0
#define		CMDERR_CMD		0x0001		//命令不支持
#define		CMDERR_LEN		0x0002		//长度错误
#define		CMDERR_PARAM	0x0003		//参数错误
#define		CMDERR_HEAD		0x0004		//命令头错误
#define		CMDERR_CRC		0x0005		//校验错误

#define		SCERR_NOCARD	0xC001		//大卡座无卡
#define		SCERR_APDU		0xC002		//APDU失败
*/
#endif

