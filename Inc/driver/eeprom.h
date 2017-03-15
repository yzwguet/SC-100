/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : eeprom.h
* Author             : MCD Application Team
* Version            : V2.0.0
* Date               : 06/16/2008
* Description        : This file contains all the functions prototypes for the
*                      EEPROM emulation firmware library.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EEPROM_H
#define __EEPROM_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"


/************************************************************** 
处理器定义，与flash结构有关，用的是最后两块
可选择为:
MW_MT3_C8
MW_MT3_CB
MW_MT3_RB
MW_MT3_RC
**************************************************************/
#define	MW_MT3_C8
/* Used Flash pages for EEPROM emulation */


/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/*************************************************************
flash作E2初始化函数，需要在用作e2的flash读写前初始化
*************************************************************/
u16 EE_Init(void);
/*************************************************************
flash作E2初始化函数，需要在用作e2的flash读写前初始化
*************************************************************/
u16 EE_ReadVariable(u16 VirtAddress, u16* Data);
/*************************************************************
flash作E2初始化函数，需要在用作e2的flash读写前初始化
*************************************************************/
u16 EE_WriteVariable(u16 VirtAddress, u16 Data);

#endif /* __EEPROM_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
