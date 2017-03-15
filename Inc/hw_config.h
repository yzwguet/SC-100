/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : hw_config.h
* Author             : MCD Application Team
* Version            : V3.1.1
* Date               : 04/07/2010
* Description        : Hardware Configuration & Setup
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "string.h"
#include "stm32f10x.h"
#include "usb_lib.h"
#include "usb_prop.h"
#include "hw_sc.h"

//#include "usb_type.h"
//#include "lcm12232_driver.h"
//#include "sst_flash_driver.h"
//#include "keyboard_driver.h"
//#include "smartcard_old.h"
//#include "au9540_9528.h"
//#include "MagneticRead.h"

#include "cmd_process.h"
//#include "pn532.h"
//#include "des.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/

/******** USART 1 *********/

#define USART_GPIO_T1                 GPIO_Pin_9		/* GPIOA */            
#define USART_GPIO_R1                 GPIO_Pin_10		/* GPIOA */            

#define USART1_IRQn                   USART1_IRQn
#define USART1_IRQHandler			    USART1_IRQHandler
/* LCD Module*/
  #define Pin_BL                        GPIO_Pin_3		/* GPIOA */
  #define Pin_RST						GPIO_Pin_4		/* GPIOB */
  #define Pin_RW						GPIO_Pin_10    	/* GPIOC */

  #define Pin_E1						GPIO_Pin_3		/* GPIOB */
  #define Pin_E2						GPIO_Pin_2	    /* GPIOD */
    
  #define Pin_A0                        GPIO_Pin_15     /* GPIOA */ 
   
  //#define Pin_DAT                       (GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11     \
  //                                      |GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15)

  #define Pin_DAT_L                     (GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9) /*PB*/
  #define Pin_DAT_H                     (GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9) /*PC*/

/* LED */
  #define LED_Pin                       GPIO_Pin_1      /* GPIOC */
  #define LED_GPIO                      GPIOB
  #define LED_RCC                       RCC_APB2Periph_GPIOB
  
  #define LED_ON()                      GPIO_ResetBits(LED_GPIO,LED_Pin)
  #define LED_OFF()                     GPIO_SetBits  (LED_GPIO,LED_Pin)
/* Green */
#if 1
  #define LED1_Pin                       GPIO_Pin_0      /* GPIOC */
  #define LED1_GPIO                      GPIOB
  #define LED1_RCC                       RCC_APB2Periph_GPIOB
  
  #define LED1_ON()                      GPIO_ResetBits(LED1_GPIO,LED1_Pin)
  #define LED1_OFF()                     GPIO_SetBits  (LED1_GPIO,LED1_Pin)


  #define LedRedOn()					LED1_ON()	  					
  #define LedRedOff()					LED1_OFF()
  #define LedGreenOn()					LED_ON()	  					
  #define LedGreenOff()					LED_OFF()

#endif
/* BEEP */
#if 1
  #define BEEP_Pin                      GPIO_Pin_6      /* GPIOB */
  #define BEEP_GPIO                     GPIOB
  #define BEEP_RCC                      RCC_APB2Periph_GPIOB
  
  #define Beep_On()                     GPIO_ResetBits(BEEP_GPIO,BEEP_Pin)
  #define Beep_Off()                    GPIO_SetBits(BEEP_GPIO,BEEP_Pin)

  #define BeepOn()						Beep_On() 
  #define BeepOff()						Beep_Off()
#endif
/* Debug USART */
  #define DEBUG_USART                   USART1
  #define DEBUG_USART_Tx_GPIO           GPIOA
  #define DEBUG_USART_Rx_GPIO           GPIOA
  #define DEBUG_USART_Tx_Pin            GPIO_Pin_9      /* GPIOA*/
  #define DEBUG_USART_Rx_Pin            GPIO_Pin_10     /* GPIOA*/
  #define DEBUG_USART_RCC               (RCC_APB2Periph_USART1 |RCC_APB2Periph_GPIOA)

  #define DEBUG_USART_IRQn              USART1_IRQn
  #define DEBUG_USART_IRQHandler	    USART1_IRQHandler

/* SST Flash */
  
  //#define SST_WP_GPIO                   GPIO_Pin_4	/* GPIOC */
  
  #define SST_CE_GPIO                   GPIO_Pin_12		/* GPIOB */
  
  #define SST_SCK_GPIO					GPIO_Pin_13
  #define SST_SO_GPIO					GPIO_Pin_14
  #define SST_SI_GPIO					GPIO_Pin_15

/* KEY Board */
  #define KEY_BOARD_IN_GPIOx            GPIOC
  #define KEY_BOADR_IN                  (GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 |GPIO_Pin_14|GPIO_Pin_15)
  

  #define KEY_BOARD_OUT_GPIOx           GPIOC
  #define KEY_BOADR_OUT                 (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2)

/* 74HC595 */
  #define HC595_RCC                     RCC_APB2Periph_GPIOA
  #define HC595_GPIO                    GPIOA		  /*GPIOA */
  #define HC595_DAT_Pin				    GPIO_Pin_5    
  #define HC595_SH_Pin					GPIO_Pin_6
  #define HC595_ST_Pin					GPIO_Pin_7

  #define HC595_DAT(x)                  HC595_GPIO->ODR = (HC595_GPIO->ODR & ~HC595_DAT_Pin)|(x?HC595_DAT_Pin:0)
  #define HC595_CLK(x)                  HC595_GPIO->ODR = (HC595_GPIO->ODR & ~HC595_SH_Pin)|(x?HC595_SH_Pin:0)
  #define HC595_LOCK(x)                 HC595_GPIO->ODR = (HC595_GPIO->ODR & ~HC595_ST_Pin)|(x?HC595_ST_Pin:0)

