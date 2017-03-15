/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : platform_config.h
* Author             : MCD Application Team
* Version            : V2.0.0
* Date               : 06/16/2008
* Description        : Evaluation board specific configuration file.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PLATFORM_CONFIG_H
#define __PLATFORM_CONFIG_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line corresponding to the STMicroelectronics evaluation board
   used to run the example */
#if !defined (USE_STM3210B_EVAL) &&  !defined (USE_STM3210E_EVAL)
 //#define USE_STM3210B_EVAL
 #define USE_STM3210E_EVAL
#endif

/* Define the STM32F10Xxx Flash page size depending on the used STM32 device */
#ifdef MW_MT3_C8		   //64*1k
  #define PAGE_SIZE  (u16)0x400  /* Page size = 1KByte */
  #define MT3_STAddress  (u16)0x0800F800  /* Page size = 1KByte */
  #define MT3_NWAddress  (u16)0x0800FBFF  /* Page size = 1KByte */
  #define MT3_EDAddress  (u16)0x0800FFFF  /* Page size = 1KByte */
#elif defined MW_MT3_CB	//128*1k
  #define PAGE_SIZE  (u16)0x400  /* Page size = 2KByte */
  #define MT3_STAddress  (u16)0x0801F800  /* Page size = 1KByte */
  #define MT3_NWAddress  (u16)0x0801FBFF  /* Page size = 1KByte */
  #define MT3_EDAddress  (u16)0x0801FFFF  /* Page size = 1KByte */
#elif defined MW_MT3_RB	   //128*1k
  #define PAGE_SIZE  (u16)0x400  /* Page size = 2KByte */
  #define MT3_STAddress  (u16)0x0801F800  /* Page size = 1KByte */
  #define MT3_NWAddress  (u16)0x0801FBFF  /* Page size = 1KByte */
  #define MT3_EDAddress  (u16)0x0801FFFF  /* Page size = 1KByte */
#elif defined MW_MT3_RC	   //128*2k
  #define PAGE_SIZE  (u16)0x800  /* Page size = 2KByte */
  #define MT3_STAddress  (u16)0x0803F000  /* Page size = 2KByte */
  #define MT3_NWAddress  (u16)0x0803F7FF  /* Page size = 2KByte */
  #define MT3_EDAddress  (u16)0x0803FFFF  /* Page size = 2KByte */
#endif /* USE_STM3210B_EVAL */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __PLATFORM_CONFIG_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
