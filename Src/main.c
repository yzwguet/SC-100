/******************** (C) COPYRIGHT 2016 SC *******************************
* File Name		: main.c
* Author		: winter
* Version		: V1.0.0
* Date			: 2017-03-15
* Description	: SC-100 Solution reader main file
*****************************************************************************/

/* Includes ------------------------------------------------------------------*/

#include "hw_config.h"
#include "platform_config.h"
#include "rc632.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
u8	Uart1Rec[280], Usb_Uart1 = 0;	//0为USB，1为Usatt1
u32	volatile	systicnum = 0, presystic;
extern const u8 company_name[];
extern const u8 device_name[];
extern	u8		Cmd_Valid;//Cmd_Valid为解析到一条正确的命令
extern	u16 	EE_Init(void);
extern	void	Tda8007_Config(void);
extern	void	Rc632_Spi_Config(void);
extern	void	SoftCloRf(void);
extern	void	SC_UART_Init(u8 slot);

extern void	SendToKB(u8	ch);

extern	u8	Uart1_Cmd_Valid;
extern	u16 EE_ReadVariable(u16 VirtAddress, u16* Data);
extern	u16 EE_WriteVariable(u16 VirtAddress, u16 Data);
/* Private function prototypes -----------------------------------------------*/
extern	u16	bhmc[40],yssj[30];
extern	u8	Led_Ctl;
extern	u8  samslot;
void Delay(__IO uint32_t nCount);
u8	volatile	BeepOnTime , BeepOffTime , BeepOnTimes , BeepFlag ;
extern u16 VirtAddVarTab[255];
extern u8   ONOFF_buf;
extern CMD_DAT pc_cmd;
extern u8	inter_flag;
u8	mt_Atr_buf[250];
u16 mt_Sta;
u16 mt_Atr_len;
u8	mt_RfCard_flag;



/* Private functions ---------------------------------------------------------*/
u16 MT_ReadSpecialSCard(u8 *pt)
{
	s16 status;
	u16 apdu_sta;
	u8  temp[280];
	u8  slect_cmd[9]= {0x00,0xa4,0x00,0x00,0x02,0xef,0x11};
    u8  authen_cmd[13] = {0x00,0x20,0x00,0x00,0x06,0x11,0x22,0x33,0x44,0x55,0x66};
	u8  read_cmd[7] = {0x00,0xb0,0x00,0x00,0xf8};
	status = RfApduA(slect_cmd, 7, temp);
	apdu_sta = temp[0]*256+temp[1];
	if((status!=2)||(apdu_sta != 0x9000))
		return status;
	status = RfApduA(authen_cmd, 11, temp);
	apdu_sta = temp[0]*256+temp[1];
	if((status!=2)||(apdu_sta != 0x9000))
		return status;
	status = RfApduA(read_cmd, 5, pt);
	if(status!=0xfa)
		return status;	
//	memcpy(pt,temp+2,248);
	return 0;
}

/*******************************************************************************
* Function Name  : main.
* Description    : main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int main(void)
{  
	u8 i;
	
	Set_System();
	/* Setup System Ticks */
	NVIC_SetVectorTable(0x08000000,0x00004000);
	SysTick_Config(SystemFrequency / 1000);
  
	/* USB HID */
	Set_USBClock();
	USB_Init();
	USB_Interrupts_Config();
  
	LED_Config();
	LedRedOn();

	BEEP_Config();	

	USART1_Config();
	Timer2_Config();
	Timer3_Config();

	SC_UART_Init(2);
	SC_UART_Init(3);

	Rc632_Spi_Config(); 		
	  
	FLASH_Unlock();
	EE_Init();
	FLASH_Lock(); 

	BeepOn();
	Delay_Ms(200);
	BeepOff();


	while (1)
	{	
#if 1
	  if(mt_RfCard_flag==0)
	  {  
		Fm1702Reset(3);  
		mt_Sta = RequCpu(0, mt_Atr_buf, &mt_Atr_len); 
		if(mt_Sta==0)
			mt_Sta = MT_ReadSpecialSCard(mt_Atr_buf);
		if(mt_Sta==0)	  
		{
			MT_BeepOnce();
			LedRedOff();LedGreenOn();
			Delay_Ms(20);
			LedRedOn();LedGreenOff();
			mt_RfCard_flag = 1;

			for(i=0;i<250;i++)
			{
				SendToKB(mt_Atr_buf[i]);
				Delay_Ms(2);
			}

		}	  
	  }
	  else
	  {
			mt_Sta = GetRFStatus();
		    if(mt_Sta != 0)
			{	
				mt_RfCard_flag = 0;
				//Fm1702Reset(3);
				SoftCloRf();
				//Delay_Ms(4);
			}	
	  }
#endif
	if(inter_flag)
	{
		inter_flag = 0;
	   	TIM_Cmd(TIM2, DISABLE); 
		LedRedOn();LedGreenOff();
	}  	
//	USB_DeCode();	//USB通讯解析		
	if(Cmd_Valid)	//USB下发指令执行入口
	{		
		Usb_Uart1 = 0;
		//LedGreenOff(); 		
		LedRedOff();LedGreenOn();
		Timer2_Config();
		PC_CommandDispatcher();	
		//LedGreenOn();
		//Delay_Ms(20); 			
	}	
	if(Uart1_Cmd_Valid == 1)//串口下发指令执行入口
	{	
		//if(PCCommandValid(Rec_Buffer) == CMD_OK)//串口数据解析
		Usb_Uart1 = 1;
		PCCommandValid(Uart1Rec);
		PC_CommandDispatcher();	
	}
		
	}
}



/******************* (C) COPYRIGHT 2017 SC *****END OF FILE****/
