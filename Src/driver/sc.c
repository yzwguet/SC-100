/**
  ******************************************************************************
  * @file    sc.c
  * @author  zhenhua mo
  * @version V1.0
  * @date    2010/01/04
  * @brief   This file is describing the SmartCard funtions.                            
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "sc.h"
#include "hw_sc.h"
#include "type.h"
#include "hw_config.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/************T1************************************/
SC_ATR SC_A2R;
CARD_Parameter Parameter[4];
SC_State SCState = SC_POWER_OFF;

u8 Cold_Hot_Reset = 0;//0为冷复位，1为热复位

vs8 Timer4_over;

u32 delay_time,delay_time_t1byte,delay_time_t1wait;
u16 delay_u8;
u8  pcb_tmp;

const u32 F_Table[16] = {0, 372, 558, 744, 1116, 1488, 2232, 0,
                         0, 512, 768, 1024, 1536, 2048, 0, 0};
const u32 D_Table[10] = {0, 1, 2, 4, 8, 16, 32, 0,12,20};
const u8  I_Table[4]  = {25, 50, 0, 0};

//------暂存
u16 SCData = 0;


#define use_clock_type  72
#define hse_72mhz       72


__IO uint32_t CardInserted = 0;
/* Private function prototypes -----------------------------------------------*/
u16 Card_Init(void);
u16 Card_PowerOff(void);
u16 Card_PowerOn(u8 RstBaud,u16 *Length,u8 * Atr);
u16 Card_Reset(u16 *Length,u8 * Atr);
//u16 Card_Apdu(u16 *Length,u8 * Command,u8 * Response);
u16 Card_Protocol(u16 Protocol);

u16 Card_Apdu_T0(u16 Length,u8 * Command,u8 * Response);
u16 Card_Apdu_T1(u16 Length,u8 * Command,u8 * Response);

void SC_PowerCmd(FunctionalState NewState);
void SC_Reset_Card(BitAction ResetState);
void SC_VoltageConfig(void);
void Parameter_Init(u8 SC_Voltagetmp);

void SC_UART_Init(u8 slot);
void SC_Init(void);
void SC_AnswerReq(SC_State *SCstate);
u16 SC_decode_Answer2reset(void);
void SC_Handler(SC_State *SCState);
u16 SC_Reset_Cold(u8 RstBaud,u8 SC_Voltagetmp,u16 *Length,u8 * Atr);

void SC_USART_SendData(uint16_t Data);
//u8 USART2_ByteReceive(u8 *Data, u32 TimeOut);
//u8 USART3_ByteReceive(u8 *Data, u32 TimeOut);
void SC3_USART_IRQHandler(void);
void SC_ParityErrorHandler(void);
u16 SC1_FindReplyReset(void);
u16 SC2_FindReplyReset(void);
u16 SC3_FindReplyReset(void);

void delay_Init(void);
void TIM4_Close(void);
void TIM4_IRQHandler(void);
void TIM4_Init(u16 Prescale);
void TIM4_Start(u16 delay);
u16 TIM4_Stop(void);

extern	u8	samflagts;
extern	u8	samslot;

void ddd(unsigned	char	ss);
/* Private functions ---------------------------------------------------------*/
/*
 *******************************************************************************
 * @brief  Card_Init
 * @param  none
 * @retval none  
 *******************************************************************************
 */
u16 Card_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
//  EXTI_InitTypeDef EXTI_InitStructure;
//  NVIC_InitTypeDef NVIC_InitStructure;

  /* CLOCK Configuration ******************************************************/
  /* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE); 
 
  /* GPIO Configuration *******************************************************/
  /* Configure Smartcard Reset  */
  GPIO_InitStructure.GPIO_Pin = SC3_RESET;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIO3_RESET, &GPIO_InitStructure);

  /* Set RSTIN HIGH */  
  GPIO_SetBits(GPIO3_RESET, SC3_RESET);

  /* Configure Smartcard 3/5V  */
#if	0	
	//2011-7-26 by lsq
  GPIO_InitStructure.GPIO_Pin = SC1_3_5V;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIO_3_5V, &GPIO_InitStructure);

  /* Configure Smartcard 1.8V  */
  //GPIO_InitStructure.GPIO_Pin = SC1_18V;
  //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  //GPIO_Init(GPIO_18V, &GPIO_InitStructure);
#endif
#if	0
	//2011-7-26 by lsq
  /* Configure Smartcard CMDVCC  */
  GPIO_InitStructure.GPIO_Pin = SC1_CMDVCC;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIO_CMDVCC, &GPIO_InitStructure);
#endif
  /* Select Smartcard CMDVCC */ 
  //GPIO_SetBits(GPIO_CMDVCC, SC1_CMDVCC);

  /* Select Smartcard OFF */
#if	0
	//2011-7-26 by lsq
  GPIO_InitStructure.GPIO_Pin = SC1_OFF;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIO_OFF, &GPIO_InitStructure);
#endif  
  return Card_OK;	  
}
/*
 *******************************************************************************
 * @brief  Card_PowerOff
 * @param  none
 * @retval none  
 *******************************************************************************
 */
u16 Card_PowerOff(void)
{	
    SC_Reset_Card(Bit_RESET); 
	/* Deinitializes the SC3_USART */
	if(samslot == 1)
	{
    	USART_DeInit(SC1_USART);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);
	}
	if(samslot == 2)
	{
    	USART_DeInit(SC2_USART);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE);
	}
	if(samslot == 3)
	{
    	USART_DeInit(SC3_USART);
		/* Disable SC3_USART clock */
    	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, DISABLE);
    }
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE); //更改第五点
    SC_PowerCmd(DISABLE);
	
	//SC_PowerCmd(ENABLE);
  	//GPIO_SetBits(GPIOB, GPIO_Pin_1);


    RCC->APB1ENR &=~ RCC_APB1ENR_TIM4EN;
    //RCC_Init(__RCC_CFGR_VAL_1MHZ);
    return Card_OK;
}
/*
 *******************************************************************************
 * @brief  Card_PowerOn
 * @param  none
 * @retval none  
 *******************************************************************************
 */
//u16 Card_PowerOn(u16 *Length,u8 * Atr)
u16 Card_PowerOn(u8 RstBaud,u16 *Length,u8 * Atr)
{
	u16 i;

    (void)Card_PowerOff();
	//i=SC_Reset_Cold(SC_Voltage_3V,Length,Atr);
	i=SC_Reset_Cold(RstBaud,SC_Voltage_3V,Length,Atr);
	if(i==Card_OK)
	{

	    return i;
	}
	(void)Card_PowerOff();
	//i=SC_Reset_Cold(SC_Voltage_3V,Length,Atr);
	i=SC_Reset_Cold(RstBaud,SC_Voltage_5V,Length,Atr);

	return i;
}
/*
 *******************************************************************************
 * @brief  SC_PowerCmd
 * @param  none
 * @retval none  
 *******************************************************************************
 */
void SC_PowerCmd(FunctionalState NewState)
{
#if	1
	//2011-7-26 by lsq    
	if(NewState != DISABLE)
    {
        GPIO_ResetBits(GPIO3_SC_CMDVCC, SC_CMDVCC);
    
    }
    else
    {
        GPIO_SetBits(GPIO3_SC_CMDVCC, SC_CMDVCC); 
    } 
#endif
}
/*
 *******************************************************************************
 * @brief  SC_Reset_Card
 * @param  none
 * @retval none  
 *******************************************************************************
 */
void SC_Reset_Card(BitAction ResetState)
{
    if(samslot == 1)
		GPIO_WriteBit(GPIO1_RESET, SC1_RESET, ResetState);
	if(samslot == 2)
		GPIO_WriteBit(GPIO2_RESET, SC2_RESET, ResetState);
	if(samslot == 3)
		GPIO_WriteBit(GPIO3_RESET, SC3_RESET, ResetState);
}
/*
 *******************************************************************************
 * @brief  SC_VoltageConfig
 * @param  none
 * @retval none  
 *******************************************************************************
 */
void SC_VoltageConfig(void)
{
#if	1


    if(Parameter[samslot].CARD_Voltage == SC_Voltage_5V)
    {
        GPIO_SetBits(GPIO3_3_5V,SC_3_5V);
    }
    else
	if(Parameter[samslot].CARD_Voltage == SC_Voltage_3V)
    {
 
        GPIO_ResetBits(GPIO3_3_5V,SC_3_5V);
    } 

#endif
}
/*
 *******************************************************************************
 * @brief  delay_Init
 * @param  none
 * @retval none  
 *******************************************************************************
 */
void delay_Init(void)
{
	delay_time = ((u32)Parameter[samslot].CARD_W*960);		//第一处参数
	
	if(Parameter[samslot].CARD_N==255)						//第二处参数
	{
		if(Parameter[samslot].CARD_PROTOCOL==T1_PROTOCOL) 	//第三处参数
		    delay_u8=11;
		else 	
		    delay_u8=12;
	}
	else 
	{
	 	delay_u8=(u16)(12+Parameter[samslot].CARD_N);				//第四处参数
	}
	//T1
	delay_time_t1byte=((u32)(1<<Parameter[samslot].CARD_CWI)+11);	  //第五处参数
	delay_time_t1wait=(((u32)1<<Parameter[samslot].CARD_BWI)*960);	 //第六处参数
}
/*
 *******************************************************************************
 * @brief  Parameter_Init
 * @param  none
 * @retval none  
 *******************************************************************************
 */
void Parameter_Init(u8 SC_Voltagetmp)
{
	Parameter[samslot].CARD_DIRECT = DIRECT;			   //第七处参数
	Parameter[samslot].CARD_PROTOCOL=T0_PROTOCOL;
	Parameter[samslot].CARD_Voltage=SC_Voltagetmp;
	Parameter[samslot].CARD_F=372;
	Parameter[samslot].CARD_D=1;
	Parameter[samslot].CARD_Ipp=50;
	Parameter[samslot].CARD_PI=5;
	Parameter[samslot].CARD_N=0;
	Parameter[samslot].CARD_W=10;
	Parameter[samslot].CARD_TRANSFER_STATION=CARD_MODE_Consult;
	Parameter[samslot].CARD_TYPE_STATION=Card_OK;
	Parameter[samslot].CARD_CWI=13;
	Parameter[samslot].CARD_BWI=4;
	Parameter[samslot].CARD_IFSC=32;
	delay_Init();
}
/*
 *******************************************************************************
 * @brief  SC_UART_Init
 * @param  none
 * @retval none  
 *******************************************************************************
 */

