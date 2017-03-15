/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : usb_desc.c
* Author             : MCD Application Team
* Version            : V3.1.1
* Date               : 04/07/2010
* Description        : Descriptors for Custom HID Demo
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

const uint8_t HID_DeviceDescriptor[HID_SIZ_DEVICE_DESC] =
  {
    0x12,                       /*bLength */
    USB_DEVICE_DESCRIPTOR_TYPE, /*bDescriptorType*/
    0x00,                       /*bcdUSB */
    0x02,
    0x00,                       /*bDeviceClass*/
    0x00,                       /*bDeviceSubClass*/
    0x00,                       /*bDeviceProtocol*/
    0x40,                       /*bMaxPacketSize40*/



    0xa4,//0xa4,                       /*idVendor (0x23a4)*/
    0X23,//0x23,//
    0x0c,//0x18,//                       /*idProduct = 0x020c*/
    0X02,//0x02,//



    0x00,                       /*bcdDevice rel. 1.00*/
    0x01,
    1,                          /*Index of string descriptor describing
    //                                          manufacturer */
    2,                          /*Index of string descriptor describing
    //                                         product*/
    3,                          /*Index of string descriptor describing the
    //                                         device serial number */
    0x01                        /*bNumConfigurations*/
  }
  ; /* HID_DeviceDescriptor */


/* USB Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
const uint8_t HID_ConfigDescriptor[HID_SIZ_CONFIG_DESC] =
  {
    0x09, /* bLength: Configuation Descriptor size */
    USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
    HID_SIZ_CONFIG_DESC,
    /* wTotalLength: Bytes returned */
    0x00,
    0x02,         /* bNumInterfaces: 1 interface */
  
	0x01,         /* bConfigurationValue: Configuration value */

	  
    0x00,         /* iConfiguration: Index of string descriptor describing
                                 the configuration*/
    0xC0,         /* bmAttributes: Bus powered */
    0xFA,//0x96,//0x32,         /* MaxPower 100 mA: this current is used for detecting Vbus */

    /************** Descriptor of Custom HID interface ****************/
    /* 09 */
    0x09,         /* bLength: Interface Descriptor size */
    USB_INTERFACE_DESCRIPTOR_TYPE,/* bDescriptorType: Interface descriptor type */
    0x00,         /* bInterfaceNumber: Number of Interface */
    0x00,         /* bAlternateSetting: Alternate setting */
    0x02,         /* bNumEndpoints */
    0x03,         /* bInterfaceClass: HID */
    0x00,         /* bInterfaceSubClass : 1=BOOT, 0=no boot */
    0x00,         /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
    0,            /* iInterface: Index of string descriptor */
    /******************** Descriptor of Custom HID HID ********************/
    /* 18 */
    0x09,         /* bLength: HID Descriptor size */
    HID_DESCRIPTOR_TYPE, /* bDescriptorType: HID */
    0x10,         /* bcdHID: HID Class Spec release number */
    0x01,
    0x00,         /* bCountryCode: Hardware target country */
    0x01,         /* bNumDescriptors: Number of HID class descriptors to follow */
    0x22,         /* bDescriptorType */
    HID_SIZ_REPORT_DESC,/* wItemLength: Total length of Report descriptor */
    0x00,
    /******************** Descriptor of Custom HID endpoints ******************/
    /* 27 */
    0x07,          /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: */

    0x81,          /* bEndpointAddress: Endpoint Address (IN) */
    0x03,          /* bmAttributes: Interrupt endpoint */
    0x40,//0x0a,//0x02,          /* wMaxPacketSize: 2 Bytes max */
    0x00,
    0x40,//0x0a,//0x01,          /* bInterval: Polling Interval (1 ms) */
    /* 34 */
    	
    0x07,	/* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,	/* bDescriptorType: */
			/*	Endpoint descriptor type */
    0x01,	/* bEndpointAddress: */
			/*	Endpoint Address (OUT) */
    0x03,	/* bmAttributes: Interrupt endpoint */
    0x40,//0x0a,//0x02,	/* wMaxPacketSize: 2 Bytes max  */
    0x00,
    0x0a,//0x20,	/* bInterval: Polling Interval (20 ms) */
    /* 41 */
