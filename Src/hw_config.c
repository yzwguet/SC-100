/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : hw_config.c
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

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "platform_config.h"
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
//#include "stm32_eval.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ErrorStatus HSEStartUpStatus;
extern u8 Rec_Buffer[280];
extern CMD_DAT pc_cmd;

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Set_System
* Description    : Configures Main system clocks & power.
* Input          : None.
* Return         : None.
*******************************************************************************/
void Set_System(void)
{
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/   
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if (HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
 
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);
    
    /* ADCCLK = PCLK2/8 */
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);    

#ifdef STM32F10X_CL
    /* Configure PLLs *********************************************************/
    /* PLL2 configuration: PLL2CLK = (HSE / 5) * 8 = 40 MHz */
    RCC_PREDIV2Config(RCC_PREDIV2_Div5);
    RCC_PLL2Config(RCC_PLL2Mul_8);

    /* Enable PLL2 */
    RCC_PLL2Cmd(ENABLE);

    /* Wait till PLL2 is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLL2RDY) == RESET)
    {}

    /* PLL configuration: PLLCLK = (PLL2 / 5) * 9 = 72 MHz */ 
    RCC_PREDIV1Config(RCC_PREDIV1_Source_PLL2, RCC_PREDIV1_Div5);
    RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);
#else
    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
#endif

    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock configuration.
       User can add here some code to deal with this error */    

    /* Go to infinite loop */
    while (1)
    {
    }
  }
 
  /* Configure the USB_DISCONNECT GPIO*/
  GPIO_Configuration();
  

}

/*******************************************************************************
* Function Name  : Set_USBClock
* Description    : Configures USB Clock input (48MHz).
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Set_USBClock(void)
{
#ifdef STM32F10X_CL
  /* Select USBCLK source */
  RCC_OTGFSCLKConfig(RCC_OTGFSCLKSource_PLLVCO_Div3);

  /* Enable the USB clock */ 
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_OTG_FS, ENABLE) ;
#else
  /* Select USBCLK source */
  RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
  
  /* Enable the USB clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
#endif /* STM32F10X_CL */
}

/*******************************************************************************
* Function Name  : Enter_LowPowerMode.
* Description    : Power-off system clocks and power while entering suspend mode.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Enter_LowPowerMode(void)
{
  /* Set the device state to suspend */
  bDeviceState = SUSPENDED;
}

/*******************************************************************************
* Function Name  : Leave_LowPowerMode.
* Description    : Restores system clocks and power while exiting suspend mode.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Leave_LowPowerMode(void)
{
  DEVICE_INFO *pInfo = &Device_Info;
  
  /* Set the device state to the correct state */
  if (pInfo->Current_Configuration != 0)
  {
    /* Device configured */
    bDeviceState = CONFIGURED;
  }
  else 
  {
    bDeviceState = ATTACHED;
  }
}

/*******************************************************************************
* Function Name  : USB_Interrupts_Config.
* Description    : Configures the USB interrupts.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Interrupts_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure; 
  
  /* 2 bit for pre-emption priority, 2 bits for subpriority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  
  
#ifdef STM32F10X_CL
  /* Enable the USB Interrupts */
  NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);   
#else
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif /* STM32F10X_CL */
 
}