#if 1
void SC_UART_Init(u8 slot)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	USART_InitTypeDef USART_InitStructure;
    USART_ClockInitTypeDef USART_ClockInitStructure;

  if(slot == 1)
  {

	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);   //更改第六处
	 
	  /* GPIO Configuration *******************************************************/
	  /* Configure Smartcard Reset  */
	  GPIO_InitStructure.GPIO_Pin = SC1_RESET;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	  GPIO_Init(GPIO1_RESET, &GPIO_InitStructure);
	
	  /* Set RSTIN HIGH */  	 
	  GPIO_SetBits(GPIO1_RESET, SC1_RESET);   
	  //return Card_OK;							  //PB13到位  需要测试
	  	    
		/* Enable SC3_USART clock */
	    if(Cold_Hot_Reset==0)
	        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
			//RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	  //更改第二点
	    /* Configure SC3_USART CK(PB.12) as alternate function push-pull */
	    GPIO_InitStructure.GPIO_Pin = SC1_USART_ClkPin;
	    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	    GPIO_Init(GPIOA, &GPIO_InitStructure);					 //更改第三点
	  
	    /* Configure SC3_USART Tx (PB.10) as alternate function open-drain */
	    GPIO_InitStructure.GPIO_Pin = SC1_USART_TxPin;
	    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	    GPIO_Init(GPIOA, &GPIO_InitStructure);					//更改第三点
	    /* SC3_USART configuration ------------------------------------------------------*/
	    /* SC3_USART configured as follow:
	        - Word Length = 9 Bits
	        - 0.5 Stop Bit
	        - Even parity
	        - BaudRate =  baud
	        - Hardware flow control disabled (RTS and CTS signals)
	        - Tx and Rx enabled
	        - USART Clock enabled
	    */
	    /* USART Clock set to 3.6 MHz (PCLK1 (72 MHZ) / 20) */
	    if(use_clock_type==hse_72mhz)
	        USART_SetPrescaler(SC1_USART, 10);  //5 7.00
	    else
	        USART_SetPrescaler(SC1_USART, 0x01); 
				
	    /* USART Guard Time set to 16 Bit */
	    USART_SetGuardTime(SC1_USART, delay_u8-11);
	  
	    USART_ClockInitStructure.USART_Clock = USART_Clock_Enable;
	    USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
	    USART_ClockInitStructure.USART_CPHA = USART_CPHA_1Edge;
	    USART_ClockInitStructure.USART_LastBit = USART_LastBit_Enable;
	    USART_ClockInit(SC1_USART, &USART_ClockInitStructure);
	
	    if(use_clock_type==hse_72mhz)
			USART_InitStructure.USART_BaudRate = (u32)SAM_USB_UART_CLOCK*(u32)Parameter[samslot].CARD_D/(u32)Parameter[samslot].CARD_F;	//第十二处参数
	    else
	  	    USART_InitStructure.USART_BaudRate = (u32)SAM_UART_CLOCK*(u32)Parameter[samslot].CARD_D/(u32)Parameter[samslot].CARD_F;		//第十三处参数
	
	    USART_InitStructure.USART_WordLength = USART_WordLength_9b;
	    USART_InitStructure.USART_StopBits = USART_StopBits_1_5;
	    USART_InitStructure.USART_Parity = USART_Parity_Even;
	    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	    USART_Init(SC1_USART, &USART_InitStructure);  
	
	    if(Parameter[samslot].CARD_PROTOCOL==T0_PROTOCOL)	  //第十四处参数
	    {
	        /* Enable the SC3_USART Parity Error Interrupt */
	        USART_ITConfig(SC1_USART, USART_IT_PE, ENABLE);
	
	        /* Enable the SC3_USART Framing Error Interrupt */
	        USART_ITConfig(SC1_USART, USART_IT_ERR, ENABLE);
	    }
	    else 
		if(Parameter[samslot].CARD_PROTOCOL==T1_PROTOCOL)		 //第十五处参数
	    {
	        /* Enable the SC3_USART Parity Error Interrupt */
	        USART_ITConfig(SC1_USART, USART_IT_PE, DISABLE);
	
	        /* Enable the SC3_USART Framing Error Interrupt */
	        USART_ITConfig(SC1_USART, USART_IT_ERR, DISABLE);
	    }
	    else ;
	    
		/* Enable SC3_USART */
	    USART_Cmd(SC1_USART, ENABLE);
	
	    /* Enable the NACK Transmission */
	    USART_SmartCardNACKCmd(SC1_USART, ENABLE);
	
	    /* Enable the Smartcard Interface */
	    USART_SmartCardCmd(SC1_USART, ENABLE);
  }
  if(slot == 2)
  {
  		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   //更改第六处
	 
	  /* GPIO Configuration *******************************************************/
	  /* Configure Smartcard Reset  */
	  GPIO_InitStructure.GPIO_Pin = SC2_RESET;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	  GPIO_Init(GPIO2_RESET, &GPIO_InitStructure);
	
	  /* Set RSTIN HIGH */  	 
	  GPIO_SetBits(GPIO2_RESET, SC2_RESET); 
	   
	  //return Card_OK;							  //PB13到位  需要测试
	  	    
		/* Enable SC3_USART clock */
	    if(Cold_Hot_Reset==0)
	        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
			//RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	  //更改第二点
	                           
	    /* Configure SC3_USART CK(PB.12) as alternate function push-pull */
	    GPIO_InitStructure.GPIO_Pin = SC2_USART_ClkPin;
	    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	    GPIO_Init(GPIOA, &GPIO_InitStructure);					 //更改第三点
	  
	    /* Configure SC3_USART Tx (PB.10) as alternate function open-drain */
	    GPIO_InitStructure.GPIO_Pin = SC2_USART_TxPin;
	    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	    GPIO_Init(GPIOA, &GPIO_InitStructure);					//更改第三点
	    /* SC3_USART configuration ------------------------------------------------------*/
	    /* SC3_USART configured as follow:
	        - Word Length = 9 Bits
	        - 0.5 Stop Bit
	        - Even parity
	        - BaudRate =  baud
	        - Hardware flow control disabled (RTS and CTS signals)
	        - Tx and Rx enabled
	        - USART Clock enabled
	    */
	    /* USART Clock set to 3.6 MHz (PCLK1 (72 MHZ) / 20) */
	    if(use_clock_type==hse_72mhz)
	        USART_SetPrescaler(SC2_USART, 5); 
	    else
	        USART_SetPrescaler(SC2_USART, 0x01); 
				
	    /* USART Guard Time set to 16 Bit */
	    USART_SetGuardTime(SC2_USART, delay_u8-11);
	  
	    USART_ClockInitStructure.USART_Clock = USART_Clock_Enable;
	    USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
	    USART_ClockInitStructure.USART_CPHA = USART_CPHA_1Edge;
	    USART_ClockInitStructure.USART_LastBit = USART_LastBit_Enable;
	    USART_ClockInit(SC2_USART, &USART_ClockInitStructure);
	
	    if(use_clock_type==hse_72mhz)
			USART_InitStructure.USART_BaudRate = (u32)SAM_USB_UART_CLOCK*(u32)Parameter[samslot].CARD_D/(u32)Parameter[samslot].CARD_F;	//第十二处参数
	    else
	  	    USART_InitStructure.USART_BaudRate = (u32)SAM_UART_CLOCK*(u32)Parameter[samslot].CARD_D/(u32)Parameter[samslot].CARD_F;		//第十三处参数
	
	    USART_InitStructure.USART_WordLength = USART_WordLength_9b;
	    USART_InitStructure.USART_StopBits = USART_StopBits_1_5;
	    USART_InitStructure.USART_Parity = USART_Parity_Even;
	    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	    USART_Init(SC2_USART, &USART_InitStructure);  
	
	    if(Parameter[samslot].CARD_PROTOCOL==T0_PROTOCOL)	  //第十四处参数
	    {
	        /* Enable the SC3_USART Parity Error Interrupt */
	        USART_ITConfig(SC2_USART, USART_IT_PE, ENABLE);
	
	        /* Enable the SC3_USART Framing Error Interrupt */
	        USART_ITConfig(SC2_USART, USART_IT_ERR, ENABLE);
	    }
	    else 
		if(Parameter[samslot].CARD_PROTOCOL==T1_PROTOCOL)		 //第十五处参数
	    {
	        /* Enable the SC3_USART Parity Error Interrupt */
	        USART_ITConfig(SC2_USART, USART_IT_PE, DISABLE);
	
	        /* Enable the SC3_USART Framing Error Interrupt */
	        USART_ITConfig(SC2_USART, USART_IT_ERR, DISABLE);
	    }
	    else ;
	    
		/* Enable SC3_USART */
	    USART_Cmd(SC2_USART, ENABLE);
	
	    /* Enable the NACK Transmission */
	    USART_SmartCardNACKCmd(SC2_USART, ENABLE);
	
	    /* Enable the Smartcard Interface */
	    USART_SmartCardCmd(SC2_USART, ENABLE);
  }
  if(slot == 3)
  {
	  /* CLOCK Configuration ******************************************************/
	  /* Enable GPIO clock */
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   //更改第六处
	 
	  /* GPIO Configuration *******************************************************/
	  /* Configure Smartcard Reset  */
	  GPIO_InitStructure.GPIO_Pin = SC3_RESET;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	  GPIO_Init(GPIO3_RESET, &GPIO_InitStructure);
	
	  /* Set RSTIN HIGH */  	 
	  GPIO_SetBits(GPIO3_RESET, SC3_RESET);
	   while(0)
	  {
	  	GPIO_SetBits(GPIO3_RESET, SC3_RESET);
		Delay_Ms(100);
		GPIO_ResetBits(GPIO3_RESET, SC3_RESET);
		Delay_Ms(100);
	  }   
#if 0			  //卡到位信号
	  //return Card_OK;							  //PB13到位  需要测试
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//GPIO_Mode_Out_PP;
	  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	  /* Set RSTIN HIGH */  
	  GPIO_SetBits(GPIOB, GPIO_Pin_13); 
#endif		
	    
		/* Enable SC3_USART clock */
	    if(Cold_Hot_Reset==0)
	        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
			//RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	  //更改第二点
	                           
	    /* Configure SC3_USART CK(PB.12) as alternate function push-pull */
	    GPIO_InitStructure.GPIO_Pin = SC3_USART_ClkPin;
	    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	    GPIO_Init(GPIOB, &GPIO_InitStructure);					 //更改第三点
	  
	    /* Configure SC3_USART Tx (PB.10) as alternate function open-drain */
	    GPIO_InitStructure.GPIO_Pin = SC3_USART_TxPin;
	    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	    GPIO_Init(GPIOB, &GPIO_InitStructure);					//更改第三点
	    /* SC3_USART configuration ------------------------------------------------------*/
	    /* SC3_USART configured as follow:
	        - Word Length = 9 Bits
	        - 0.5 Stop Bit
	        - Even parity
	        - BaudRate =  baud
	        - Hardware flow control disabled (RTS and CTS signals)
	        - Tx and Rx enabled
	        - USART Clock enabled
	    */
	    /* USART Clock set to 3.6 MHz (PCLK1 (72 MHZ) / 20) */
	    if(use_clock_type==hse_72mhz)
	        USART_SetPrescaler(SC3_USART, 5); 
	    else
	        USART_SetPrescaler(SC3_USART, 0x01); 
				
	    /* USART Guard Time set to 16 Bit */
	    USART_SetGuardTime(SC3_USART, delay_u8-11);
	  
	    USART_ClockInitStructure.USART_Clock = USART_Clock_Enable;
	    USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
	    USART_ClockInitStructure.USART_CPHA = USART_CPHA_1Edge;
	    USART_ClockInitStructure.USART_LastBit = USART_LastBit_Enable;
	    USART_ClockInit(SC3_USART, &USART_ClockInitStructure);
	
	    if(use_clock_type==hse_72mhz)
			USART_InitStructure.USART_BaudRate = (u32)SAM_USB_UART_CLOCK*(u32)Parameter[samslot].CARD_D/(u32)Parameter[samslot].CARD_F;	//第十二处参数
	    else
	  	    USART_InitStructure.USART_BaudRate = (u32)SAM_UART_CLOCK*(u32)Parameter[samslot].CARD_D/(u32)Parameter[samslot].CARD_F;		//第十三处参数
	
	    USART_InitStructure.USART_WordLength = USART_WordLength_9b;
	    USART_InitStructure.USART_StopBits = USART_StopBits_1_5;
	    USART_InitStructure.USART_Parity = USART_Parity_Even;
	    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	    USART_Init(SC3_USART, &USART_InitStructure);  
	
	    if(Parameter[samslot].CARD_PROTOCOL==T0_PROTOCOL)	  //第十四处参数
	    {
	        /* Enable the SC3_USART Parity Error Interrupt */
	        USART_ITConfig(SC3_USART, USART_IT_PE, ENABLE);
	
	        /* Enable the SC3_USART Framing Error Interrupt */
	        USART_ITConfig(SC3_USART, USART_IT_ERR, ENABLE);
	    }
	    else 
		if(Parameter[samslot].CARD_PROTOCOL==T1_PROTOCOL)		 //第十五处参数
	    {
	        /* Enable the SC3_USART Parity Error Interrupt */
	        USART_ITConfig(SC3_USART, USART_IT_PE, DISABLE);
	
	        /* Enable the SC3_USART Framing Error Interrupt */
	        USART_ITConfig(SC3_USART, USART_IT_ERR, DISABLE);
	    }
	    else ;
	    
		/* Enable SC3_USART */
	    USART_Cmd(SC3_USART, ENABLE);
	
	    /* Enable the NACK Transmission */
	    USART_SmartCardNACKCmd(SC3_USART, ENABLE);
	
	    /* Enable the Smartcard Interface */
	    USART_SmartCardCmd(SC3_USART, ENABLE);
	}
}
#endif
#if 0
void SC_UART_Init(u8 slot)
{
  	  GPIO_InitTypeDef GPIO_InitStructure;

	  USART_InitTypeDef USART_InitStructure;
      USART_ClockInitTypeDef USART_ClockInitStructure;
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);    
	 
	  /* GPIO Configuration *******************************************************/
	  /* Configure Smartcard Reset  */
	  GPIO_InitStructure.GPIO_Pin = SC3_RESET;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	  GPIO_Init(GPIO3_RESET, &GPIO_InitStructure);
	
	  /* Set RSTIN HIGH */  	 
	  GPIO_SetBits(GPIO3_RESET, SC3_RESET);  
	  /* Configure Smartcard 3/5V  */
	  GPIO_InitStructure.GPIO_Pin = SC_3_5V;
	  GPIO_Init(GPIO3_3_5V, &GPIO_InitStructure);
	
	  /* Configure Smartcard CMDVCC  */
	  GPIO_InitStructure.GPIO_Pin = SC_CMDVCC;
	  GPIO_Init(GPIO3_SC_CMDVCC, &GPIO_InitStructure);	    
		/* Enable SC3_USART clock */
	    if(Cold_Hot_Reset==0)
	        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
			//RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	  //更改第二点
	                           
	    /* Configure SC3_USART CK(PB.12) as alternate function push-pull */
	    GPIO_InitStructure.GPIO_Pin = SC3_USART_ClkPin;
	    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	    GPIO_Init(GPIOB, &GPIO_InitStructure);					 //更改第三点
	  
	    /* Configure SC3_USART Tx (PB.10) as alternate function open-drain */
	    GPIO_InitStructure.GPIO_Pin = SC3_USART_TxPin;
	    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	    GPIO_Init(GPIOB, &GPIO_InitStructure);					//更改第三点
	    /* SC3_USART configuration ------------------------------------------------------*/
	    /* SC3_USART configured as follow:
	        - Word Length = 9 Bits
	        - 0.5 Stop Bit
	        - Even parity
	        - BaudRate =  baud
	        - Hardware flow control disabled (RTS and CTS signals)
	        - Tx and Rx enabled
	        - USART Clock enabled
	    */
	    /* USART Clock set to 3.6 MHz (PCLK1 (72 MHZ) / 20) */
	    if(use_clock_type==hse_72mhz)
	        USART_SetPrescaler(SC3_USART, 5); 
	    else
	        USART_SetPrescaler(SC3_USART, 0x01); 
				
	    /* USART Guard Time set to 16 Bit */
	    USART_SetGuardTime(SC3_USART, delay_u8-11);
	  
	    USART_ClockInitStructure.USART_Clock = USART_Clock_Enable;
	    USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
	    USART_ClockInitStructure.USART_CPHA = USART_CPHA_1Edge;
	    USART_ClockInitStructure.USART_LastBit = USART_LastBit_Enable;
	    USART_ClockInit(SC3_USART, &USART_ClockInitStructure);
	
	    if(use_clock_type==hse_72mhz)
			USART_InitStructure.USART_BaudRate = (u32)SAM_USB_UART_CLOCK*(u32)Parameter[samslot].CARD_D/(u32)Parameter[samslot].CARD_F;	//第十二处参数
	    else
	  	    USART_InitStructure.USART_BaudRate = (u32)SAM_UART_CLOCK*(u32)Parameter[samslot].CARD_D/(u32)Parameter[samslot].CARD_F;		//第十三处参数
	
	    USART_InitStructure.USART_WordLength = USART_WordLength_9b;
	    USART_InitStructure.USART_StopBits = USART_StopBits_1_5;
	    USART_InitStructure.USART_Parity = USART_Parity_Even;
	    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	    USART_Init(SC3_USART, &USART_InitStructure);  
	
	    if(Parameter[samslot].CARD_PROTOCOL==T0_PROTOCOL)	  //第十四处参数
	    {
	        /* Enable the SC3_USART Parity Error Interrupt */
	        USART_ITConfig(SC3_USART, USART_IT_PE, ENABLE);
	
	        /* Enable the SC3_USART Framing Error Interrupt */
	        USART_ITConfig(SC3_USART, USART_IT_ERR, ENABLE);
	    }
	    else 
		if(Parameter[samslot].CARD_PROTOCOL==T1_PROTOCOL)		 //第十五处参数
	    {
	        /* Enable the SC3_USART Parity Error Interrupt */
	        USART_ITConfig(SC3_USART, USART_IT_PE, DISABLE);
	
	        /* Enable the SC3_USART Framing Error Interrupt */
	        USART_ITConfig(SC3_USART, USART_IT_ERR, DISABLE);
	    }
	    else ;
	    
		/* Enable SC3_USART */
	    USART_Cmd(SC3_USART, ENABLE);
	
	    /* Enable the NACK Transmission */
	    USART_SmartCardNACKCmd(SC3_USART, ENABLE);
	
	    /* Enable the Smartcard Interface */
	    USART_SmartCardCmd(SC3_USART, ENABLE);
}
#endif
/*
 *******************************************************************************
 * @brief  SC_UART_Init3
 * @param  none
 * @retval none  
 *******************************************************************************
 */