#ifdef MT_KEYBOARD
	//以下为接口描述符
 /************** Descriptor of Joystick Mouse interface ****************/
 /* 09 */
 0x09,         /*bLength: Interface Descriptor size*/
 USB_INTERFACE_DESCRIPTOR_TYPE,/*bDescriptorType: Interface descriptor type*/
 0x01,         /*bInterfaceNumber: Number of Interface*/
 0x00,         /*bAlternateSetting: Alternate setting*/
 0x02,         /*bNumEndpoints*/
 0x03,         /*bInterfaceClass: HID*/
 0x01,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
 0x01,         /*bInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
 0,            /*iInterface: Index of string descriptor*/
 /* 50 */
 //以下为HID描述符
 /******************** Descriptor of Joystick Mouse HID ********************/
 /* 18 */
 0x09,         /*bLength: HID Descriptor size*/
 HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
 0x00,         /*bcdHID: HID Class Spec release number*/
 0x01,
 0x00,         /*bCountryCode: Hardware target country*/
 0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
 0x22,         /*bDescriptorType*/
 JOYSTICK_SIZ_REPORT_DESC,/*wItemLength: Total length of Report descriptor*/
 0x00,
 /* 59 */
 //以下为输入端点1描述符
 /******************** Descriptor of Joystick Mouse endpoint ********************/
 /* 27 */
 0x07,          /*bLength: Endpoint Descriptor size*/
 USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/
 //0x81,          /*bEndpointAddress: Endpoint Address (IN)*/
 0x82,          /*bEndpointAddress: Endpoint Address (IN)*/
 0x03,          /*bmAttributes: Interrupt endpoint*/
 //0x08,          /*wMaxPacketSize: 8 Byte max */
 0x10,          /*wMaxPacketSize: 8 Byte max */
 0x00,
 //0x20,          /*bInterval: Polling Interval (32 ms)*/
 //0x08,          /*bInterval: Polling Interval (32 ms)*/
 0x01,          /*bInterval: Polling Interval (32 ms)*/
 /* 66 */
 //以下为输出端但1描述符
 
 0x07,          /*bLength: Endpoint Descriptor size*/
 USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/
 0x02,          /*bEndpointAddress: Endpoint Address (OUT)*/
 0x03,          /*bmAttributes: Interrupt endpoint*/
 //0x08,          /*wMaxPacketSize: 8 Byte max */
 0x10,          /*wMaxPacketSize: 8 Byte max */
 0x00,
 0x20,
 /* 73 */
#endif
  }
  ; /* HID_ConfigDescriptor */
const uint8_t HID_ReportDescriptor[HID_SIZ_REPORT_DESC] =
  {
  0x06, 
0x00,0xff,      //Usage Tables

0x09,0x01,      //Usage
0xa1,0x01,      //Collection(Application)

0x85,0x01,      //ReportID
0x95,0x08,      //Report Count(8)
0x75,0x08,      //Report Size(8)
0x26,0xff,0x00,  //Logic Maxmum(ff)
0x15,0x00,      //Logic Minimum(0)

0x09,0x01,      //Usage  
0x91,0x02,      //Output(Data,Var,Abs)
0x85,0x02,      //ReportID
0x95,0x01,      //Report Count(1)
0x75,0x08,      //Report Size(8)
0x26,0xff,0x00,   //Logic Maxmum(ff)
0x15,0x00,      //Logic Minimum(0)

0x09,0x01,      //Usage  
0x91,0x02,      //Output(Data,Var,Abs)
0x85,0x03,      //ReportID
0x95,0x02,      //Report Count(2)
0x75,0x08,      //Report Size(8)
0x26,0xff,0x00,   //Logic Maxmum(ff)
0x15,0x00,      //Logic Minimum(0)

0x09,0x01,      //Usage  
0x91,0x02,      //Output(Data,Var,Abs)
0x85,0x04,      //ReportID
0x95,0x01,      //Report Count(1)
0x75,0x08,      //Report Size(8)
0x26,0xff,0x00,   //Logic Maxmum(ff)
0x15,0x00,      //Logic Minimum(0)

0x09,0x01,      //Usage  
0x81,0x02,      //Input(Data,Var,Abs)
0x85,0x05,      //ReportID
0x95,0x32,      //Report Count(32)
0x75,0x08,      //Report Size(8)
0x26,0xff,0x00,   //Logic Maxmum(ff)
0x15,0x00,      //Logic Minimum(0)

0x09,0x01,      //Usage
0x81,0x02,      //Input(Data,Var,Abs)
0x85,0x06,      //ReportID
0x95,0xfe,      //Report Count(fe)
0x75,0x08,      //Report Size(8)
0x26,0xff,0x00,   //Logic Maxmum(ff)
0x15,0x00,      //Logic Minimum(0)

0x09,0x01,      //Usage
0xb1,0x02,   
0x85,0x07,      //ReportID
0x95,0x32,      //Report Count(32)
0x75,0x08,      //Report Size(8)
0x26,0xff,0x00,   //Logic Maxmum(ff)
0x15,0x00,      //Logic Minimum(0)

0x09,0x01,      //Usage
0x91,0x02,      //Output(Data,Var,Abs)
0xc0         //Eed_collection

  }; /* HID_ReportDescriptor */

  
#ifdef MT_KEYBOARD
  
