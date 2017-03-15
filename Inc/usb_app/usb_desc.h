/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : usb_desc.h
* Author             : MCD Application Team
* Version            : V3.1.1
* Date               : 04/07/2010
* Description        : Descriptor Header for Custom HID Demo
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_DESC_H
#define __USB_DESC_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/

//config a moni keyboard for mingtech  add by yzw 2015/5/14
#define MT_KEYBOARD

#define USB_DEVICE_DESCRIPTOR_TYPE              0x01 //Éè±¸ÃèÊö·û
#define USB_CONFIGURATION_DESCRIPTOR_TYPE       0x02 //ÅäÖÃÃèÊö·û
#define USB_STRING_DESCRIPTOR_TYPE              0x03 //×Ö·û´®ÃèÊö·û
#define USB_INTERFACE_DESCRIPTOR_TYPE           0x04 //½Ó¿ÚÃèÊö·û
#define USB_ENDPOINT_DESCRIPTOR_TYPE            0x05 //¶ËµãÃèÊö·û

#define HID_DESCRIPTOR_TYPE                     0x21 //HIDÃèÊö·û
#define HID_SIZ_HID_DESC                        0x09 
#define HID_OFF_HID_DESC                        0x12

#define HID_SIZ_DEVICE_DESC               18
#ifdef MT_KEYBOARD
	#define HID_SIZ_CONFIG_DESC               73//93//261
#else
	#define HID_SIZ_CONFIG_DESC               41//93//261
#endif
#define HID_SIZ_REPORT_DESC               0x71//162
#define HID_SIZ_STRING_LANGID             4
#define HID_SIZ_STRING_VENDOR             38
#define HID_SIZ_STRING_PRODUCT            24//68//32
#define HID_SIZ_STRING_SERIAL             26


#define STANDARD_ENDPOINT_DESC_SIZE        0x09

#ifdef MT_KEYBOARD
	#define JOYSTICK_SIZ_REPORT_DESC                61
#endif


/* Exported functions ------------------------------------------------------- */
extern const uint8_t HID_DeviceDescriptor[HID_SIZ_DEVICE_DESC];
extern const uint8_t HID_ConfigDescriptor[HID_SIZ_CONFIG_DESC];
extern const uint8_t HID_ReportDescriptor[HID_SIZ_REPORT_DESC];

extern const uint8_t HID_StringLangID[HID_SIZ_STRING_LANGID];
extern const uint8_t HID_StringVendor[HID_SIZ_STRING_VENDOR];
extern const uint8_t HID_StringProduct[HID_SIZ_STRING_PRODUCT];
extern uint8_t HID_StringSerial[HID_SIZ_STRING_SERIAL];
extern const uint8_t Joystick_ReportDescriptor[JOYSTICK_SIZ_REPORT_DESC];

#endif /* __USB_DESC_H */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