void SC_UART_Init1(u8 default_setting)
{
	USART_InitTypeDef USART_InitStructure;

	USART_SetGuardTime(SC1_USART, delay_u8-11);
	
	if(default_setting==default_data)
	{
		if(use_clock_type==hse_72mhz)
		    USART_InitStructure.USART_BaudRate = (u32)SAM_USB_UART_CLOCK*1/372;
	    else
  	        USART_InitStructure.USART_BaudRate = (u32)SAM_UART_CLOCK*1/372;					    	
	}
	else
	{
		if(use_clock_type==hse_72mhz)
			USART_InitStructure.USART_BaudRate = (u32)SAM_USB_UART_CLOCK*(u32)Parameter[samslot].CARD_D/(u32)Parameter[samslot].CARD_F;	 //第十六处参数
		else
  		    USART_InitStructure.USART_BaudRate = (u32)SAM_UART_CLOCK*(u32)Parameter[samslot].CARD_D/(u32)Parameter[samslot].CARD_F;	  //第十七处参数
	}
	//USART_InitStructure.USART_BaudRate = (u32)SAM_UART_CLOCK*(u32)Parameter[samslot].CARD_D/(u32)Parameter[samslot].CARD_F;
  
    USART_InitStructure.USART_WordLength = USART_WordLength_9b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1_5;
    USART_InitStructure.USART_Parity = USART_Parity_Even;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  
    USART_Init(SC1_USART, &USART_InitStructure); 
  
  	if(Parameter[samslot].CARD_PROTOCOL == T0_PROTOCOL)	  //第十八处参数
  	{
  	    /* Enable the SC3_USART Parity Error Interrupt */
  	    USART_ITConfig(SC1_USART, USART_IT_PE, ENABLE);

  	    /* Enable the SC3_USART Framing Error Interrupt */
  	    USART_ITConfig(SC1_USART, USART_IT_ERR, ENABLE);
  	}
  	else 
	if(Parameter[samslot].CARD_PROTOCOL == T1_PROTOCOL)	   //第十九处参数
  	{
  	    /* Enable the SC3_USART Parity Error Interrupt */
  	    USART_ITConfig(SC1_USART, USART_IT_PE, DISABLE);

  	    /* Enable the SC3_USART Framing Error Interrupt */
  	    USART_ITConfig(SC1_USART, USART_IT_ERR, DISABLE);
  	}
  	else ;  
}
void SC_UART_Init2(u8 default_setting)
{
	USART_InitTypeDef USART_InitStructure;

	USART_SetGuardTime(SC2_USART, delay_u8-11);
	
	if(default_setting==default_data)
	{
		if(use_clock_type==hse_72mhz)
		    USART_InitStructure.USART_BaudRate = (u32)SAM_USB_UART_CLOCK*1/372;
	    else
  	        USART_InitStructure.USART_BaudRate = (u32)SAM_UART_CLOCK*1/372;					    	
	}
	else
	{
		if(use_clock_type==hse_72mhz)
			USART_InitStructure.USART_BaudRate = (u32)SAM_USB_UART_CLOCK*(u32)Parameter[samslot].CARD_D/(u32)Parameter[samslot].CARD_F;	 //第十六处参数
		else
  		    USART_InitStructure.USART_BaudRate = (u32)SAM_UART_CLOCK*(u32)Parameter[samslot].CARD_D/(u32)Parameter[samslot].CARD_F;	  //第十七处参数
	}
	//USART_InitStructure.USART_BaudRate = (u32)SAM_UART_CLOCK*(u32)Parameter[samslot].CARD_D/(u32)Parameter[samslot].CARD_F;
  
    USART_InitStructure.USART_WordLength = USART_WordLength_9b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1_5;
    USART_InitStructure.USART_Parity = USART_Parity_Even;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  
    USART_Init(SC2_USART, &USART_InitStructure); 
  
  	if(Parameter[samslot].CARD_PROTOCOL == T0_PROTOCOL)	  //第十八处参数
  	{
  	    /* Enable the SC3_USART Parity Error Interrupt */
  	    USART_ITConfig(SC2_USART, USART_IT_PE, ENABLE);

  	    /* Enable the SC3_USART Framing Error Interrupt */
  	    USART_ITConfig(SC2_USART, USART_IT_ERR, ENABLE);
  	}
  	else 
	if(Parameter[samslot].CARD_PROTOCOL == T1_PROTOCOL)	   //第十九处参数
  	{
  	    /* Enable the SC3_USART Parity Error Interrupt */
  	    USART_ITConfig(SC2_USART, USART_IT_PE, DISABLE);

  	    /* Enable the SC3_USART Framing Error Interrupt */
  	    USART_ITConfig(SC2_USART, USART_IT_ERR, DISABLE);
  	}
  	else ;  
}
void SC_UART_Init3(u8 default_setting)
{
	USART_InitTypeDef USART_InitStructure;

	USART_SetGuardTime(SC3_USART, delay_u8-11);
	
	if(default_setting==default_data)
	{
		if(use_clock_type==hse_72mhz)
		    USART_InitStructure.USART_BaudRate = (u32)SAM_USB_UART_CLOCK*1/372;
	    else
  	        USART_InitStructure.USART_BaudRate = (u32)SAM_UART_CLOCK*1/372;					    	
	}
	else
	{
		if(use_clock_type==hse_72mhz)
			USART_InitStructure.USART_BaudRate = (u32)SAM_USB_UART_CLOCK*(u32)Parameter[samslot].CARD_D/(u32)Parameter[samslot].CARD_F;	 //第十六处参数
		else
  		    USART_InitStructure.USART_BaudRate = (u32)SAM_UART_CLOCK*(u32)Parameter[samslot].CARD_D/(u32)Parameter[samslot].CARD_F;	  //第十七处参数
	}
	//USART_InitStructure.USART_BaudRate = (u32)SAM_UART_CLOCK*(u32)Parameter[samslot].CARD_D/(u32)Parameter[samslot].CARD_F;
  
    USART_InitStructure.USART_WordLength = USART_WordLength_9b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1_5;
    USART_InitStructure.USART_Parity = USART_Parity_Even;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  
    USART_Init(SC3_USART, &USART_InitStructure); 
  
  	if(Parameter[samslot].CARD_PROTOCOL == T0_PROTOCOL)	  //第十八处参数
  	{
  	    /* Enable the SC3_USART Parity Error Interrupt */
  	    USART_ITConfig(SC3_USART, USART_IT_PE, ENABLE);

  	    /* Enable the SC3_USART Framing Error Interrupt */
  	    USART_ITConfig(SC3_USART, USART_IT_ERR, ENABLE);
  	}
  	else 
	if(Parameter[samslot].CARD_PROTOCOL == T1_PROTOCOL)	   //第十九处参数
  	{
  	    /* Enable the SC3_USART Parity Error Interrupt */
  	    USART_ITConfig(SC3_USART, USART_IT_PE, DISABLE);

  	    /* Enable the SC3_USART Framing Error Interrupt */
  	    USART_ITConfig(SC3_USART, USART_IT_ERR, DISABLE);
  	}
  	else ;  
}
/*
 *******************************************************************************
 * @brief  SC_Init
 * @param  none
 * @retval none  
 *******************************************************************************
 */
void SC_Init(void)
{
	u8 i;		  
    /* chose Voltage*/
    SC_VoltageConfig();
  
    SC_UART_Init(samslot);	///待处理		
  
    /* enable card voltage */
    SC_PowerCmd(ENABLE);

    SC_Reset_Card(Bit_SET);
                   
    /* Reset SC_A2R Structure --------------------------------------------*/
    SC_A2R.TS = 0;
    SC_A2R.T0 = 0;
    for (i = 0; i < SETUP_LENGTH; i++)
    {
  	    SC_A2R.T[i] = 0;
    }
    for (i = 0; i < HIST_LENGTH; i++)
    {
        SC_A2R.H[i] = 0;
    }
    for(i=0; i<D_LENGTH; i++)
	{
  	    SC_A2R.D[i]=0;
    }
	SC_A2R.Tlength = 0;
    SC_A2R.Hlength = 0;
    SC_A2R.Dlength = 0;       	
    SC_A2R.TCK = 0;                        
}
/*
 *******************************************************************************
 * @brief  SC_AnswerReq
 * @param  none
 * @retval none  
 *******************************************************************************
 */
void SC_AnswerReq(SC_State *SCstate)
{ 
    u16 i;
    switch(*SCstate)
    {
        case SC_RESET_LOW:    	
    	    SC_Reset_Card(Bit_RESET);
    	    //等待400个周期

            for(i=0;i<400;i++);
            //-----
           (*SCstate) = SC_RESET_HIGH;
          break;
        case SC_RESET_HIGH:
        /* Check responce with reset high --------------------------------------*/     
            SC_Reset_Card(Bit_SET); /* Reset High */
            //等待400个周期
			if(samslot == 1)
			{
	            if(SC1_FindReplyReset()==Card_OK)
	      	        (*SCstate) = SC_ACTIVE;
	            else//其他原因造成的不成功，则全部算不支持卡型
	      	        (*SCstate) = SC_POWER_OFF;
			}
			if(samslot == 2)
			{
	            if(SC2_FindReplyReset()==Card_OK)
	      	        (*SCstate) = SC_ACTIVE;
	            else//其他原因造成的不成功，则全部算不支持卡型
	      	        (*SCstate) = SC_POWER_OFF;
			}
			if(samslot == 3)
			{
	            if(SC3_FindReplyReset()==Card_OK)
	      	        (*SCstate) = SC_ACTIVE;
	            else//其他原因造成的不成功，则全部算不支持卡型
	      	        (*SCstate) = SC_POWER_OFF;
			}             
          break;
        case SC_ACTIVE:
          break;
    
        case SC_POWER_OFF:       
          break;
        default:
            (*SCstate) = SC_POWER_OFF;
    }
}
/*
 *******************************************************************************
 * @brief  SC_decode_Answer2reset
 * @param  none
 * @retval none  
 *******************************************************************************
 */