const u8 Joystick_ReportDescriptor[JOYSTICK_SIZ_REPORT_DESC] =
{
 0x05, 0x01, // USAGE_PAGE (Generic Desktop)
 0x09, 0x06, // USAGE (Keyboard)
 0xa1, 0x01, // COLLECTION (Application)
 0x05, 0x07, //     USAGE_PAGE (Keyboard/Keypad)
 
 
 //0x19, 0xe0, //     USAGE_MINIMUM (Keyboard LeftControl)
 //0x29, 0xe7, //     USAGE_MAXIMUM (Keyboard Right GUI)
 0x19, 0x04, //     USAGE_MINIMUM (Keyboard LeftControl)
 0x29, 0x27, //     USAGE_MAXIMUM (Keyboard Right GUI)
 0x15, 0x00, //     LOGICAL_MINIMUM (0)
 0x25, 0x01, //     LOGICAL_MAXIMUM (1)
 0x95, 0x08, //     REPORT_COUNT (8)
 //0x95, 0x0b, //     REPORT_COUNT (8)
 0x75, 0x01, //     REPORT_SIZE (1)
 0x81, 0x02, //     INPUT (Data,Var,Abs)
 
 0x95, 0x01, //     REPORT_COUNT (1)
 0x75, 0x08, //     REPORT_SIZE (8)
 0x81, 0x03, //     INPUT (Cnst,Var,Abs)
 
 0x95, 0x06, //   REPORT_COUNT (6)
 //0x95, 0x07, //   REPORT_COUNT (6)
 0x75, 0x08, //   REPORT_SIZE (8)
 0x25, 0xFF, //   LOGICAL_MAXIMUM (255)
 0x19, 0x00, //   USAGE_MINIMUM (Reserved (no event indicated))
 0x29, 0x65, //   USAGE_MAXIMUM (Keyboard Application)
 0x81, 0x00, //     INPUT (Data,Ary,Abs)
 
 0x25, 0x01, //     LOGICAL_MAXIMUM (1)
 0x95, 0x02, //   REPORT_COUNT (2)
 0x75, 0x01, //   REPORT_SIZE (1)
 0x05, 0x08, //   USAGE_PAGE (LEDs)
 0x19, 0x01, //   USAGE_MINIMUM (Num Lock)
 0x29, 0x02, //   USAGE_MAXIMUM (Caps Lock)
 0x91, 0x02, //   OUTPUT (Data,Var,Abs)
 0x95, 0x01, //   REPORT_COUNT (1)
 0x75, 0x06, //   REPORT_SIZE (6)
 0x91, 0x03, //   OUTPUT (Cnst,Var,Abs)
 
 0xc0        // END_COLLECTION
}; /* Joystick_ReportDescriptor */

#endif  
  /* USB String Descriptors (optional) */
const uint8_t HID_StringLangID[HID_SIZ_STRING_LANGID] =
  {
    HID_SIZ_STRING_LANGID,
    USB_STRING_DESCRIPTOR_TYPE,
    0x09,
    0x04
  }
  ; /* LangID = 0x0409: U.S. English */

const uint8_t HID_StringVendor[HID_SIZ_STRING_VENDOR] =	 //厂商字符串
  {
    HID_SIZ_STRING_VENDOR, /* Size of Vendor string */
    USB_STRING_DESCRIPTOR_TYPE,  /* bDescriptorType*/
    /* Manufacturer: "STMicroelectronics" */
	'M',0,'i',0,'n',0,'g',0,'T',0,'e',0,'c',0,'h',0	  
    /*'S', 0, 'T', 0, 'M', 0, 'i', 0, 'c', 0, 'r', 0, 'o', 0, 'e', 0,
    'l', 0, 'e', 0, 'c', 0, 't', 0, 'r', 0, 'o', 0, 'n', 0, 'i', 0,
    'c', 0, 's', 0 */
  };

const uint8_t HID_StringProduct[HID_SIZ_STRING_PRODUCT] =  //产品字符串
  {
    HID_SIZ_STRING_PRODUCT,          /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    //'M',0,'i',0,'n',0,'g',0,'T',0,'e',0,'c',0,'h',0,'C',0,'a',0,'r',0,'d',0,'R',0,
	//'e',0,'a',0,'d',0,'e',0,'r',0,'-',0,'C',0,'I',0,'C',0,'-',0,'V',0,'0',0,
	//'1',0,'0',0,'0',0,'-',0,'1',0,'1',0,'0',0,'5',0	 
  	'C',0,'a',0,'r',0,'d',0,' ',0,'R',0,'e',0,'a',0,'d',0,'e',0,'r',0

    /*'S', 0, 'T', 0, 'M', 0, '3', 0, '2', 0, ' ', 0, 'C', 0,
    'u', 0, 's', 0, 't', 0, 'm', 0, ' ', 0, 'H', 0, 'I', 0,
    'D', 0 */
  };
uint8_t HID_StringSerial[HID_SIZ_STRING_SERIAL] = //产品序列号
  {
    HID_SIZ_STRING_SERIAL,           /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
	'M',0,'i',0,'n',0,'g',0,'T',0,'e',0,'c',0,'h',0,
	'0',0,'0',0,'0',0,'1',0
    /*'S', 0, 'T', 0, 'M', 0,'3', 0,'2', 0, '1', 0, '0', 0 */
  };
/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/