/*******************************************************************************
* Function Name  : USB_Cable_Config.
* Description    : Software Connection/Disconnection of USB Cable.
* Input          : NewState: new state.
* Output         : None.
* Return         : None
*******************************************************************************/
void USB_Cable_Config (FunctionalState NewState)
{ 
  
  if (NewState != DISABLE)
  {
    GPIO_ResetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);

  }
  else
  {
    GPIO_SetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
  }
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_DISCONNECT | RCC_APB2Periph_AFIO,ENABLE);  
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);

  /* USB_DISCONNECT used as USB pull-up */
  GPIO_InitStructure.GPIO_Pin = USB_DISCONNECT_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(USB_DISCONNECT, &GPIO_InitStructure);
  
  GPIO_SetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);  
}
/*******************************************************************************
* Function Name  : Get_SerialNum.
* Description    : Create the serial number string descriptor.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Get_SerialNum(void)
{
  uint32_t Device_Serial0, Device_Serial1, Device_Serial2;
  
  Device_Serial0 = *(uint32_t*)(0x1FFFF7E8);
  Device_Serial1 = *(uint32_t*)(0x1FFFF7EC);
  Device_Serial2 = *(uint32_t*)(0x1FFFF7F0);
  
  if(Device_Serial0 != 0)
  {
     HID_StringSerial[2] = (uint8_t)(Device_Serial0 & 0x000000FF);  
     HID_StringSerial[4] = (uint8_t)((Device_Serial0 & 0x0000FF00) >> 8);
     HID_StringSerial[6] = (uint8_t)((Device_Serial0 & 0x00FF0000) >> 16);
     HID_StringSerial[8] = (uint8_t)((Device_Serial0 & 0xFF000000) >> 24);  
     
     HID_StringSerial[10] = (uint8_t)(Device_Serial1 & 0x000000FF);  
     HID_StringSerial[12] = (uint8_t)((Device_Serial1 & 0x0000FF00) >> 8);
     HID_StringSerial[14] = (uint8_t)((Device_Serial1 & 0x00FF0000) >> 16);
     HID_StringSerial[16] = (uint8_t)((Device_Serial1 & 0xFF000000) >> 24); 
     
     HID_StringSerial[18] = (uint8_t)(Device_Serial2 & 0x000000FF);  
     HID_StringSerial[20] = (uint8_t)((Device_Serial2 & 0x0000FF00) >> 8);
     HID_StringSerial[22] = (uint8_t)((Device_Serial2 & 0x00FF0000) >> 16);
     HID_StringSerial[24] = (uint8_t)((Device_Serial2 & 0xFF000000) >> 24); 
  }   
}
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void LED_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
  
  	RCC_APB2PeriphClockCmd(LED_RCC | LED1_RCC,ENABLE);
	GPIO_InitStructure.GPIO_Pin = LED_Pin | LED1_Pin;	//GPIO_TxPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LED_GPIO, &GPIO_InitStructure);
	//LedGreenOn();
	LedGreenOff();
	//LedRedOn();
	LedRedOff();
}
void BEEP_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  	RCC_APB2PeriphClockCmd(BEEP_RCC,ENABLE);    

	GPIO_InitStructure.GPIO_Pin = BEEP_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;//GPIO_Mode_Out_PP;
	GPIO_Init(BEEP_GPIO, &GPIO_InitStructure);
	//BeepOn();
	BeepOff();

}
/*
 *******************************************************************************
 * @brief  Put_Char 
 * @param  none
 * @retval none  
 *******************************************************************************
 */
void Put_Char(u8 dat)
{
    DEBUG_USART->DR = (dat & (uint16_t)0x01FF);
	while(USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);
}
/*
 *******************************************************************************
 * @brief  Put_String 
 * @param  none
 * @retval none  
 *******************************************************************************
 */
void Put_String(u8* string,u32 length)
{
    while(length-- != 0)
	{
	    DEBUG_USART->DR = ((*(string++)) & (uint16_t)0x01FF);
	    while(USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);
	}
}
/*******************************************************************************
* Function Name  : Delay
* Description    : Inserts a delay time.
* Input          : nCount: specifies the delay time length.
* Output         : None
* Return         : None
*******************************************************************************/
static __IO uint32_t TimingDelay;