u16 SC_decode_Answer2reset(void)
{
	u8 j,i;
	//---------F,D----
    j=SC_A2R.T0&0x10;
   
	if(j==0)//default		此处可能出问题
    {
  	    //Parameter[samslot].CARD_F=372;
	    //Parameter[samslot].CARD_D=1;
    }
    else
    {
  	    Parameter[samslot].CARD_F=F_Table[((SC_A2R.T[0]&0xf0)>>4)&0x0f];  //第二十
        Parameter[samslot].CARD_D=D_Table[SC_A2R.T[0]&0x0f];			  //第二十一
    }
 	
	Parameter[samslot].CARD_TRANSFER_STATION=CARD_MODE_Consult;			  //第二十二
	
	Parameter[samslot].CARD_PROTOCOL = SC_A2R.D[0] & (u8)0x0F;			  //第二十三
	
	if(SC_A2R.D[0]&0x10)//A2 exist
	{
		Parameter[samslot].CARD_TRANSFER_STATION=CARD_MODE_Special;		  //第二十四处参数
		//找到A2
		i=0;
		for(j=0;j<4;j++)
	    {
	  	    i=i+(((SC_A2R.T0 & (u8)0xF0) >> (4 + j)) & (u8)0x1);
	    }
	    if(SC_A2R.T[i-1]&0x10)
	    {
	  	    Parameter[samslot].CARD_F=372;		//第二十五
	  	    Parameter[samslot].CARD_D=1;		//第二十六
	    }
	    //专用模式TA2将指出要使用的协议
	    Parameter[samslot].CARD_PROTOCOL = SC_A2R.T[i-1] & (u8)0x0F;   //第二十七
	}
	//-------I,P----------
    j=SC_A2R.T0&(1<<5);
    if(j==0)//default
    {
  	    Parameter[samslot].CARD_PI=5;				   //第二十八
  	    Parameter[samslot].CARD_Ipp=50;				   //第二十九
    }
    else//存在TB1
    {  	
  	    Parameter[samslot].CARD_Ipp=I_Table[((SC_A2R.T[SC_A2R.T0&(1<<4)]&0x60)>>5)&0x03];  //第三十
  	    Parameter[samslot].CARD_PI=SC_A2R.T[SC_A2R.T0&(1<<4)]&0x1f;						  //第三十一
    }
    //add p
    if(SC_A2R.D[0]&0x20)//TB2存在
    {
  	//找到B2
		i=0;
		for(j=0;j<4;j++)
	    {
	  	    i=i+(((SC_A2R.T0 & (u8)0xF0) >> (4 + j)) & (u8)0x1);
	    }
	    i=i+((SC_A2R.D[0]>>4)&0x01);
        Parameter[samslot].CARD_PI=SC_A2R.T[i]&0xff;	//第三十二
    }
    //--------N------
    if((SC_A2R.T0&(1<<6))==0)
    {
  	    Parameter[samslot].CARD_N=0;				   //第三十三
    }
    else//C1存在
    {
  	//找到C1
  	    i=((SC_A2R.T0>>4)&0x01)+((SC_A2R.T0>>5)&0x01);
  	    Parameter[samslot].CARD_N=SC_A2R.T[i];			//第三十四
    }
	//--------W-----
	if(SC_A2R.D[0]&0x40)//TC2存在
	{
		i=0;
		for(j=0;j<4;j++)
	    {
	  	    i=i+(((SC_A2R.T0 & (u8)0xF0) >> (4 + j)) & (u8)0x1);
	    }
	    for(j=0;j<2;j++)
	    {
	  	    i=i+(((SC_A2R.D[0] & (u8)0xF0) >> (4 + j)) & (u8)0x1);
	    }
		Parameter[samslot].CARD_W=SC_A2R.T[i];		  //第三十五
	}
    return Card_OK;
}

/*
 *******************************************************************************
 * @brief  SC_Handler
 * @param  none
 * @retval none  
 *******************************************************************************
 */
void SC_Handler(SC_State *SCState)
{
    u32 i = 0;
    //SerialPutChar(0x3e);
    //SerialPutChar(*SCState);
	//SerialPutChar(0x3f);
	switch(*SCState)
    {
        case SC_POWER_ON:		 //卡片上电
            /* Smartcard intialization ---------------------------------------*/  
            SC_Init();         												  
            /* Next State ----------------------------------------------------*/
            *SCState = SC_RESET_LOW;
          
		 	 break;

        case SC_RESET_LOW:		//复位为低
      	
      	    while(((*SCState) != SC_POWER_OFF) && ((*SCState) != SC_ACTIVE))
            {
                SC_AnswerReq(SCState); /* Check for answer to reset */
            }

            if((*SCState) == SC_POWER_OFF)
        	{
        		Parameter[samslot].CARD_TYPE_STATION=Card_NotSupport;
				//Parameter[samslot].CARD_TYPE_STATION=0x11;
        	}
          break;

        case SC_ACTIVE:		  //激活
			 
      	    i=SC_decode_Answer2reset();//
      	                                         //
      	    if(i == Card_OK) 
      	    {                                        
        	    if(( Parameter[samslot].CARD_PROTOCOL == T0_PROTOCOL)||(Parameter[samslot].CARD_PROTOCOL== T1_PROTOCOL))
        	    {
          	        (*SCState) = SC_ACTIVE_ON_T0;
          	        Parameter[samslot].CARD_TYPE_STATION=Card_OK ;
        	    }
        	    else
        	    {
          	        (*SCState) = SC_POWER_OFF;
           	        Parameter[samslot].CARD_TYPE_STATION=Card_NotSupport;
					//Parameter[samslot].CARD_TYPE_STATION=0x12;
        	    }
            }
            else
            {
                (*SCState) = SC_POWER_OFF;
                Parameter[samslot].CARD_TYPE_STATION=Card_NotSupport;
				//Parameter[samslot].CARD_TYPE_STATION=0x13;
            }
		 	
			//Parameter[samslot].CARD_TYPE_STATION=Card_OK ;
          break;

        case SC_ACTIVE_ON_T0://暂时未处理，在t0协议的激活状态下
          break;

        case SC_POWER_OFF:	  //断电
            Card_PowerOff(); /* Disable Smartcard interface */
          break;

        default: (*SCState) = SC_POWER_OFF;
		  break;
    }
}
/*
 *******************************************************************************
 * @brief  SC_Reset_Common
 * @param  none
 * @retval none  
 *******************************************************************************
 */
u16 SC_Reset_Common(u8 RstBaud,u8 SC_Voltagetmp,u16 *Length,u8 * Atr)
{
	u16 i,j = 0;
	
	//RCC_Init(__RCC_CFGR_VAL_8MHZ);
	
	Parameter_Init(SC_Voltagetmp);
	//1--9600,2--19200,3--38400,4--76800,5--115200.
	//if((RstBaud >>4) == 3)
	//	Parameter[samslot].CARD_F = 93;
	switch(RstBaud >>4)
	{
		case	2:
			Parameter[samslot].CARD_F = 186;	
			break;
		case	3:
			Parameter[samslot].CARD_F = 93;	
			break;
		case	4:
			Parameter[samslot].CARD_F = 46;	
			break;
		case	5:
			Parameter[samslot].CARD_F = 31;	
			break;
		default:
			Parameter[samslot].CARD_F = 372;	
			break;
	}
				  ///20110805TS
	//SC_UART_Init3(other_data);		  ///20110805TS
      
    TIM4_Init(Parameter[samslot].CARD_F);
	/* Wait A2R --------------------------------------------------------------*/
	SCState = SC_POWER_ON;		  //0

	while((SCState != SC_ACTIVE_ON_T0)&&(SCState != SC_POWER_OFF))
	{
		SC_Handler(&SCState);	
	}
	
	if(SCState != SC_POWER_OFF)
	{
	 	*Length=2+SC_A2R.Hlength+SC_A2R.Tlength;
	 	*Atr++=SC_A2R.TS;
	 	*Atr++=SC_A2R.T0;
	 	
		for(i=0;i<SC_A2R.Tlength;i++)
	 	    *Atr++=SC_A2R.T[i];
	 	
		for(i=0;i<SC_A2R.Hlength;i++)
	 	    *Atr++=SC_A2R.H[i];
	 	
		if(Parameter[samslot].CARD_PROTOCOL !=T0_PROTOCOL)
	 	{
	 		*Atr++=SC_A2R.TCK;
	 		*Length+=1;
	 	}
	 	pcb_tmp=0;	
//		if(samslot == 1)
//			SC_UART_Init1(default_data);
//		if(samslot == 2)
//			SC_UART_Init2(default_data);
//		if(samslot == 3)
//			SC_UART_Init3(default_data);  
	}	
	//----v1.25 2009-12-18 edit2:start
	delay_time=((u32)Parameter[samslot].CARD_W*960);
	//----v1.25 2009-12-18 edit2:end

//		System_WaitTime(5);
    for(i=0; i<500; i++)
	    for(j=0; j<1000; j++);
	/*
	if((s16)Parameter[samslot].CARD_TYPE_STATION == 0)
	{
		i = Card_Protocol(Parameter[samslot].CARD_PROTOCOL);
	}   
   	*/ 
	//////////
	//Parameter[samslot].CARD_PROTOCOL=T0_PROTOCOL;	
    //delay_Init();
//    SC_UART_Init3(other_data);
      
    //TIM4_Init(93);
	///////////
    return (s16)Parameter[samslot].CARD_TYPE_STATION;	
}
/*
 *******************************************************************************
 * @brief  SC_Reset_Cold
 * @param  none
 * @retval none  
 *******************************************************************************
 */
//返回成功失败，不符合T0或T1卡
//u16 SC_Reset_Cold(u8 SC_Voltagetmp,u16 *Length,u8 * Atr)
u16 SC_Reset_Cold(u8 RstBaud,u8 SC_Voltagetmp,u16 *Length,u8 * Atr)
{
	Cold_Hot_Reset=0;
	//return SC_Reset_Common(SC_Voltagetmp,Length,Atr);
	return SC_Reset_Common(RstBaud,SC_Voltagetmp,Length,Atr);
}
/*
 *******************************************************************************
 * @brief  Card_Reset
 * @param  none
 * @retval none  
 *******************************************************************************
 */
 #if 0
u16 Card_Reset(u16 *Length,u8 * Atr)
{
	//u16 i;
	
	//外围是否有电
	//i=GPIO_POWER_DOWN->ODR;
	//i&=(SC_POWER_DOWN);
	//if(i!=0) 
	//    return Card_NoPower;

	Cold_Hot_Reset=1;
	return SC_Reset_Common(Parameter[samslot].CARD_Voltage,Length,Atr);	
}
#endif
/*
 *******************************************************************************
 * @brief  SC_USART_SendData
 * @param  none
 * @retval none  
 *******************************************************************************
 */
void SC_USART_SendData(u16 Data)
{
	u16 tmp2=0,i;
    /* Check the parameters */
    assert_param(IS_USART_ALL_PERIPH(USARTx));
    assert_param(IS_USART_DATA(Data)); 
      		   
    /* Transmit Data */
    if(Parameter[samslot].CARD_DIRECT!=DIRECT)
    {
  	    //取反向
  	    for(i=0;i<8;i++)
  		    tmp2|=(Data<<(15-2*i))&(1<<(15-i));	
  	    Data=tmp2;
    }
	if(samslot == 1)
	{
    	SC1_USART->DR = (Data & (u16)0x01FF);
		while(USART_GetFlagStatus(SC1_USART, USART_FLAG_TC) == RESET);
	}
	if(samslot == 2)
	{
    	SC2_USART->DR = (Data & (u16)0x01FF);
		while(USART_GetFlagStatus(SC2_USART, USART_FLAG_TC) == RESET);
	}
	if(samslot == 3)
	{
    	SC3_USART->DR = (Data & (u16)0x01FF);
		while(USART_GetFlagStatus(SC3_USART, USART_FLAG_TC) == RESET);
	}
}
/*
 *******************************************************************************
 * @brief  USART_ByteReceive
 * @param  none
 * @retval none  
 *******************************************************************************
 */
