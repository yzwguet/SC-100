/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : usb_prop.c
* Author             : MCD Application Team
* Version            : V3.1.1
* Date               : 04/07/2010
* Description        : All processings related to Custom HID Demo
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "hw_config.h"

/* Private typedef -----------------------------------------------------------*/
extern CMD_DAT pc_cmd;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t ProtocolValue;

u8 USB_LongReportFlagt=0x00;
uint32_t USB_LongReportPack=0;

#define Max_Rec            512
#define Max_Tra            512

uint8_t Receive_Buffer[Max_Rec];
uint8_t Transceive_Buffer[Max_Tra];

u8 Receive_finish;
u8 Transceive_finish;
u8 Not_processed_flag;

u32 data_length = Max_Tra;



volatile u8 USB_NeedContinue=0;//1需要继续收包
volatile u16 USB_ReceuvePackCounter=0;//本通讯帧中的第几包，每包254字节
volatile u16 USB_NeedReceiveCounter=0;
volatile u16 USB_NeedReceuvePackCounter=0;


volatile	u8	Cmd_Valid;//Cmd_Valid为解析到一条正确的命令
/* -------------------------------------------------------------------------- */
/*  Structures initializations */
/* -------------------------------------------------------------------------- */

DEVICE Device_Table =
  {
    EP_NUM,
    1
  };

DEVICE_PROP Device_Property =
  {
    HID_init,
    HID_Reset,
    HID_Status_In,
    HID_Status_Out,
    HID_Data_Setup,
    HID_NoData_Setup,
    HID_Get_Interface_Setting,
    HID_GetDeviceDescriptor,
    HID_GetConfigDescriptor,
    HID_GetStringDescriptor,
    0,
    0x40 /*MAX PACKET SIZE*/
  };
USER_STANDARD_REQUESTS User_Standard_Requests =
  {
    HID_GetConfiguration,
    HID_SetConfiguration,
    HID_GetInterface,
    HID_SetInterface,
    HID_GetStatus,
    HID_ClearFeature,
    HID_SetEndPointFeature,
    HID_SetDeviceFeature,
    HID_SetDeviceAddress
  };

ONE_DESCRIPTOR Device_Descriptor =
  {
    (uint8_t*)HID_DeviceDescriptor,
    HID_SIZ_DEVICE_DESC
  };

ONE_DESCRIPTOR Config_Descriptor =
  {
    (uint8_t*)HID_ConfigDescriptor,
    HID_SIZ_CONFIG_DESC
  };

ONE_DESCRIPTOR HID_Report_Descriptor =
  {
    (uint8_t *)HID_ReportDescriptor,
    HID_SIZ_REPORT_DESC
  };
#ifdef MT_KEYBOARD
  ONE_DESCRIPTOR Joystick_Report_Descriptor =		 	   
  {
    (uint8_t *)Joystick_ReportDescriptor,
    JOYSTICK_SIZ_REPORT_DESC
  };
#endif
  
ONE_DESCRIPTOR HID_Hid_Descriptor =
  {
    (uint8_t*)HID_ConfigDescriptor + HID_OFF_HID_DESC,
    HID_SIZ_HID_DESC
  };

ONE_DESCRIPTOR String_Descriptor[4] =
  {
    {(uint8_t*)HID_StringLangID, HID_SIZ_STRING_LANGID},
    {(uint8_t*)HID_StringVendor, HID_SIZ_STRING_VENDOR},
    {(uint8_t*)HID_StringProduct,HID_SIZ_STRING_PRODUCT},
    {(uint8_t*)HID_StringSerial, HID_SIZ_STRING_SERIAL},
  };

/* Extern variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
u8 *HID_GetJoystick_ReportDescriptor(uint16_t Length);
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : HID_init.
* Description    : CCID init routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void HID_init(void)
{
  /* Update the serial number string descriptor with the data from the unique 
  ID*/
  Get_SerialNum();
    
  pInformation->Current_Configuration = 0;
  /* Connect the device */
  PowerOn();

  /* Perform basic device initialization operations */
  USB_SIL_Init();

  bDeviceState = UNCONNECTED;
}

/*******************************************************************************
* Function Name  : HID_Reset.
* Description    : CCID reset routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void HID_Reset(void)
{
  /* Set Joystick_DEVICE as not configured */
  pInformation->Current_Configuration = 0;
  pInformation->Current_Interface = 0;/*the default Interface*/
  
  /* Current Feature initialization */
  pInformation->Current_Feature = HID_ConfigDescriptor[7];
  
