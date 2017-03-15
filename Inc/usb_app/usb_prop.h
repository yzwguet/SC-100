/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : usb_prop.h
* Author             : MCD Application Team
* Version            : V3.1.1
* Date               : 04/07/2010
* Description        : All processings related to Custom HID demo
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_PROP_H
#define __USB_PROP_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef enum _HID_REQUESTS
{
  GET_REPORT = 1,
  GET_IDLE,
  GET_PROTOCOL,

  SET_REPORT = 9,
  SET_IDLE,
  SET_PROTOCOL
} HID_REQUESTS;


/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void HID_init(void);
void HID_Reset(void);
void HID_SetConfiguration(void);

void HID_GetStatus(void);

void HID_SetDeviceAddress (void);
void HID_Status_In (void);
void HID_Status_Out (void);
RESULT HID_Data_Setup(uint8_t);
RESULT HID_NoData_Setup(uint8_t);
RESULT HID_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting);
uint8_t *HID_GetDeviceDescriptor(uint16_t );
uint8_t *HID_GetConfigDescriptor(uint16_t);
uint8_t *HID_GetStringDescriptor(uint16_t);
RESULT HID_SetProtocol(void);
uint8_t *HID_GetProtocolValue(uint16_t Length);
RESULT HID_SetProtocol(void);
uint8_t *HID_GetReportDescriptor(uint16_t Length);
uint8_t *HID_GetHIDDescriptor(uint16_t Length);

uint8_t *HID_GetReport(uint16_t Length);
uint8_t *HID_SetReport(uint16_t Length);

//******************************************************************************

void USB_DeCode(void);
void USB_Command_ReplyTogether(u8 *buf,u16 len);

/* Exported define -----------------------------------------------------------*/
#define HID_GetConfiguration          NOP_Process
//#define HID_SetConfiguration          NOP_Process
#define HID_GetInterface              NOP_Process
#define HID_SetInterface              NOP_Process
#define HID_GetStatus                 NOP_Process
#define HID_ClearFeature              NOP_Process
#define HID_SetEndPointFeature        NOP_Process
#define HID_SetDeviceFeature          NOP_Process
//#define HID_SetDeviceAddress          NOP_Process

#define REPORT_DESCRIPTOR                   0x22

#endif /* __USB_PROP_H */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