//-------------v1.25 2009-12-18 edit1:start
u8 USART1_ByteReceive(u8 *Data, u32 TimeOut)
{
	u8 i;
	u16 tmp=0,tmp2=0;
	u16 tmpu1,tmpu2;
	
	tmpu1=TimeOut/0x10000;
	tmpu2=TimeOut%0x10000;

	while(tmpu1>0)
	{
		TIM4_Start(0xffff);
		while((TIM4_Stop()!=IS_OVER)&&(USART_GetFlagStatus(SC1_USART, USART_FLAG_RXNE) == RESET)); 	
		TIM4_Close();

		if(Timer4_over==NOT_OVER)
		{
			if(Parameter[samslot].CARD_DIRECT!=DIRECT)
  		    {
  			    tmp=(u16)USART_ReceiveData(SC1_USART);
  			    for(i=0;i<8;i++)
  			        tmp2|=(tmp<<(15-2*i))&(1<<(15-i)); 		
  			    *Data=(u8)((~(tmp2>>8))&0xff); 		
  		    }
  		    else
  			    *Data = (u8)USART_ReceiveData(SC1_USART);

   		    return SUCCESS;	
		}
		tmpu1 -= 1;
	}
	if(tmpu2 == 0)
		return ERROR;
		//return 0;

	TIM4_Start((tmpu2));//2010.04.19 mzh 时间除于3，减少超时时间
	while((TIM4_Stop()!=IS_OVER)&&(USART_GetFlagStatus(SC1_USART, USART_FLAG_RXNE) == RESET));
	TIM4_Close();

	if(Timer4_over==NOT_OVER)
	{
		if(Parameter[samslot].CARD_DIRECT!=DIRECT)
  	    {
  		    tmp=(u16)USART_ReceiveData(SC1_USART);
  		    for(i=0;i<8;i++)
  		        tmp2|=(tmp<<(15-2*i))&(1<<(15-i)); 		
  		    *Data=(u8)((~(tmp2>>8))&0xff); 		
  	    }
  	    else
  		    *Data = (u8)USART_ReceiveData(SC1_USART);

   	    return SUCCESS;	
	}
	else
		return ERROR;
		//return 0;
}
u8 USART2_ByteReceive(u8 *Data, u32 TimeOut)
{
	u8 i;
	u16 tmp=0,tmp2=0;
	u16 tmpu1,tmpu2;
	
	tmpu1=TimeOut/0x10000;
	tmpu2=TimeOut%0x10000;

	while(tmpu1>0)
	{
		TIM4_Start(0xffff);
		while((TIM4_Stop()!=IS_OVER)&&(USART_GetFlagStatus(SC2_USART, USART_FLAG_RXNE) == RESET)); 	
		TIM4_Close();

		if(Timer4_over==NOT_OVER)
		{
			if(Parameter[samslot].CARD_DIRECT!=DIRECT)
  		    {
  			    tmp=(u16)USART_ReceiveData(SC2_USART);
  			    for(i=0;i<8;i++)
  			        tmp2|=(tmp<<(15-2*i))&(1<<(15-i)); 		
  			    *Data=(u8)((~(tmp2>>8))&0xff); 		
  		    }
  		    else
  			    *Data = (u8)USART_ReceiveData(SC2_USART);

   		    return SUCCESS;	
		}
		tmpu1 -= 1;
	}
	if(tmpu2 == 0)
		return ERROR;
		//return 0;

	TIM4_Start((tmpu2));//2010.04.19 mzh 时间除于3，减少超时时间
	while((TIM4_Stop()!=IS_OVER)&&(USART_GetFlagStatus(SC2_USART, USART_FLAG_RXNE) == RESET));
	TIM4_Close();

	if(Timer4_over==NOT_OVER)
	{
		if(Parameter[samslot].CARD_DIRECT!=DIRECT)
  	    {
  		    tmp=(u16)USART_ReceiveData(SC2_USART);
  		    for(i=0;i<8;i++)
  		        tmp2|=(tmp<<(15-2*i))&(1<<(15-i)); 		
  		    *Data=(u8)((~(tmp2>>8))&0xff); 		
  	    }
  	    else
  		    *Data = (u8)USART_ReceiveData(SC2_USART);

   	    return SUCCESS;	
	}
	else
		return ERROR;
		//return 0;
}  
u8 USART3_ByteReceive(u8 *Data, u32 TimeOut)
{
	u8 i;
	u16 tmp=0,tmp2=0;
	u16 tmpu1,tmpu2;
	
	tmpu1=TimeOut/0x10000;
	tmpu2=TimeOut%0x10000;

	while(tmpu1>0)
	{
		TIM4_Start(0xffff);
		while((TIM4_Stop()!=IS_OVER)&&(USART_GetFlagStatus(SC3_USART, USART_FLAG_RXNE) == RESET)); 	
		TIM4_Close();

		if(Timer4_over==NOT_OVER)
		{
			if(Parameter[samslot].CARD_DIRECT!=DIRECT)
  		    {
  			    tmp=(u16)USART_ReceiveData(SC3_USART);
  			    for(i=0;i<8;i++)
  			        tmp2|=(tmp<<(15-2*i))&(1<<(15-i)); 		
  			    *Data=(u8)((~(tmp2>>8))&0xff); 		
  		    }
  		    else
  			    *Data = (u8)USART_ReceiveData(SC3_USART);

   		    return SUCCESS;	
		}
		tmpu1 -= 1;
	}
	if(tmpu2 == 0)
		return ERROR;
		//return 0;

	TIM4_Start((tmpu2));//2010.04.19 mzh 时间除于3，减少超时时间
	while((TIM4_Stop()!=IS_OVER)&&(USART_GetFlagStatus(SC3_USART, USART_FLAG_RXNE) == RESET));
	TIM4_Close();

	if(Timer4_over==NOT_OVER)
	{
		if(Parameter[samslot].CARD_DIRECT!=DIRECT)
  	    {
  		    tmp=(u16)USART_ReceiveData(SC3_USART);
  		    for(i=0;i<8;i++)
  		        tmp2|=(tmp<<(15-2*i))&(1<<(15-i)); 		
  		    *Data=(u8)((~(tmp2>>8))&0xff); 		
  	    }
  	    else
  		    *Data = (u8)USART_ReceiveData(SC3_USART);

   	    return SUCCESS;	
	}
	else
		return ERROR;
		//return 0;
}
 
//------------v1.25 2009-12-18 edit1:end
/*
 *******************************************************************************
 * @brief  SC3_USART_IRQHandler
 * @param  none
 * @retval none  
 *******************************************************************************
 */
#if 0		   //测试点，看是否需要此中断，若要则走下面流程	   确定不需此处，查询方式
void SC3_USART_IRQHandler(void)
{
    /* If a Frame error is signaled by the card */
    if(USART_GetITStatus(SC3_USART, USART_IT_FE) != RESET)
    {
        USART_ReceiveData(SC3_USART);

        /* Resend the u8 that failed to be received (by the Smartcard) correctly */
        SC_ParityErrorHandler();
    }
  
    /* If the SC3_USART detects a parity error */
    if(USART_GetITStatus(SC3_USART, USART_IT_PE) != RESET)
    {
        /* Enable SC3_USART RXNE Interrupt (until receiving the corrupted u8) */
        USART_ITConfig(SC3_USART, USART_IT_RXNE, ENABLE);
        /* Flush the SC3_USART DR register */
        USART_ReceiveData(SC3_USART);
    }
  
    if(USART_GetITStatus(SC3_USART, USART_IT_RXNE) != RESET)
    {
        /* Disable SC3_USART RXNE Interrupt */
        USART_ITConfig(SC3_USART, USART_IT_RXNE, DISABLE);
        USART_ReceiveData(SC3_USART);
    }
  
    /* If a Overrun error is signaled by the card */
    if(USART_GetITStatus(SC3_USART, USART_IT_ORE) != RESET)
    {
        USART_ReceiveData(SC3_USART);
    }
    /* If a Noise error is signaled by the card */
    if(USART_GetITStatus(SC3_USART, USART_IT_NE) != RESET)
    {
        USART_ReceiveData(SC3_USART);
    }
}
#endif
#if 0		    
void SC1_USART_IRQHandler(void)
{
    /* If a Frame error is signaled by the card */
    if(USART_GetITStatus(SC1_USART, USART_IT_FE) != RESET)
    {
        USART_ReceiveData(SC1_USART);

        /* Resend the u8 that failed to be received (by the Smartcard) correctly */
        SC_ParityErrorHandler();
    }
  
    /* If the SC3_USART detects a parity error */
    if(USART_GetITStatus(SC1_USART, USART_IT_PE) != RESET)
    {
        /* Enable SC3_USART RXNE Interrupt (until receiving the corrupted u8) */
        USART_ITConfig(SC1_USART, USART_IT_RXNE, ENABLE);
        /* Flush the SC3_USART DR register */
        USART_ReceiveData(SC1_USART);
    }
  
    if(USART_GetITStatus(SC1_USART, USART_IT_RXNE) != RESET)
    {
        /* Disable SC3_USART RXNE Interrupt */
        USART_ITConfig(SC1_USART, USART_IT_RXNE, DISABLE);
        USART_ReceiveData(SC1_USART);
    }
  
    /* If a Overrun error is signaled by the card */
    if(USART_GetITStatus(SC1_USART, USART_IT_ORE) != RESET)
    {
        USART_ReceiveData(SC1_USART);
    }
    /* If a Noise error is signaled by the card */
    if(USART_GetITStatus(SC1_USART, USART_IT_NE) != RESET)
    {
        USART_ReceiveData(SC1_USART);
    }
}
void SC2_USART_IRQHandler(void)
{
    /* If a Frame error is signaled by the card */
    if(USART_GetITStatus(SC2_USART, USART_IT_FE) != RESET)
    {
        USART_ReceiveData(SC2_USART);

        /* Resend the u8 that failed to be received (by the Smartcard) correctly */
        SC_ParityErrorHandler();
    }
  
    /* If the SC3_USART detects a parity error */
    if(USART_GetITStatus(SC2_USART, USART_IT_PE) != RESET)
    {
        /* Enable SC3_USART RXNE Interrupt (until receiving the corrupted u8) */
        USART_ITConfig(SC2_USART, USART_IT_RXNE, ENABLE);
        /* Flush the SC3_USART DR register */
        USART_ReceiveData(SC2_USART);
    }
  
    if(USART_GetITStatus(SC2_USART, USART_IT_RXNE) != RESET)
    {
        /* Disable SC3_USART RXNE Interrupt */
        USART_ITConfig(SC2_USART, USART_IT_RXNE, DISABLE);
        USART_ReceiveData(SC2_USART);
    }
  
    /* If a Overrun error is signaled by the card */
    if(USART_GetITStatus(SC2_USART, USART_IT_ORE) != RESET)
    {
        USART_ReceiveData(SC2_USART);
    }
    /* If a Noise error is signaled by the card */
    if(USART_GetITStatus(SC2_USART, USART_IT_NE) != RESET)
    {
        USART_ReceiveData(SC2_USART);
    }
}
#endif
/*
 *******************************************************************************
 * @brief  SC_ParityErrorHandler
 * @param  none
 * @retval none  
 *******************************************************************************
 */
void SC_ParityErrorHandler(void)
{
    SC_USART_SendData(SCData);
}
/*
 *******************************************************************************
 * @brief  SC_FindReplyReset
 * @param  none
 * @retval none  
 *******************************************************************************
 */
u16 SC1_FindReplyReset(void)
{
	u8 Data = 0,i,tmp,flag=0;
	u8 *buf;
	//----
	//---Ts
	//USART1->SR &=~ USART_FLAG_RXNE;
	//TS需要屏蔽奇偶校验位

    /* Enable the SC3_USART Parity Error Interrupt */
    USART_ITConfig(SC1_USART, USART_IT_PE, DISABLE);

    /* Enable the SC3_USART Framing Error Interrupt */
    USART_ITConfig(SC1_USART, USART_IT_ERR, DISABLE);
	
	
	USART_ReceiveData(SC1_USART);  //清空接收缓冲？
 	//if(USART_ByteReceive(&Data, 160) != SUCCESS)//时间溢出
	/*	
	if(USART_ByteReceive(&Data, 640) != SUCCESS)//时间溢出	
  	{
	    return Card_OverTime;
	}
  	*/
	if(USART1_ByteReceive(&Data, 640) != SUCCESS)//时间溢出	
  	{
	    return Card_OverTime;
	}
	/* Enable the SC3_USART Parity Error Interrupt */
    USART_ITConfig(SC1_USART, USART_IT_PE, ENABLE);

    /* Enable the SC3_USART Framing Error Interrupt */
    USART_ITConfig(SC1_USART, USART_IT_ERR, ENABLE);	
  
    SC_A2R.TS=Data;
 
    if(SC_A2R.TS==0x3b)
  	    Parameter[samslot].CARD_DIRECT=DIRECT;
    else 
	if(SC_A2R.TS==0x03)
  	    Parameter[samslot].CARD_DIRECT=INDIRECT;	
    else//数据出错
  	{
	    return Card_NotSupport;
	} 	
  	
    //---T0
    if(USART1_ByteReceive(&Data, 9600) != SUCCESS)//时间溢出	
  	{
	    return Card_OverTime;
	}

    SC_A2R.T0=Data;
    SC_A2R.Hlength = SC_A2R.T0 & (u8)0x0F;
    SC_A2R.Tlength=0;
    buf=&SC_A2R.T[0];
    SC_A2R.Dlength=0; 	  	
    //---TA1、TB1、TC1、TD1、
    if ((SC_A2R.T0 & (u8)0x80) == 0x80)
    {
        flag = 1;
    }
    tmp=0;
    for (i = 0; i < 4; i++)
    {
        tmp=tmp+(((SC_A2R.T0 & (u8)0xF0) >> (4 + i)) & (u8)0x1);
    }
    SC_A2R.Tlength = SC_A2R.Tlength+tmp;
    for(i=0;i<tmp;i++)
    {
		if(USART1_ByteReceive(&Data, 9600) != SUCCESS)//时间溢出	
  		{
		    return Card_OverTime;
		}
  		*buf++=Data;	
    }    	
    //T0为无TCK
    if(SC_A2R.T0&0x80)
    {
  		Parameter[samslot].CARD_PROTOCOL = SC_A2R.T[SC_A2R.Tlength - 1] & (u8)0x0F;
  		SC_A2R.D[SC_A2R.Dlength++]=SC_A2R.T[SC_A2R.Tlength - 1];
    }
    else
  		Parameter[samslot].CARD_PROTOCOL = T0_PROTOCOL;
    //---TAi、TBi、TCi、TDi、
    while (flag)
    {
    	if ((SC_A2R.T[SC_A2R.Tlength - 1] & (u8)0x80) == 0x80)
    	{
      	    flag = 1;
    	}
    	else
    	{
      	    flag = 0;
    	}
        tmp=0;
  		for (i = 0; i < 4; i++)
    	{
    		tmp=tmp+(((SC_A2R.T[SC_A2R.Tlength - 1] & (u8)0xF0) >> (4 + i)) & (u8)0x1);
    	}
    	SC_A2R.Tlength = SC_A2R.Tlength+tmp;
    	for(i=0;i<tmp;i++)
    	{
			if(USART1_ByteReceive(&Data, 9600) != SUCCESS)//时间溢出	
	  		{
			    return Card_OverTime;
			}
  			*buf++=Data;	
    	}
    	if(flag == 1)
    		SC_A2R.D[SC_A2R.Dlength++]=SC_A2R.T[SC_A2R.Tlength - 1];
    }
    //---Hi
    for(i=0;i<SC_A2R.Hlength;i++)
    {
    	if(USART1_ByteReceive(&Data, 9600) != SUCCESS)//时间溢出	
  		{
		    return Card_OverTime;
		}
  		SC_A2R.H[i]=Data;	
    }
    //--TCK
    if(Parameter[samslot].CARD_PROTOCOL !=T0_PROTOCOL)
    {
		if(USART1_ByteReceive(&Data, 9600) != SUCCESS)//时间溢出	
  		{
		    return Card_OverTime;
		}
    	SC_A2R.TCK = Data;
    }
    return Card_OK;
}
u16 SC2_FindReplyReset(void)
{
	u8 Data = 0,i,tmp,flag=0;
	u8 *buf;
	//----
	//---Ts
	//USART1->SR &=~ USART_FLAG_RXNE;
	//TS需要屏蔽奇偶校验位

    /* Enable the SC3_USART Parity Error Interrupt */
    USART_ITConfig(SC2_USART, USART_IT_PE, DISABLE);

    /* Enable the SC3_USART Framing Error Interrupt */
    USART_ITConfig(SC2_USART, USART_IT_ERR, DISABLE);
	
	
	USART_ReceiveData(SC2_USART);  //清空接收缓冲？
 	//if(USART_ByteReceive(&Data, 160) != SUCCESS)//时间溢出
	/*	
	if(USART_ByteReceive(&Data, 640) != SUCCESS)//时间溢出	
  	{
	    return Card_OverTime;
	}
  	*/
	if(USART2_ByteReceive(&Data, 640) != SUCCESS)//时间溢出	
  	{
	    return Card_OverTime;
	}
	/* Enable the SC3_USART Parity Error Interrupt */
    USART_ITConfig(SC2_USART, USART_IT_PE, ENABLE);

    /* Enable the SC3_USART Framing Error Interrupt */
    USART_ITConfig(SC2_USART, USART_IT_ERR, ENABLE);	
  
    SC_A2R.TS=Data;
 
    if(SC_A2R.TS==0x3b)
  	    Parameter[samslot].CARD_DIRECT=DIRECT;
    else 
	if(SC_A2R.TS==0x03)
  	    Parameter[samslot].CARD_DIRECT=INDIRECT;	
    else//数据出错
  	{
	    return Card_NotSupport;
	} 	
  	
    //---T0
    if(USART2_ByteReceive(&Data, 9600) != SUCCESS)//时间溢出	
  	{
	    return Card_OverTime;
	}
    SC_A2R.T0=Data;
    SC_A2R.Hlength = SC_A2R.T0 & (u8)0x0F;
    SC_A2R.Tlength=0;
    buf=&SC_A2R.T[0];
    SC_A2R.Dlength=0; 	  	
    //---TA1、TB1、TC1、TD1、
    if ((SC_A2R.T0 & (u8)0x80) == 0x80)
    {
        flag = 1;
    }
    tmp=0;
    for (i = 0; i < 4; i++)
    {
        tmp=tmp+(((SC_A2R.T0 & (u8)0xF0) >> (4 + i)) & (u8)0x1);
    }
    SC_A2R.Tlength = SC_A2R.Tlength+tmp;
    for(i=0;i<tmp;i++)
    {
    	if(USART2_ByteReceive(&Data, 9600) != SUCCESS)//时间溢出	
  		{
		    return Card_OverTime;
		}
  		*buf++=Data;	
    }    	
    //T0为无TCK
    if(SC_A2R.T0&0x80)
    {
  		Parameter[samslot].CARD_PROTOCOL = SC_A2R.T[SC_A2R.Tlength - 1] & (u8)0x0F;
  		SC_A2R.D[SC_A2R.Dlength++]=SC_A2R.T[SC_A2R.Tlength - 1];
    }
    else
  		Parameter[samslot].CARD_PROTOCOL = T0_PROTOCOL;
    //---TAi、TBi、TCi、TDi、
    while (flag)
    {
    	if ((SC_A2R.T[SC_A2R.Tlength - 1] & (u8)0x80) == 0x80)
    	{
      	    flag = 1;
    	}
    	else
    	{
      	    flag = 0;
    	}
        tmp=0;
  		for (i = 0; i < 4; i++)
    	{
    		tmp=tmp+(((SC_A2R.T[SC_A2R.Tlength - 1] & (u8)0xF0) >> (4 + i)) & (u8)0x1);
    	}
    	SC_A2R.Tlength = SC_A2R.Tlength+tmp;
    	for(i=0;i<tmp;i++)
    	{
			if(USART2_ByteReceive(&Data, 9600) != SUCCESS)//时间溢出	
	  		{
			    return Card_OverTime;
			}
  			*buf++=Data;	
    	}
    	if(flag == 1)
    		SC_A2R.D[SC_A2R.Dlength++]=SC_A2R.T[SC_A2R.Tlength - 1];
    }
    //---Hi
    for(i=0;i<SC_A2R.Hlength;i++)
    {
		if(USART2_ByteReceive(&Data, 9600) != SUCCESS)//时间溢出	
  		{
		    return Card_OverTime;
		}
		SC_A2R.H[i]=Data;	
    }
    //--TCK
    if(Parameter[samslot].CARD_PROTOCOL !=T0_PROTOCOL)
    {
		if(USART2_ByteReceive(&Data, 9600) != SUCCESS)//时间溢出	
  		{
		    return Card_OverTime;
		}
    	SC_A2R.TCK = Data;
    }
    return Card_OK;
}
 