#ifdef STM32F10X_CL   
  /* EP0 is already configured in DFU_Init() by USB_SIL_Init() function */
  
  /* Init EP1 IN as Interrupt endpoint */
  OTG_DEV_EP_Init(EP1_IN, OTG_DEV_EP_TYPE_INT, 2);
  
  /* Init EP1 OUT as Interrupt endpoint */
  OTG_DEV_EP_Init(EP1_OUT, OTG_DEV_EP_TYPE_INT, 2);
#else 
  SetBTABLE(BTABLE_ADDRESS);

  /* Initialize Endpoint 0 */
  SetEPType(ENDP0, EP_CONTROL);
  SetEPTxStatus(ENDP0, EP_TX_STALL);
  SetEPRxAddr(ENDP0, ENDP0_RXADDR);
  SetEPTxAddr(ENDP0, ENDP0_TXADDR);
  Clear_Status_Out(ENDP0);
  SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
  SetEPRxValid(ENDP0);

  /* Initialize Endpoint 1 */
  SetEPType(ENDP1, EP_INTERRUPT);
  SetEPTxAddr(ENDP1, ENDP1_TXADDR);
  SetEPRxAddr(ENDP1, ENDP1_RXADDR);
  SetEPTxCount(ENDP1, 2);
  SetEPRxCount(ENDP1, 2);
  SetEPRxStatus(ENDP1, EP_RX_VALID);
  SetEPTxStatus(ENDP1, EP_TX_NAK);
  
	/* Initialize Endpoint 2 */
  SetEPType(ENDP2, EP_INTERRUPT);
  SetEPTxAddr(ENDP2, ENDP2_TXADDR);
  SetEPRxAddr(ENDP2, ENDP2_RXADDR);
  SetEPTxCount(ENDP2, 8);
  SetEPRxCount(ENDP2, 8);
  SetEPRxStatus(ENDP2, EP_RX_VALID);
  SetEPTxStatus(ENDP2, EP_TX_NAK);    
  /* Set this device to response on default address */
  SetDeviceAddress(0);
#endif /* STM32F10X_CL */

  bDeviceState = ATTACHED;
}
/*******************************************************************************
* Function Name  : HID_Status_In.
* Description    : Joystick status IN routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void HID_Status_In(void)
{
  if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))  
    && 
	pInformation->USBbRequest == SET_REPORT
	&& 
	pInformation->ControlState == WAIT_STATUS_IN)
  {				   
    Receive_finish = 1;
	Not_processed_flag = 1;
	USB_DeCode(); 
  }
}
/*******************************************************************************
* Function Name  : HID_Status_Out
* Description    : Joystick status OUT routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void HID_Status_Out (void)
{
 /* if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))  
    && pInformation->USBbRequest == GET_REPORT
	&& pInformation->ControlState == WAIT_STATUS_IN)
  {
    Transceive_finish = 1;
    
  }*/
  if(Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
  {
  	if(pInformation->USBbRequest == GET_REPORT)
	{
		//if(pInformation->ControlState == WAIT_STATUS_IN)
		Transceive_finish = 1;
	}
  }	
}

/*******************************************************************************
* Function Name  : HID_Data_Setup
* Description    : Handle the data class specific requests.
* Input          : Request Nb.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT HID_Data_Setup(uint8_t RequestNo)
{
  uint8_t *(*CopyRoutine)(uint16_t);

  CopyRoutine = NULL;

  if ((RequestNo == GET_DESCRIPTOR)
      && (Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT))
      && (pInformation->USBwIndex0 == 0))
  {

    if (pInformation->USBwValue1 == REPORT_DESCRIPTOR)
    {
      CopyRoutine = HID_GetReportDescriptor;
    }
    else if (pInformation->USBwValue1 == HID_DESCRIPTOR_TYPE)
    {
      CopyRoutine = HID_GetHIDDescriptor;
    }

  } /* End of GET_DESCRIPTOR */
#ifdef MT_KEYBOARD
	else
	if ((RequestNo == GET_DESCRIPTOR)
      && (Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT))
      && (pInformation->USBwIndex0 == 1))
  {

    if (pInformation->USBwValue1 == REPORT_DESCRIPTOR)
    {
      CopyRoutine = HID_GetJoystick_ReportDescriptor;		  

    }
    else if (pInformation->USBwValue1 == HID_DESCRIPTOR_TYPE)
    {
      CopyRoutine = HID_GetHIDDescriptor;
    }

  } /* End of GET_DESCRIPTOR */	
