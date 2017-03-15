/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : stm32f10x_it.c
* Author             : MCD Application Team
* Version            : V3.1.1
* Date               : 04/07/2010
* Description        : Main Interrupt Service Routines.
*                      This file provides template for all exceptions handler
*                      and peripherals interrupt service routine.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
//#include "platform_config.h"
//#include "stm32f10x_it.h"
#include "usb_lib.h"
#include "usb_istr.h"
#include "usb_pwr.h"
//#include "stm32_eval.h"
#include "hw_config.h"

#define	Uart1_Stx		0x02
#define	Uart1_Etx		0x03
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define Max_SysTime		864000000		//系统时间最大值（10天）
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//extern u8 Key_Val;

			u8		AU_Cmd_Valid = 0, AU_Start = 0;
			u8		AU_Buffer[600];
			u16		AU_Length, AU_Count;
			u8		AUTIM_FLAG;
			u32		AUTIM_start;

			u32     SystemTime;

			u8      Slot_Change;

extern TIME_CNT BeepCtrl;

extern GET_PSD get_psd;
extern	u8	ds8007_int;
//liwei////////////////////////
u8	Cmd_Valid_Usart;//串口解析到一条正确的命令
u8	Cmd_Valid_kb;//Cmd_Valid_kb为解析到一条正确的键盘发来的密码
u8 Cmd_Start;	//header received flag
u16 Rec_Count;	//received data counter through USART1
u8 Rec_Buffer[280];	//received data buffer
u32	U1TIM_start;	//U1TIM_start;
u8	U1TIM_FLAG;
u16	Rec_Len = 0;
extern	u8	Uart1Rec[280];

extern	u8 dssc_readreg(u8 add);
extern	void	SoftCloRf(void);
//   Uart1_
u8	Uart1_Rec_Data, Uart1_Cmd_Sta=0, Uart1_Cmd_Valid=0, Uart1_Bcc=0;
u16  Uart1_Rec_Count=0,	ij, Uart1_Rec_Len=0;

u8 led_flag;
u8	inter_flag;

//liwei////////////////////////
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/*******************************************************************************
* Function Name  : NMI_Handler
* Description    : This function handles NMI exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NMI_Handler(void)
{
}

/*******************************************************************************
* Function Name  : HardFault_Handler
* Description    : This function handles Hard Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : MemManage_Handler
* Description    : This function handles Memory Manage exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : BusFault_Handler
* Description    : This function handles Bus Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : UsageFault_Handler
* Description    : This function handles Usage Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : SVC_Handler
* Description    : This function handles SVCall exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SVC_Handler(void)
{
}

/*******************************************************************************
* Function Name  : DebugMon_Handler
* Description    : This function handles Debug Monitor exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DebugMon_Handler(void)
{
}

/*******************************************************************************
* Function Name  : PendSV_Handler
* Description    : This function handles PendSVC exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PendSV_Handler(void)
{
}

/*******************************************************************************
* Function Name  : SysTick_Handler
* Description    : This function handles SysTick Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_Handler(void)
{
    //TimingDelay_Decrement();
	systicnum++;
}

/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/

#ifndef STM32F10X_CL
/*******************************************************************************
* Function Name  : USB_LP_CAN1_RX0_IRQHandler
* Description    : This function handles USB Low Priority or CAN RX0 interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_LP_CAN1_RX0_IRQHandler(void)
{
  USB_Istr();
}

//5ms的整数倍，开时间
void	TIM2_Cmd_OP(void)
{
	*(u32 *)(0x40000024)=0;
	TIM_Cmd(TIM2, ENABLE);

}
//关定时器
void	TIM2_Cmd_CLO(void)
{
	TIM_Cmd(TIM2, DISABLE);
	*(u32 *)(0x40000024)=0;
}
/*******************************************************************************
* Function Name  : UART2_IRQHandler
* Description    : This function handles UART2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

//liwei 2011-07-21//////////////////////
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		Uart1_Rec_Data= USART_ReceiveData(USART1);
		if((Uart1_Rec_Data == Uart1_Stx) && (Uart1_Cmd_Sta == 0))
		{
			Uart1_Rec_Count = 0;
			Uart1Rec[Uart1_Rec_Count++] = Uart1_Rec_Data;
			Uart1_Cmd_Sta = 1;			//接到头后，置标志
			TIM2_Cmd_OP();		//10ms		   ///此为定时器加的
		}
		else	if(Uart1_Cmd_Sta != 0)
		{
			Uart1Rec[Uart1_Rec_Count++] = Uart1_Rec_Data;
			TIM2_Cmd_OP();		//10ms		  ///此为定时器加的
			if(Uart1_Rec_Count == 3)
			{
				Uart1_Rec_Len = Uart1Rec[1];
				Uart1_Rec_Len <<= 8;
				Uart1_Rec_Len += Uart1Rec[2];
			}
		}
		if(Uart1_Rec_Count == (Uart1_Rec_Len + 5))
		{
			Uart1_Bcc = 0;
			for(ij = 3; ij < Uart1_Rec_Len+4; ij ++)			//建议分开写//不用，此处已经完成接收
				Uart1_Bcc ^= Uart1Rec[ij];
			if((Uart1_Bcc == 0) && (Uart1Rec[Uart1_Rec_Len+4] == Uart1_Etx))
				Uart1_Cmd_Valid = 0x01;
			TIM2_Cmd_CLO();					  ///此为定时器加的
			Uart1_Bcc = 0;
			Uart1_Cmd_Sta = 0;
		}	 
	}	
}
////////////////////////////////////////

/*******************************************************************************
* Function Name  : UART2_IRQHandler
* Description    : This function handles UART2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART2_IRQHandler(void)
{     
}
void AU9540_USART_IRQHandler(void)
{ 	 
}
/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles Timer2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_IRQHandler(void)
{
	//TIM2_irq();


	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //清除TIMx更新中断标志 
		//if(PAPERCHECK == 0) 正常情况 TEMPCHECK ==1
//	led_flag++;
//	if(led_flag%2)\
//	{	LedGreenOff();LedRedOn();}
// 
//	else
//	{	LedGreenOn();LedRedOff();}

	 inter_flag = 1;

	
		
	//	LED1=!LED1;
	}

}
/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles Timer3 interrupt request.1/10ms
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//改变配置来实现
void TIM3_IRQHandler(void)
{	
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	*(u32 *)(0x40000424)=0;
	
	if(BeepOnTimes == 0)		   //响完了
	{
		TIM_Cmd(TIM3, DISABLE);
	}
	else  if(BeepFlag == 1)
	{
		BeepFlag = 0;
		BeepOff();
		Tim3_Ch(BeepOffTime);
		BeepOnTimes -- ;
	}
	else  if(BeepFlag == 0)
	{
		BeepFlag = 1;
		BeepOn();
		Tim3_Ch(BeepOnTime);
	}
	else
	{
		BeepOnTimes = 0;
		TIM_Cmd(TIM3, DISABLE);
	}
}
/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles Timer3 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI0_IRQHandler(void)
{		
}
/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles Timer3 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI4_IRQHandler(void)
{
}
#endif /* STM32F10X_CL */

#ifdef STM32F10X_CL
/*******************************************************************************
* Function Name  : OTG_FS_IRQHandler
* Description    : This function handles USB-On-The-Go FS global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void OTG_FS_IRQHandler(void)
{
  STM32_PCD_OTG_ISR_Handler(); 
}

#endif /* STM32F10X_CL */

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/*******************************************************************************
* Function Name  : PPP_IRQHandler
* Description    : This function handles PPP interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/*void PPP_IRQHandler(void)
{
}*/

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