u16 SC3_FindReplyReset(void)
{
	u8 Data = 0,i,tmp,flag=0;
	u8 *buf;
	//----
	//---Ts
	//USART1->SR &=~ USART_FLAG_RXNE;
	//TS需要屏蔽奇偶校验位

    /* Enable the SC3_USART Parity Error Interrupt */
    USART_ITConfig(SC3_USART, USART_IT_PE, DISABLE);

    /* Enable the SC3_USART Framing Error Interrupt */
    USART_ITConfig(SC3_USART, USART_IT_ERR, DISABLE);
	
	
	USART_ReceiveData(SC3_USART);  //清空接收缓冲？
 	//if(USART_ByteReceive(&Data, 160) != SUCCESS)//时间溢出
	if(USART3_ByteReceive(&Data, 640) != SUCCESS)//时间溢出  调试值	
  	{
	    return Card_OverTime;
	}
  	
	/* Enable the SC3_USART Parity Error Interrupt */
    USART_ITConfig(SC3_USART, USART_IT_PE, ENABLE);

    /* Enable the SC3_USART Framing Error Interrupt */
    USART_ITConfig(SC3_USART, USART_IT_ERR, ENABLE);	
  
    SC_A2R.TS=Data;
 
    if(SC_A2R.TS==0x3b)
  	    Parameter[samslot].CARD_DIRECT=DIRECT;
    else 
	if(SC_A2R.TS==0x03)
  	    Parameter[samslot].CARD_DIRECT=INDIRECT;	
    else//数据出错
  	{
	    return Card_NotSupport;
	} 	
  	
    //---T0
	if(USART3_ByteReceive(&Data, 9600) != SUCCESS)//时间溢出	
	{
	    return Card_OverTime;
	}

    SC_A2R.T0=Data;
    SC_A2R.Hlength = SC_A2R.T0 & (u8)0x0F;
    SC_A2R.Tlength=0;
    buf=&SC_A2R.T[0];
    SC_A2R.Dlength=0; 	  	
    //---TA1、TB1、TC1、TD1、
    if ((SC_A2R.T0 & (u8)0x80) == 0x80)
    {
        flag = 1;
    }
    tmp=0;
    for (i = 0; i < 4; i++)
    {
        tmp=tmp+(((SC_A2R.T0 & (u8)0xF0) >> (4 + i)) & (u8)0x1);
    }
    SC_A2R.Tlength = SC_A2R.Tlength+tmp;
    for(i=0;i<tmp;i++)
    {
    	if(USART3_ByteReceive(&Data, 9600) != SUCCESS)//时间溢出	
  		{
		    return Card_OverTime;
		}
  		*buf++=Data;	
    }    	
    //T0为无TCK
    if(SC_A2R.T0&0x80)
    {
  		Parameter[samslot].CARD_PROTOCOL = SC_A2R.T[SC_A2R.Tlength - 1] & (u8)0x0F;
  		SC_A2R.D[SC_A2R.Dlength++]=SC_A2R.T[SC_A2R.Tlength - 1];
    }
    else
  		Parameter[samslot].CARD_PROTOCOL = T0_PROTOCOL;
    //---TAi、TBi、TCi、TDi、
    while (flag)
    {
    	if ((SC_A2R.T[SC_A2R.Tlength - 1] & (u8)0x80) == 0x80)
    	{
      	    flag = 1;
    	}
    	else
    	{
      	    flag = 0;
    	}
        tmp=0;
  		for (i = 0; i < 4; i++)
    	{
    		tmp=tmp+(((SC_A2R.T[SC_A2R.Tlength - 1] & (u8)0xF0) >> (4 + i)) & (u8)0x1);
    	}
    	SC_A2R.Tlength = SC_A2R.Tlength+tmp;
    	for(i=0;i<tmp;i++)
    	{
			if(USART3_ByteReceive(&Data, 9600) != SUCCESS)//时间溢出	
	  		{
			    return Card_OverTime;
			}
  			*buf++=Data;	
    	}
    	if(flag == 1)
    		SC_A2R.D[SC_A2R.Dlength++]=SC_A2R.T[SC_A2R.Tlength - 1];
    }
    //---Hi
    for(i=0;i<SC_A2R.Hlength;i++)
    {
		if(USART3_ByteReceive(&Data, 9600) != SUCCESS)//时间溢出	
  		{
		    return Card_OverTime;
		}
  		SC_A2R.H[i]=Data;	
    }
    //--TCK
    if(Parameter[samslot].CARD_PROTOCOL !=T0_PROTOCOL)
    {
		if(USART3_ByteReceive(&Data, 9600) != SUCCESS)//时间溢出	
  		{
		    return Card_OverTime;
		}
    	SC_A2R.TCK = Data;
    }
    return Card_OK;
}
 
/*
 *******************************************************************************
 * @brief  Card_Apdu
 * @param  none
 * @retval none  
 *******************************************************************************
 */
/*
u16 Card1_Apdu(u16 *Length,u8 * Command,u8 * Response)
{
	u16 tmp; //tmp2,leng,	i,	leng2;
	//u8  com_tmp[5]={0,0xc0,0,0,0};
	u8  *res;
	
	res = Response;
	delay_Init();
	if(Parameter[samslot].CARD_PROTOCOL==T0_PROTOCOL)
	{ 
	 	tmp=Card1_Apdu_T0(*Length,Command,res);
//loopreceive:
		if((tmp&0xff00)==0x6100)//还有一定数据需要再接收
	 	{ 
	 		return	tmp; 			
	 	}
		else 
		if((tmp&0xff00)==0x6c00) //LE错误，重发
		{	 
		    return	tmp;
		}
	 	else 
		    return tmp;
	}
	else 
	{
	 	tmp=Card1_Apdu_T1(*Length,Command,Response);
//loopreceive1:
	 	if((tmp&0xff00)==0x6100)
	 	{
	 		return	tmp;
	 	}
		else 
		if((tmp&0xff00)==0x6c00) //LE错误，重发
		{	 
			return	tmp;
		}
	 	else 
		    return tmp;

	 	//tmp=tmp2;	
	 	//goto loopreceive1;
	}
}
*/

u16 Card_Apdu(u16 Length,u8* uCommand,u8* Response)
{

	u16 tmp; //tmp2,leng,	i,	leng2;
	//u8  com_tmp[5]={0,0xc0,0,0,0};
	u8  *res;

	res = Response;
	delay_Init();
	if(Parameter[samslot].CARD_PROTOCOL==T0_PROTOCOL)
	{ 
	 	tmp=Card_Apdu_T0(Length,uCommand,res);
//loopreceive:
		if((tmp&0xff00)==0x6100)//还有一定数据需要再接收
	 	{ 
	 		return	tmp;			
	 	}
		else 
		if((tmp&0xff00)==0x6c00) //LE错误，重发
		{	 
		    return	tmp;
		}
	 	else 
		    return tmp;
	}
	else 
	{
	 	tmp=Card_Apdu_T1(Length,uCommand,Response);
//loopreceive1:
	 	if((tmp&0xff00)==0x6100)
	 	{
	 		return	tmp;
	 	}
		else 
		if((tmp&0xff00)==0x6c00) //LE错误，重发
		{	 
			return	tmp;
		}
	 	else 
		    return tmp;

	 	//tmp=tmp2;	
	 	//goto loopreceive1;
	}
#if 0
#endif
#if 0
	uint16 tmp,tmp2,leng,i,leng2;
	uint8 com_tmp[5]={0,0xc0,0,0,0};
	uint8 *res;

	res=Response;
	if(Parameter[Select_Card_NO].CARD_PROTOCOL==T0_PROTOCOL)
	{ 
	 	tmp=Card_Apdu_T0(Length,uCommand,res);
loopreceive:
		 if((tmp&0xff00)==0x6100)//还有一定数据需要再接收
	 	{ 
	 		leng=*Response;
			leng=(leng<<8)&0x00ff;
			leng+=	(*(Response+1));
	 		
	 		com_tmp[4]=tmp&0xff;
	 		res=Response+leng;
	 		tmp2=Card_Apdu_T0(5,com_tmp,res);//将数据放末尾
			leng2=*(Response+leng);
			leng2=(leng2<<8)&0x00ff;
			leng2+=	(*(Response+1+leng));
			res=Response+leng;
	 		for(i=0;i<leng2;i++)
	 		{
	 		  *(res+i)=*(res+i+2);	
	 		}
	 		//长度重新描述
	 		i=leng+leng2-2;
	 		*Response=(i>>8)&0xff;
	 		*(Response+1)=i&0xff;
	 	}
		else if((tmp&0xff00)==0x6c00) //LE错误，重发
		{	 Command[(Length)-1]=tmp&0xff;
			tmp2=Card_Apdu_T0(Length,Command,res);			
		}
	 	else
		{ 
		//CS_Default();
		return tmp;}
	 	tmp=tmp2;	
	 	goto loopreceive;
	}
	else 
		{
	 	tmp=Card_Apdu_T1(Length,Command,Response);
loopreceive1:
	 		if((tmp&0xff00)==0x6100)
	 	{
	 		leng=*Response;
			leng=(leng<<8)&0x00ff;
			leng+=	(*(Response+1)); 	 		
	 		com_tmp[4]=tmp&0xff;
	 		res=Response+leng;
	 		tmp2=Card_Apdu_T1(5,com_tmp,res);//将数据放末尾
	 		leng2=*(Response+leng);
			leng2=(leng2<<8)&0x00ff;
			leng2+=	(*(Response+1+leng));
			res=Response+leng;
	 		for(i=0;i<leng2;i++)
	 		{
	 		  *(res+i)=*(res+i+2);	
	 		}
	 		//长度重新描述
	 		i=leng+leng2-2;
	 		*Response=(i>>8)&0xff;
	 		*(Response+1)=i&0xff;
	 	}
		else if((tmp&0xff00)==0x6c00) //LE错误，重发
		{	 Command[(Length)-1]=tmp&0xff;
			tmp2=Card_Apdu_T1(Length,Command,Response);
		}
	 	else
		{//CS_Default(); 
		return tmp;}
	 	tmp=tmp2;	
	 	goto loopreceive1;
	}
#endif
}