#else	

#endif
  /*** GET_PROTOCOL ***/
  else if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
           && RequestNo == GET_PROTOCOL)
  {
    CopyRoutine = HID_GetProtocolValue;
  }
  /*** GET_REPORT ***/
  else if((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
        && RequestNo == GET_REPORT)
  {
	if(Not_processed_flag)
	{
	    return USB_NOT_READY;
	}
    CopyRoutine = HID_GetReport;
  }
  
  /**** SET_REPORT ****/
  else if((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
        && RequestNo == SET_REPORT)
  {
    CopyRoutine = HID_SetReport;
  }

  if (CopyRoutine == NULL)
  {
    return USB_UNSUPPORT;
  }

  pInformation->Ctrl_Info.CopyData = CopyRoutine;
  pInformation->Ctrl_Info.Usb_wOffset = 0;
  (*CopyRoutine)(0);
  return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : HID_NoData_Setup
* Description    : handle the no data class specific requests
* Input          : Request Nb.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT HID_NoData_Setup(uint8_t RequestNo)
{
  if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
      && (RequestNo == SET_PROTOCOL)
      )
  {
    return HID_SetProtocol();
  }

  else
  {
    return USB_UNSUPPORT;
  }
}
/*******************************************************************************
* Function Name  : HID_Get_Interface_Setting.
* Description    : tests the interface and the alternate setting according to the
*                  supported one.
* Input          : - Interface : interface number.
*                  - AlternateSetting : Alternate Setting number.
* Output         : None.
* Return         : USB_SUCCESS or USB_UNSUPPORT.
*******************************************************************************/
RESULT HID_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting)
{
  if (AlternateSetting > 0)
  {
    return USB_UNSUPPORT;
  }
  else if (Interface > 0)
  {
    return USB_UNSUPPORT;
  }
  return USB_SUCCESS;
}
/*******************************************************************************
* Function Name  : HID_GetDeviceDescriptor.
* Description    : Gets the device descriptor.
* Input          : Length
* Output         : None.
* Return         : The address of the device descriptor.
*******************************************************************************/
uint8_t *HID_GetDeviceDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Device_Descriptor);
}

/*******************************************************************************
* Function Name  : HID_GetConfigDescriptor.
* Description    : Gets the configuration descriptor.
* Input          : Length
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
uint8_t *HID_GetConfigDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Config_Descriptor);
}

/*******************************************************************************
* Function Name  : HID_GetStringDescriptor
* Description    : Gets the string descriptors according to the needed index
* Input          : Length
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
uint8_t *HID_GetStringDescriptor(uint16_t Length)
{
  uint8_t wValue0 = pInformation->USBwValue0;
  if (wValue0 > 4)
  {
    return NULL;
  }
  else 
  {
    return Standard_GetDescriptorData(Length, &String_Descriptor[wValue0]);
  }
}
/*******************************************************************************
* Function Name  : HID_SetConfiguration.
* Description    : Udpade the device state to configured and command the ADC 
*                  conversion.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void HID_SetConfiguration(void)
{
  if (pInformation->Current_Configuration != 0)
  {
    /* Device configured */
    bDeviceState = CONFIGURED;
  }
}
/*******************************************************************************
* Function Name  : HID_SetConfiguration.
* Description    : Udpade the device state to addressed.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void HID_SetDeviceAddress (void)
{
  bDeviceState = ADDRESSED;
}
/*******************************************************************************
* Function Name  : HID_GetReportDescriptor.
* Description    : Gets the HID report descriptor.
* Input          : Length
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
uint8_t *HID_GetReportDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &HID_Report_Descriptor);
}
#ifdef MT_KEYBOARD
uint8_t *HID_GetJoystick_ReportDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Joystick_Report_Descriptor);
}	
#endif
/*******************************************************************************
* Function Name  : HID_GetHIDDescriptor.
* Description    : Gets the HID descriptor.
* Input          : Length
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
uint8_t *HID_GetHIDDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &HID_Hid_Descriptor);
}
/*******************************************************************************
* Function Name  : HID_SetProtocol
* Description    : Joystick Set Protocol request routine.
* Input          : None.
* Output         : None.
* Return         : USB SUCCESS.
*******************************************************************************/
RESULT HID_SetProtocol(void)
{
  uint8_t wValue0 = pInformation->USBwValue0;
  ProtocolValue = wValue0;
  return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : HID_GetProtocolValue
* Description    : get the protocol value
* Input          : Length.
* Output         : None.
* Return         : address of the protcol value.
*******************************************************************************/
uint8_t *HID_GetProtocolValue(uint16_t Length)
{
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = 1;
    return NULL;
  }
  else
  {
    return (uint8_t *)(&ProtocolValue);
  }
}
/*******************************************************************************
* Function Name  : CustomHID_GetReport.
* Description    : Gets the device feature report.
* Input          : Length
* Output         : None.
* Return         : The address of the feature.
*******************************************************************************/
uint8_t *HID_GetReport(uint16_t Length)
{
  uint32_t  wOffset;//,tmp;

  wOffset = pInformation->Ctrl_Info.Usb_wOffset;
  if (Length == 0)
  {
  	if(data_length >= 255)						 //////
	{
		pInformation->Ctrl_Info.Usb_wLength = 255 - wOffset;
	 	data_length -= 255;
	}
	else 
	{
		pInformation->Ctrl_Info.Usb_wLength = data_length - wOffset;
		data_length = 0;	
	}
    return 0;
  }
  //-------------------------
  if(USB_LongReportFlagt==0x01)//长报告
  {
  	if(wOffset == 0)
	{
		wOffset = wOffset+USB_LongReportPack*255;
	    USB_LongReportPack += 1;	
	}
	else 
		wOffset=wOffset + (USB_LongReportPack-1)*255;	
  }
  return (Transceive_Buffer + wOffset);
 
}