void	Delay_Ms(u16	delaytime)
{
	presystic = systicnum + delaytime;
	//while(presystic >= systicnum);
	while(presystic > systicnum);
}
/*******************************************************************************
* Function Name  : BccResult.
* Description    : BccResult routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
unsigned	char	BccResult(unsigned	char	*pArray , unsigned	short	arrayLength)
{
	unsigned	char	res;
	unsigned	short	arrayCount;
	res = 0x00;
	for(arrayCount = 0x00; arrayCount < arrayLength ; arrayCount++)
	{
		res ^= pArray[arrayCount];	
	}
	return	res;	
}
/*******************************************************************************
* Function Name  : 串口通讯命令解析
* Description    : 
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void	PCCommandValid(unsigned char *cmd_dat)		 
{
	pc_cmd.stx = cmd_dat[0];
	pc_cmd.len = (cmd_dat[1] << 8) + cmd_dat[2];
	pc_cmd.cmd = (cmd_dat[3] << 8) + cmd_dat[4];   
	pc_cmd.unused = cmd_dat[5];
	memcpy(pc_cmd.dat,cmd_dat+6,(pc_cmd.len-3));	
	return;			
}
void Timer2_Config(void)	 //1ms原始的
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; 
    //  TIM_OCInitTypeDef  TIM_OCInitStructure ;
	NVIC_InitTypeDef NVIC_InitStructure; 

    TIM_DeInit( TIM2);                               
     
    /* TIM2 clock enable [TIM2定时器允许]*/ 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    /* TIM2 configuration */ 
    TIM_TimeBaseStructure.TIM_Period = 4999;//199;     //10ms用作字节间    200*3600/72000000     
    TIM_TimeBaseStructure.TIM_Prescaler = 7199;//3599;    	    
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 

    /* Clear TIM2 update pending flag*/ 
    TIM_ClearFlag(TIM2, TIM_FLAG_Update); 

    /* Enable TIM2 Update interrupt*/ 
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);   

    /* TIM2 enable counter */ 
   // TIM_Cmd(TIM2, ENABLE);
	TIM_Cmd(TIM2, DISABLE); 

    /* Configure the NVIC Preemption Priority Bits */   
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 
   
    /* Enable the TIM2 gloabal Interrupt */ 
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;   
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure);
	TIM_Cmd(TIM2, ENABLE);  
}
/////显示时间控制定时器tim初始化，时基为1s,作数码管内部调用，
void Timer3_Config(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	NVIC_InitTypeDef NVIC_InitStructure; 

    TIM_DeInit(TIM3);                               
     
    /* TIM2 clock enable [TIM2定时器允许]*/ 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    /* TIM2 configuration */ 
    TIM_TimeBaseStructure.TIM_Period = 199;     //10ms用作字节间    200*3600/72000000*1000      
    TIM_TimeBaseStructure.TIM_Prescaler = 3599;    	    
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 

    /* Clear TIM2 update pending flag*/ 
    TIM_ClearFlag(TIM3, TIM_FLAG_Update); 

    /* Enable TIM2 Update interrupt*/ 
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);   

    /* TIM2 enable counter */ 
    //TIM_Cmd(TIM3, ENABLE);
	TIM_Cmd(TIM3, DISABLE); 

    /* Configure the NVIC Preemption Priority Bits */   
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 
   
    /* Enable the TIM2 gloabal Interrupt */ 
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;   
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure);
}
void	Tim3_Ch(u16	dltim)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; 

	NVIC_InitTypeDef NVIC_InitStructure; 

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_DeInit(TIM3);                               
    
	TIM_Cmd(TIM3, DISABLE); 
    /* TIM2 clock enable [TIM2定时器允许]*/ 
    
    /* TIM2 configuration */ 
    //TIM_TimeBaseStructure.TIM_Period = 20 * dltim -1;     //1ms用作字节间    20*3600/72000000*1000      
    //TIM_TimeBaseStructure.TIM_Prescaler = 3599; 
	TIM_TimeBaseStructure.TIM_Period = (200 * dltim -1);     //1ms用作字节间    20*3600/72000000*1000 
	//TIM_TimeBaseStructure.TIM_Period = 20 ;      
    TIM_TimeBaseStructure.TIM_Prescaler = 35999;   	    
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 

    /* Clear TIM2 update pending flag*/ 
    TIM_ClearFlag(TIM3, TIM_FLAG_Update); 

    /* Enable TIM2 Update interrupt*/ 
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);   

    /* TIM2 enable counter */ 
	TIM_Cmd(TIM3, DISABLE); 

    /* Configure the NVIC Preemption Priority Bits */   
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 
   
    /* Enable the TIM2 gloabal Interrupt */ 
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;   
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure);
	TIM_Cmd(TIM3, ENABLE);
}
/*
 *******************************************************************************
 * @brief  USART1_Configuration 
 * @param  none
 * @retval none  
 *******************************************************************************
 */
void USART1_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  USART_InitTypeDef USART_InitStructure;


  /* CLOCK Configuration ******************************************************/
  /* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1 , ENABLE); 
  

  /* GPIO Configuration *******************************************************/
  /* Configure USART Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = USART_GPIO_T1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
    
  /* Configure USART Rx as input floating */
  GPIO_InitStructure.GPIO_Pin = USART_GPIO_R1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure the NVIC Preemption Priority Bits */  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
    /* Enable the NFC_USART Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	    
  /* USART Configuration ******************************************************/  	 
	   /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 9600;//115200;//256000;//115200;//9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  /* USART configuration */
  USART_Init(USART1, &USART_InitStructure);

  /* Enable USART Receive and Transmit interrupts */
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    
  /* Enable USART */
  USART_Cmd(USART1, ENABLE);   
}
void SerialPutChar(uint8_t c)
{
  USART_SendData(USART1, c);
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}
/*************波特率设置********************/
void	Usart1_Chbaud(u8	baudch)
{

	USART_InitTypeDef USART_InitStructure;	
	u32	baudnum;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1 , ENABLE); 
	USART_Cmd(USART1, DISABLE);   
	switch(baudch)
	{
		case	0x00:
			baudnum = 9600;
			break;
		case	0x01:
			baudnum = 19200;
			break;
		case	0x02:
			baudnum = 38400;
			break;
		case	0x03:
			baudnum = 57600;
			break;
		case	0x04:
			baudnum = 115200;
			break;
		case	0x05:
			baudnum = 4800;
			break;
		default:
			baudnum = 9600;
			break;		  	
	}
	
	USART_Cmd(USART1, DISABLE);     
	USART_InitStructure.USART_BaudRate = baudnum;
	
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	USART_Init(USART1, &USART_InitStructure);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE); 
}
void MT_BeepOnce(void)
{
	BeepOnTimes = 1;
	BeepOffTime = 1;
	BeepOnTime = 1;

	Tim3_Ch(BeepOnTime);
	//TIM_Cmd(TIM3, ENABLE);
	BeepOn();
	BeepFlag = 1;	   //0表示响，1表示关

}

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/