u16 Card_Apdu_T0(u16 Length,u8 * Command,u8 * Response)
{ 	
	u16 length,i;
	u8 *tmp_buf,*tmp_response;
	u8 type_comm;
	u8 locData;
	u16 leng_comm_remain;//未发送完成的命令字节数
	u16 leng_respond_remain;//未收完的数据字节数
	u16 sw1_sw2;
	u8 send_ins;
	tmp_buf=Command;
	
	//找到命令格式
	if(Length==4)
		type_comm=Apdu_comm_0;
	else 	
	if(Length==5)
		type_comm=Apdu_comm_1;
	else 
	if(Length>5)
	{
		length=tmp_buf[4];
		if(Length==(length+5))	
			type_comm=Apdu_comm_2;
		else 
		if(Length==(length+6))	
			type_comm=Apdu_comm_3;
		else
			return Card_Parameter;
	}
	else
		return Card_Parameter;	
		
    //初始化
	leng_comm_remain=0;
	leng_respond_remain=0;;
	//回复也许要初始话
	sw1_sw2=0;
	//提取命令头
	tmp_buf=Command;
    // Send header -------------------------------------------------------------
    SCData = *tmp_buf++;
    SC_USART_SendData(SCData); 
  
    send_ins=*tmp_buf++;
    SCData =send_ins;
    SC_USART_SendData(SCData);
   
    SCData = *tmp_buf++;
    SC_USART_SendData( SCData);
  
    SCData = *tmp_buf++;
    SC_USART_SendData(SCData);

	switch(type_comm)
	{
		case Apdu_comm_0:
			SCData=0x00;
			SC_USART_SendData(SCData);
	      break;
		case Apdu_comm_1:
			leng_respond_remain=*tmp_buf++;
			SCData = leng_respond_remain;
  		    SC_USART_SendData(SCData);
		  break;
		case Apdu_comm_2:
			leng_comm_remain=*tmp_buf++;
			SCData = leng_comm_remain;
  		    SC_USART_SendData(SCData);
		  break;
		case Apdu_comm_3:
			leng_comm_remain=*tmp_buf++;
			leng_respond_remain=*(tmp_buf+leng_comm_remain);
			SCData = leng_comm_remain;
  		    SC_USART_SendData(SCData);
		  break;
		default:
			return 	Card_Parameter;
	}		
	length=0;  
	tmp_response=&Response[2];//从第2个字节开始，前2字节为长度
	*Response=0;
    *(Response+1)=0;
	tmp_buf=&Command[5];//命令的后续
	if(samslot == 1)
    	(void)USART_ReceiveData(SC1_USART);
	if(samslot == 2)
    	(void)USART_ReceiveData(SC2_USART);
	if(samslot == 3)
    	(void)USART_ReceiveData(SC3_USART);	
	//回复处理
command_a:
    do
    {
        locData=0;
		if(samslot == 1)
	        if(USART1_ByteReceive(&locData , delay_time)!= SUCCESS)
	        {
	      	    *Response=length>>8;
	            *(Response+1)=length&0xff;
	      	    return Card_OverTime;
	        }
		if(samslot == 2)
	        if(USART2_ByteReceive(&locData , delay_time)!= SUCCESS)
	        {
	      	    *Response=length>>8;
	            *(Response+1)=length&0xff;
	      	    return Card_OverTime;
	        }
		if(samslot == 3)
	        if(USART3_ByteReceive(&locData , delay_time)!= SUCCESS)
	        {
	      	    *Response=length>>8;
	            *(Response+1)=length&0xff;
	      	    return Card_OverTime;
	        }
    }while (locData == 0x60);	//null
  
    if(((locData & (u8)0xF0) == 0x60) || ((locData & (u8)0xF0) == 0x90))//sw1,sw2
    {
        // SW1 received 
        sw1_sw2 = locData;
        *tmp_response++=locData;
        length+=1;
        *Response=(u8)((length>>8)&0xff);	
        *(Response+1)=(u8)(length&0xff);
		if(samslot == 1)
		{
	        if((USART1_ByteReceive(&locData, delay_time)) == SUCCESS)
	        {
	            // SW2 received 
	            sw1_sw2=(sw1_sw2<<8)&0xff00;
	            sw1_sw2|=(locData&0xff);
	            *tmp_response++=locData;
	      	    length+=1;    	
	        }
	        else 
	        {
		        return Card_OverTime;
		    }
      	}
		if(samslot == 2)
		{
	        if((USART2_ByteReceive(&locData, delay_time)) == SUCCESS)
	        {
	            // SW2 received 
	            sw1_sw2=(sw1_sw2<<8)&0xff00;
	            sw1_sw2|=(locData&0xff);
	            *tmp_response++=locData;
	      	    length+=1;    	
	        }
	        else 
	        {
		        return Card_OverTime;
		    }
      	}
		if(samslot == 3)
		{
	        if((USART3_ByteReceive(&locData, delay_time)) == SUCCESS)
	        {
	            // SW2 received 
	            sw1_sw2=(sw1_sw2<<8)&0xff00;
	            sw1_sw2|=(locData&0xff);
	            *tmp_response++=locData;
	      	    length+=1;    	
	        }
	        else 
	        {
		        return Card_OverTime;
		    }
      	}
        *Response=(u8)((length>>8)&0xff);	
        *(Response+1)=(u8)(length&0xff);           
        return sw1_sw2 ;    
    }
    else 
	if(((locData^send_ins)==0)||((locData^send_ins)==0x01))//发送或接收完余下的数据
    {
        if(leng_comm_remain!=0)//继续发送完以后的
        {
            TIM4_Start(delay_u8-11);
	        while(TIM4_Stop()!=IS_OVER);
	        TIM4_Close();
            for(i=0;i<leng_comm_remain;i++)
            {
        	    SCData = *tmp_buf++;	
        	    SC_USART_SendData(SCData);
            } 
            if(samslot == 1)
		    	(void)USART_ReceiveData(SC1_USART);
			if(samslot == 2)
		    	(void)USART_ReceiveData(SC2_USART);
			if(samslot == 3)
		    	(void)USART_ReceiveData(SC3_USART);  
            leng_comm_remain=0;            
      	    goto command_a;	
        }
        if(leng_respond_remain!=0)//继续接受余下的
        {
      	    for(i=0;i<leng_respond_remain;i++)
      	    {
      		    if(samslot == 1)
				{
					if((USART1_ByteReceive(&locData, delay_time)) == SUCCESS)
	    			{
	      			    *tmp_response++=locData;	  
	      			    length+=1;
	      			    *Response=(u8)((length>>8)&0xff);	
	      			    *(Response+1)=(u8)(length&0xff);
	    			}
	    			else 
					{
						return Card_OverTime ;
					}
				}
				if(samslot == 2)
				{
					if((USART2_ByteReceive(&locData, delay_time)) == SUCCESS)
	    			{
	      			    *tmp_response++=locData;	  
	      			    length+=1;
	      			    *Response=(u8)((length>>8)&0xff);	
	      			    *(Response+1)=(u8)(length&0xff);
	    			}
	    			else 
					{
						return Card_OverTime ;
					}
				}
				if(samslot == 3)
				{
					if((USART3_ByteReceive(&locData, delay_time)) == SUCCESS)
	    			{
	      			    *tmp_response++=locData;	  
	      			    length+=1;
	      			    *Response=(u8)((length>>8)&0xff);	
	      			    *(Response+1)=(u8)(length&0xff);
	    			}
	    			else 
					{
						return Card_OverTime ;
					}
				}	
      	    }
      	    leng_respond_remain=0;
        }	 
        goto command_a;    
    }
    else 
	if(((locData^send_ins)==0xff)||((locData^send_ins)==0xfe))//发送或接收余下的一字节数据
    {
        if(leng_comm_remain!=0)//继续发送一个字节	
        {
            TIM4_Start(delay_u8-11);
	        while(TIM4_Stop()!=IS_OVER);
	        TIM4_Close();
      	    SCData = *tmp_buf++;	
      	    leng_comm_remain-=1;
            SC_USART_SendData(SCData);
  		    goto command_a;
        }	
        if(leng_respond_remain!=0)//继续接受余下的一个字节
        {
      	    if(samslot == 1)
			{
				if((USART1_ByteReceive(&locData, delay_time)) == SUCCESS)
	    		{
	      		    *tmp_response++=locData;
	      		    length+=1;
	      		    *Response=(u8)((length>>8)&0xff);	
	      		    *(Response+1)=(u8)(length&0xff);
	    		}
	    		else
	    		{
				    return Card_OverTime ;
				}
			}
			if(samslot == 2)
			{
				if((USART2_ByteReceive(&locData, delay_time)) == SUCCESS)
	    		{
	      		    *tmp_response++=locData;
	      		    length+=1;
	      		    *Response=(u8)((length>>8)&0xff);	
	      		    *(Response+1)=(u8)(length&0xff);
	    		}
	    		else
	    		{
				    return Card_OverTime ;
				}
			}
			if(samslot == 3)
			{
				if((USART3_ByteReceive(&locData, delay_time)) == SUCCESS)
	    		{
	      		    *tmp_response++=locData;
	      		    length+=1;
	      		    *Response=(u8)((length>>8)&0xff);	
	      		    *(Response+1)=(u8)(length&0xff);
	    		}
	    		else
	    		{
				    return Card_OverTime ;
				}
			}
    		leng_respond_remain-=1;  			
        }	
        goto command_a;	     	
    }
    else
  	    goto command_a;	
		
}
u16 Card_Apdu_T1(u16 Length,u8 * Command,u8 * Response)
{
	u8 *tmp_buf,*tmp_buf_resp;
	u16 tmp,leng,len,sw1_sw2,i;
	u8 CRC_t1;
	u8 locData;
	tmp_buf=Command;

	//查看命令是否错误
	if(Length>5)
	{
		if((Length!=(tmp_buf[4]+5))&&(Length!=(tmp_buf[4]+6)))
			return Card_Parameter;	
	}
	else 
	if(Length<4)
		return Card_Parameter;
	else ;
	//---发送NAN PCB LEN命令	
	CRC_t1=0;
	SCData = T1_NAN;
	CRC_t1^=SCData;
    SC_USART_SendData(SCData);    
	
	SCData = pcb_tmp;
	pcb_tmp=pcb_tmp?0:0x40;
	CRC_t1^=SCData;
    SC_USART_SendData(SCData);    
    
	SCData = Length;
	CRC_t1^=SCData;
    SC_USART_SendData(SCData);    
    //---发送命令包
    for(tmp=0;tmp<Length;tmp++)
    {
  	    SCData = *tmp_buf++;
		CRC_t1^=SCData;
  	    SC_USART_SendData(SCData);  	    
    }
    //--发送CRC
    SCData = CRC_t1;
    SC_USART_SendData(SCData);
	if(samslot == 1)
    	(void)USART_ReceiveData(SC1_USART);
	if(samslot == 2)
    	(void)USART_ReceiveData(SC2_USART);
	if(samslot == 3)
    	(void)USART_ReceiveData(SC3_USART);
    //(void)USART_ReceiveData(SC3_USART);
    //--接收数据
  
    len=0;
    leng=0;
	CRC_t1=0;
	tmp_buf_resp=&Response[2];
	tmp_buf=&Response[0];
	*tmp_buf=0;
	*(tmp_buf+1)=0;
	//NAD
	if(samslot == 1)
		if((USART1_ByteReceive(&locData, delay_time_t1wait)) != SUCCESS) 
		{
		    return Card_OverTime;
		}
	if(samslot == 2)
		if((USART2_ByteReceive(&locData, delay_time_t1wait)) != SUCCESS) 
		{
		    return Card_OverTime;
		}
	if(samslot == 3)
		if((USART3_ByteReceive(&locData, delay_time_t1wait)) != SUCCESS) 
		{
		    return Card_OverTime;
		}
	*tmp_buf_resp++=locData;
	CRC_t1^=locData;
	leng++;
	*tmp_buf=(leng>>8)&0xff;
	*(tmp_buf+1)=leng&0xff;
	//PCB
	if(samslot == 1)
		if((USART1_ByteReceive(&locData, delay_time_t1byte)) != SUCCESS) 
		{
		    return Card_OverTime;
		}
	if(samslot == 2)
		if((USART2_ByteReceive(&locData, delay_time_t1byte)) != SUCCESS) 
		{
		    return Card_OverTime;
		}
	if(samslot == 3)
		if((USART3_ByteReceive(&locData, delay_time_t1byte)) != SUCCESS) 
		{
		    return Card_OverTime;
		}
	*tmp_buf_resp++=locData;
	CRC_t1^=locData;
	leng++;
	*tmp_buf=(leng>>8)&0xff;
	*(tmp_buf+1)=leng&0xff;
	//LEN
	if(samslot == 1)
		if((USART1_ByteReceive(&locData, delay_time_t1byte)) != SUCCESS) 
		{
		    return Card_OverTime;
		}
	if(samslot == 2)
		if((USART2_ByteReceive(&locData, delay_time_t1byte)) != SUCCESS) 
		{
		    return Card_OverTime;
		}
	if(samslot == 3)
		if((USART3_ByteReceive(&locData, delay_time_t1byte)) != SUCCESS) 
		{
		    return Card_OverTime;
		}
	*tmp_buf_resp++=locData;
	CRC_t1^=locData;
	len=locData;
	leng++;
	*tmp_buf=(leng>>8)&0xff;
	*(tmp_buf+1)=leng&0xff;
	//--INF
	for(i=0;i<len;i++)
	{
		if(samslot == 1)
			if((USART1_ByteReceive(&locData, delay_time_t1byte)) != SUCCESS) 
			{
			    return Card_OverTime;
			}
		if(samslot == 2)
			if((USART2_ByteReceive(&locData, delay_time_t1byte)) != SUCCESS) 
			{
			    return Card_OverTime;
			}
		if(samslot == 3)
			if((USART3_ByteReceive(&locData, delay_time_t1byte)) != SUCCESS) 
			{
			    return Card_OverTime;
			}
		*tmp_buf_resp++=locData;
		CRC_t1^=locData;
		leng++;
		*tmp_buf=(leng>>8)&0xff;
		*(tmp_buf+1)=leng&0xff;	
	}
	//取SW1、SW2
	sw1_sw2=*(tmp_buf_resp-2);
	sw1_sw2=((sw1_sw2<<8)&0xff00)|((*(tmp_buf_resp-1))&0xff);
	//CRC
	if(samslot == 1)
		if((USART1_ByteReceive(&locData, delay_time_t1byte)) != SUCCESS) 
		{
		    return Card_OverTime;
		}
	if(samslot == 2)
		if((USART2_ByteReceive(&locData, delay_time_t1byte)) != SUCCESS) 
		{
		    return Card_OverTime;
		}
	if(samslot == 3)
		if((USART3_ByteReceive(&locData, delay_time_t1byte)) != SUCCESS) 
		{
		    return Card_OverTime;
		}
	*tmp_buf_resp++=locData;
	leng++;
	*tmp_buf=(leng>>8)&0xff;
	*(tmp_buf+1)=leng&0xff;
	if(CRC_t1!=locData)	
	{
	    return Card_CRCERR;
	}
	return sw1_sw2;	
}
/*
 *******************************************************************************
 * @brief  Card_Protocol
 * @param  none
 * @retval none  
 *******************************************************************************
 */