/*******************************************************************************
* Function Name  : CustomHID_SetReport.
* Description    : Sets the device feature report.
* Input          : Length
* Output         : None.
* Return         : The address of the feature.
*******************************************************************************/
uint8_t *HID_SetReport(uint16_t Length)
{
  uint32_t  wOffset;

  wOffset = pInformation->Ctrl_Info.Usb_wOffset;	 
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = 255 - wOffset;
    return 0;
  }
  
  if(USB_NeedContinue != 0)
  	wOffset = wOffset + USB_ReceuvePackCounter * 255;	
  
  return (Receive_Buffer + wOffset);  
}

/*******************************************************************************
* Function Name  : TransmitToPC.
* Description    : Transmit the Data to PC.
* Input          : Length
* Output         : None.
* Return         : The address of the feature.
*******************************************************************************/
#define	PStx		0x02
#define	PEtx		0x03


//USB_LongReportFlagt		   :0：短包，1：长包

//USB_NeedContinue			   :0：短包，1：长包
//USB_ReceuvePackCounter	   :接收包计数
//USB_NeedReceuvePackCounter   :需要接收包计数

//Receive_finish
//Not_processed_flag
//Cmd_Valid
void USB_DeCode(void)
{
	//u16 len;
	u8 bcc;

	if(Receive_finish)
	{
	    Receive_finish = 0;
		if(USB_NeedContinue == 0)//起始包
		{						
			if((Receive_Buffer[1]) != PStx)
			    return;	  
			pc_cmd.len = (Receive_Buffer[2] << 8) + Receive_Buffer[3];
			
			//只有一包，作好校验，处理
			if((pc_cmd.len + 6) <= 255)			
			//if((pc_cmd.len + 6) <= 254)			
			{				
				bcc = BccResult(&Receive_Buffer[4], pc_cmd.len);
				pc_cmd.bcc = Receive_Buffer[pc_cmd.len+4];
				if(bcc != pc_cmd.bcc)
					return;	
				memcpy(&pc_cmd.stx,&Receive_Buffer[1],(pc_cmd.len+3));
				pc_cmd.len = __REV16(pc_cmd.len);
				pc_cmd.cmd = __REV16(pc_cmd.cmd);
				
				//完成检测
				USB_NeedContinue=0;
				USB_NeedReceuvePackCounter=0;
				USB_ReceuvePackCounter=0;				
				//USB_LongReportFlagt=0x00; //缺省为短包,发送时用				
				//Not_processed_flag=1;	
				Cmd_Valid = 0x01;
			}
			//有超过一包的数据，为第二包做好准备
			else 
			{
				memcpy(&pc_cmd.stx,&Receive_Buffer[1],254);
				pc_cmd.len = __REV16(pc_cmd.len);
				pc_cmd.cmd = __REV16(pc_cmd.cmd);
				USB_NeedContinue=1;			 
				USB_NeedReceuvePackCounter=(pc_cmd.len+6)/255; 
				USB_ReceuvePackCounter=1;				
				//USB_LongReportFlagt=0x00; //缺省为短包,发送时用				
				//Not_processed_flag=1;
				Cmd_Valid=0x00;				
			}
		}
		else//后续包
		{
			memcpy(&pc_cmd.dat[248],&Receive_Buffer[256],pc_cmd.len - 251 + 2);

			USB_NeedReceuvePackCounter -= 1;
			USB_ReceuvePackCounter += 1;
			if(USB_NeedReceuvePackCounter == 0)//最后一包
			{				
				bcc = BccResult(&pc_cmd.stx,pc_cmd.len+3);
				bcc ^= 0x02;
				bcc ^= pc_cmd.len >> 8;
				bcc ^= pc_cmd.len;
				//if(bcc!=pc_cmd.dat[__REV16(pc_cmd.len)-5])
				if(bcc != pc_cmd.dat[pc_cmd.len-3])
				{
				 	USB_NeedContinue=0;
					USB_NeedReceuvePackCounter=0;
					Cmd_Valid=0x00;
					USB_ReceuvePackCounter=0; 
				}
				else
				{
					USB_NeedContinue=0;
					USB_NeedReceuvePackCounter=0;
					USB_ReceuvePackCounter=0;
					Cmd_Valid=0x01;

				}
			}
		}
	}
}
void USB_Command_ReplyTogether(u8 *buf,u16 len)	  //传进来的是数据长度，只需调分包处
{

    //if(len<254)
	if(len<247)				   //此处 250-252 OK	249
	    USB_LongReportFlagt=0;
	else
	    USB_LongReportFlagt=1;

	if(USB_LongReportFlagt == 0)
	{
	    //len += 6;					 //stx len sta uused
		memcpy(&Transceive_Buffer[1],buf,len+6);
	    Transceive_Buffer[0]=0x06;
		Transceive_Buffer[2]=(len+3)>>8;
		Transceive_Buffer[3]=(len+3);	
        //Transceive_Buffer[len+1] = BccResult(buf,len);
		Transceive_Buffer[len+7] = BccResult(buf+3,len+3);
		Transceive_Buffer[len+8] = 0x03;
		//if(len == 247)
		//{
		//	Transceive_Buffer[len+9] = 0x06;
		//	data_length = len +10;
		//}
		//else
	    	data_length = len + 9;	  //data_length最大255 246
	    Not_processed_flag = 0;	
	    USB_LongReportPack=0;
	    _SetEPTxStatus(ENDP0, EP_TX_VALID);
	}
	else
	{
	    memcpy(&Transceive_Buffer[1],buf,254);
	    Transceive_Buffer[0]=0x06;	
		Transceive_Buffer[2]=(len+3)>>8;
		Transceive_Buffer[3]=(len+3);
		Transceive_Buffer[255+0]=0x06;	
		memcpy(&Transceive_Buffer[255+1],&buf[254],(len - 247));
		if(len == 247)
        	Transceive_Buffer[len+7] = BccResult(buf+3,len+3);
		else
			Transceive_Buffer[len+8] = BccResult(buf+3,len+3);
	    Transceive_Buffer[len+9] = 0x03;
	    data_length = len + 10;
	    Not_processed_flag = 0;	
	    USB_LongReportPack=0;
	    _SetEPTxStatus(ENDP0, EP_TX_VALID);
	}


}

void	SendToKB(u8	ch)
{
	u8	i, Buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	u8	chgchar[10] = {0x27, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26};
	
	if(ch == 0x3d)
		Buffer[2] = 0x2e;
	else
		if((ch == 0x0a)||(ch == 0x0d))
			Buffer[2] = 0x28;
	else	
		if((ch >= 0x30) && (ch <= 0x39))
			Buffer[2] = chgchar[ch-0x30];
	else
		if(ch == 0x4f)
			Buffer[2] = 0x12;
	else	
		Buffer[2] = 0;
	UserToPMABufferCopy(Buffer, GetEPTxAddr(ENDP2), 8);
	SetEPTxValid(ENDP2);
	while(GetEPTxStatus((ENDP2&0x7F)) != EP_TX_NAK);
	for(i=0; i<8; i++)
		Buffer[i] = 0;
	UserToPMABufferCopy(Buffer, GetEPTxAddr(ENDP2), 8);
	SetEPTxValid(ENDP2);
	while(GetEPTxStatus((ENDP2&0x7F)) != EP_TX_NAK);
}
/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