#define IO_SAM1     0x00
#define IO_SAM2     0x01
#define IO_SAM3     0x02
#define IO_SAM4     0x03

#define RST_SAM1    0x01
#define RST_SAM2    0x02
#define RST_SAM3    0x04
#define RST_SAM4    0x08


  /**************PSAM USART*********************/
  #define PSAM_IO_PORT GPIOA
  #define PSAM_IO_PIN GPIO_Pin_2
  #define PSAM_CLK_PORT GPIOA
  #define PSAM_CLK_PIN GPIO_Pin_4
  
  #define PSAM_POWER_PORT GPIOA
  #define PSAM_POWER_PIN GPIO_Pin_5
  #define PSAM_RST_PORT GPIOA
  #define PSAM_RST_PIN GPIO_Pin_3

  #define PSAM_UART_RCC RCC_APB1Periph_USART2
  #define PSAM_UARTX 	USART2

  #define PSAM_UART_Frequency 4
  #define PSAM_UART_RATE	  4500000//36/8

#define SAM_PWR_OFF()	  GPIO_ResetBits(PSAM_POWER_PORT, PSAM_POWER_PIN)//断电
#define SAM_PWR_ON()      GPIO_SetBits(PSAM_POWER_PORT, PSAM_POWER_PIN)//加电

  /* PN532 IIC */
  #define I2C_SCK_PORT    GPIOA
  #define I2C_SCK_PIN     GPIO_Pin_1
  #define I2C_SDA_PORT    GPIOA
  #define I2C_SDA_PIN     GPIO_Pin_0

 /************PN532**********************/
  //#define PN532_IRQ_PORT GPIOC
  //#define PN532_IRQ_PIN GPIO_Pin_2

/* Debug USART */
  #define AU9540_USART                   USART3
  #define AU9540_USART_Tx_GPIO           GPIOB
  #define AU9540_USART_Rx_GPIO           GPIOB
  #define AU9540_USART_Tx_Pin            GPIO_Pin_10      /* GPIOB*/
  #define AU9540_USART_Rx_Pin            GPIO_Pin_11     /* GPIOB*/
  #define AU9540_USART_RCC               RCC_APB1Periph_USART3
  #define AU9540_GPIO_RCC                RCC_APB2Periph_GPIOB

  #define AU9540_USART_IRQn              USART3_IRQn
  #define AU9540_USART_IRQHandler	     USART3_IRQHandler

extern	u8		AU_Cmd_Valid ;
extern	u8		AU_Buffer[600];
extern	u16		AU_Length, AU_Count;
extern  u32     SystemTime;

extern u8	Cmd_Valid_Usart;//串口解析到一条正确的命令
extern u8 Rec_Buffer[280];	//received data buffer
extern	volatile	u32	systicnum,presystic;

#define AU_TIMEOUT		0xC001		//AU9540通信超时
#define	AU_LRCERR		0xC002		//LRC错误
#define	AU_TIME_EX		0xC003		//通信延时申请
#define	AU_SC_NPRE		0xC004		//大卡座无卡

#define	PNERR_APDU		0xC102		//APDU 失败
#define	PNERR_TYPE		0xC103		//非CPU卡
#define PNERR_NPRE		0xC104		//RF无卡
/* -------- STATUS CODE ----------- */
#define CMD_OK			0x00		//
#define CMDERR_HEAD		0x01		//命令头
#define CMDERR_LEN		0x02		//命令长度错误或程序段溢出
#define CMDERR_CRC		0x03		//
#define CMDERR_CMD		0x04		//命令不支持

#define MC_TIMEOUT		0x10		//等待磁卡刷卡超时
#define MC_READERR		0x11		//磁卡刷卡失败
#define	MC_PWROFF		0x12		//磁卡未上电
/* Exported functions ------------------------------------------------------- */
void Set_System(void);
void Set_USBClock(void);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void USB_Interrupts_Config(void);
void USB_Cable_Config (FunctionalState NewState);
void GPIO_Configuration(void);
void Get_SerialNum(void);

void LED_Config(void);  
void BEEP_Config(void);

void Debug_USART_Config(void);
void Put_Char(u8 dat);
void Put_String(u8* string,u32 length);
void SysTimeBase_Config(void);

void HC595_Config(void);
void HC595_IO_CTL(u8* buf,u8 num);

void SAM_CS(u8 cs);
void SAM_RST_H(u8 rst);
void SAM_RST_L(u8 rst);
void VOICE_CS(u8 cs);
void Delay_Ms(u16 delaytime);

void TimingDelay_Decrement(void);

void SerialPutChar(uint8_t c);
void USART1_Config(void);
void Timer2_Config(void);

void	Usart1_Chbaud(u8	baudch);

unsigned char BccResult(unsigned	char	*pArray , unsigned	short	arrayLength);
void	PCCommandValid(unsigned char *cmd_dat);
//extern	volatile	u8	Cmd_Valid,Cmd_Valid2,Bcc2,Bcc, BeepOnTime, BeepOffTime, BeepOnTimes, BeepFlag;
extern	volatile	u8	 BeepOnTime, BeepOffTime, BeepOnTimes, BeepFlag;
extern	void	Tim3_Ch(u16	dltim);
extern	void Timer3_Config(void);
extern  void MT_BeepOnce(void);
#endif  /*__HW_CONFIG_H*/

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