u16 Card_Protocol(u16 Protocol)
{	
	u8 F_tmp,D_tmp;
	u8 locData = 0;
	u8 CRC_tmp = 0;

	if(Parameter[samslot].CARD_TRANSFER_STATION == CARD_MODE_Special)//专用模式将不进行PPS
	{
		return Card_PPSERR;	
	}

	//参数不符合
	if((Protocol!=T0_PROTOCOL)&&(Protocol!=T1_PROTOCOL)) 
	{
	    return Card_Parameter;
	}

	switch(Parameter[samslot].CARD_F)
	{
		case 372:F_tmp=1;
		  break;
		case 558:F_tmp=2;
		  break;
		case 744:F_tmp=3;
		  break;
		case 1116:F_tmp=4;
		  break;
		case 1488:F_tmp=5;
		  break;
		case 2232:F_tmp=6;
		  break;
		case 512:F_tmp=9;
		  break;
		case 768:F_tmp=10;
		  break;
		case 1024:F_tmp=11;
		  break;
		case 1536:F_tmp=12;
		  break;
		case 2048:F_tmp=13;
		  break;
		default:F_tmp=0;
		  break;					
	}
	switch(Parameter[samslot].CARD_D)
	{
		case 1:D_tmp=1;
	  	  break;	
		case 2:D_tmp=2;
		  break;
		case 4:D_tmp=3;
		  break;
		case 8:D_tmp=4;
		  break;
		case 16:D_tmp=5;
		  break;
		case 32:D_tmp=6;
		  break;
		case 12:D_tmp=8;
		  break;
		case 20:D_tmp=9;
		  break;	
		default:D_tmp=1;
		  break;
	}
	//System_WaitTime(5);
    SCData = 0xFF;
    CRC_tmp^=SCData;
    USART_SendData(SC3_USART, SCData);
    while(USART_GetFlagStatus(SC3_USART, USART_FLAG_TC) == RESET);
      
    /* Send PTS0 */
    SCData = 0x10|(Protocol&0x0f);
    CRC_tmp^=SCData;
    USART_SendData(SC3_USART, SCData);
    while(USART_GetFlagStatus(SC3_USART, USART_FLAG_TC) == RESET);
    
     /*Send PTS1 */
    SCData = ((F_tmp<<4)&0xf0)|(D_tmp&0x0f); 
    CRC_tmp^=SCData;
    USART_SendData(SC3_USART, SCData);
    while(USART_GetFlagStatus(SC3_USART, USART_FLAG_TC) == RESET);
      
    /* Send PCK */
    SCData = CRC_tmp; 
    USART_SendData(SC3_USART, SCData);
    while(USART_GetFlagStatus(SC3_USART, USART_FLAG_TC) == RESET);
    (void)USART_ReceiveData(SC3_USART);
    //----------接收
	if(samslot == 1)  
	    if((USART1_ByteReceive(&locData, 9600)) != SUCCESS) 
		{
		    return Card_PPSERR;
		}
    if(samslot == 2)  
	    if((USART2_ByteReceive(&locData, 9600)) != SUCCESS) 
		{
		    return Card_PPSERR;
		}
	if(samslot == 3)  
	    if((USART3_ByteReceive(&locData, 9600)) != SUCCESS) 
		{
		    return Card_PPSERR;
		}
    if(locData!=0xff) 
	{
	    return Card_PPSERR;
	}
    if(samslot == 1)  
	    if((USART1_ByteReceive(&locData, 9600)) != SUCCESS) 
		{
		    return Card_PPSERR;
		}  
    if(samslot == 2)  
	    if((USART2_ByteReceive(&locData, 9600)) != SUCCESS) 
		{
		    return Card_PPSERR;
		}
	if(samslot == 3)  
	    if((USART3_ByteReceive(&locData, 9600)) != SUCCESS) 
		{
		    return Card_PPSERR;
		}
    if(locData!=(0x10|(Protocol&0x0f))) 
	{
	    return Card_PPSERR;
	}
	if(samslot == 1)    
	    if((USART1_ByteReceive(&locData, 9600)) != SUCCESS) 
		{
		    return Card_PPSERR;
		}
    if(samslot == 2)    
	    if((USART2_ByteReceive(&locData, 9600)) != SUCCESS) 
		{
		    return Card_PPSERR;
		}
	if(samslot == 3)    
	    if((USART3_ByteReceive(&locData, 9600)) != SUCCESS) 
		{
		    return Card_PPSERR;
		}
    if(locData!=(((F_tmp<<4)&0xf0)|(D_tmp&0x0f)))	
	{
	    return Card_PPSERR;
	}
      
    if(samslot == 1)    
	    if((USART1_ByteReceive(&locData, 9600)) != SUCCESS) 
		{
		    return Card_PPSERR;
		}
	if(samslot == 2)    
	    if((USART2_ByteReceive(&locData, 9600)) != SUCCESS) 
		{
		    return Card_PPSERR;
		}
	if(samslot == 3)    
	    if((USART3_ByteReceive(&locData, 9600)) != SUCCESS) 
		{
		    return Card_PPSERR;
		}
    if(locData!=CRC_tmp) 
	{
	    return Card_PPSERR;
	}
      
    //成功，改变USART的值
    Parameter[samslot].CARD_PROTOCOL=Protocol;	
    delay_Init();
    SC_UART_Init3(other_data);
      
    TIM4_Init(Parameter[samslot].CARD_F);
    return Card_OK;
}
/*
 *******************************************************************************
 * @brief  TIM4_IRQHandler
 * @param  none
 * @retval none  
 *******************************************************************************
 */
/*------------------------------------------------------------------------------
  Timer4 Update Interrupt Handler  (used for Delay function)
  Timer4 Update Interrupt happens every 10 ms
 *----------------------------------------------------------------------------*/
void TIM4_IRQHandler(void) 
{	
    if (TIM4->SR & (1<<0)) 
    {                        					  
       TIM4->SR &= ~(1<<0);                        // clear UIF flag
       Timer4_over=IS_OVER;
    }
} // end TIM4_IRQHandler
/*
 *******************************************************************************
 * @brief  TIM4_Init
 * @param  none
 * @retval none  
 *******************************************************************************
 */
void TIM4_Init(u16 Prescale)
{ 
	if((RCC->APB1ENR&RCC_APB1ENR_TIM4EN)==0)
  	    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    if(use_clock_type==hse_72mhz)
        //--------------v1.24 2009-12-16 edit1:start
        TIM4->PSC = Prescale*16;                  // set prescaler
        //--------------v1.24 2009-12-16 edit1:end
    else
        TIM4->PSC = Prescale*2;
			
    TIM4->CR1 &= ~0x0001;                         // disable timer
    TIM4->SR &= ~(1<<0);                          // clear UIF flag
    Timer4_over=NOT_OVER;
    TIM4->CR1 = (1<<7)|(1<<4)|(1<<3)|(1<<2);

    TIM4->CR2 = 0;
    TIM4->DIER = __TIM4_DIER;                     // enable interrupt
    NVIC->ISER[0] |= (1 << (TIM4_IRQn& 0x1F));    // enable interrupt
    TIM4->EGR|=(1<<0);//更新预分频寄存器
	TIM4->ARR = 40000;
    TIM4->CNT=40000;  
}
/*
 *******************************************************************************
 * @brief  TIM4_Start
 * @param  none
 * @retval none  
 *******************************************************************************
 */
void TIM4_Start(u16 delay)
{			
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	TIM4->CR1 &= ~0x0001;                         // disable timer
	TIM4->SR &= ~(1<<0);                          // clear UIF flag	
	TIM4->ARR = delay;
    TIM4->CNT=delay;
    Timer4_over=NOT_OVER;
    TIM4->CR1 |= TIMX_CR1_CEN;	  
}
/*
 *******************************************************************************
 * @brief  TIM4_Stop
 * @param  none
 * @retval none  
 *******************************************************************************
 */
u16 TIM4_Stop(void)
{
	if(Timer4_over==IS_OVER)
	{ 
		TIM4->CR1 &= ~0x0001;                     // disable timer
		TIM4->SR &= ~(1<<0);                      // clear UIF flag	
		return IS_OVER;	
	}
	else 
	    return NOT_OVER;		
}
/*
 *******************************************************************************
 * @brief  TIM4_Close
 * @param  none
 * @retval none  
 *******************************************************************************
 */
void TIM4_Close(void)
{ 
	TIM4->CR1 &= ~0x0001;                         // disable timer
	TIM4->SR &= ~(1<<0);                          // clear UIF flag	
}
/*
 *******************************************************************************
 * @brief  SC_Test
 * @param  none
 * @retval none  
 *******************************************************************************
 */

#if	0 
void SC_Test(void)
{

    u8  buf[60];
	u8	samflagts=0;
//    u8  comm[8];
    u16 leng,tmp;

	Card_Init();

    /* Loop while no Smartcard is detected */  
    //while(CardInserted == 0)
    //{
    //}    
 	//SerialPutChar(0xff);
    tmp = Card_PowerOn(&leng,&buf[0]);
	//SerialPutChar(0xee);
	//SerialPutChar(tmp);
	//SerialPutChar(0xff);
	//Put_String("\r\n",15);
    samflagts=tmp;
	//SerialPutChar(0x77);
	//SerialPutChar(tmp);
	//SerialPutChar(0x77); 
	if(tmp == OK1)
    {
  	    samflagts=0xbb;
		//LCD_ClearScreen();
  	    //LCD_DrawText(0,0,"Card Rest OK!");
		//Serial_PutString("Card Rest OK!\r\n");
		//SerialPutChar(0x78);
		//Put_String(buf,leng);
		//Put_String("Card Rest OK!\r\n",15);
		return;
    }

    else
    {
  	    samflagts=tmp;
		//LCD_ClearScreen();
  	    //LCD_DrawText(0,0,"Card Rest ERR!");
		//Serial_PutString("Card Rest ERR!\r\n");
		//SerialPutChar(0x89);
		//Put_String("Card Rest ERR!\r\n",16);
		return;
			
    }
	 
    /* 
	comm[0] = 0x00;
    comm[1] = 0x84;
    comm[2] = 0x00;
    comm[3] = 0x00;
    comm[4] = 0x08;
    leng = 5;
    tmp = Card_Apdu(&leng,comm,&buf[0]);
    
	if(tmp == 0x9000)
    {
  	    //LCD_ClearScreen();
  	    //LCD_DrawText(0,0,"Card Adpu OK!");
		//Serial_PutString("Card Adpu OK!\r\n");
		SerialPutChar(0x94);
		//SerialPutChar(leng);
		Put_String(buf,buf[1]+2);
		//Put_String("Card Adpu OK!\r\n",15);
		SerialPutChar(0x94);
    }
    else
    {
  	    //LCD_ClearScreen();
  	    //LCD_DrawText(0,0,"Card Adpu ERR!");
		//Serial_PutString("Card Adpu ERR!\r\n");
		SerialPutChar(0x95);
		//Put_String("Card Adpu ERR!\r\n",16);
	}
	*/
	  
}
#endif
/*
void ddd(unsigned	char	ss)
{
	LCM1604_SetXY(1+0,0);
    Display_BCDToLCD(ss);
	Delay(1000);
}
*/
