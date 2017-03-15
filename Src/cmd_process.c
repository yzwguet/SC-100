/******************** (C) COPYRIGHT 2011 Mingtech ******************************
* File Name          : cmd_process.c
* Author             : zhenhua mo
* Version            : V1.0.0
* Date               : 07/09/2011
* Description        : pd reader main command process file
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "cmd_process.h"
#include "rc632.h"
#include "sc.h"
#include "hw_config.h"
//#include <stdio.h>
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
//===write by yzwguet 2014/4/30=============//
extern  uchar   mifare_SAK;   
u8   compare_temp;
u8   ONOFF_buf = 0;
//===========================================//

CMD_DAT pc_cmd;
MSG_DAT reader_msg;

TIME_CNT BeepCtrl;
TIME_CNT ReadMagCard_OverTime;
extern u8 Cmd_Valid,Cmd_Valid_kb;
extern u8 Key_Val;
extern u8 Rec_Buffer[280];
extern u8 Rec_Count;
extern u8 Cmd_Start;
extern u8 Cmd_Valid;
extern u8 Key_Val;
extern u16 VirtAddVarTab[30];
extern	u8 Usb_Uart1;
extern	u8	Uart1_Cmd_Valid;
extern const u8 dotchar[];
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void CMD_Read9(MSG_DAT* msg);
void Uart1_TX_STA(s16	stats, u8 *pt,u16 len);
extern	u16 EE_ReadVariable(u16 VirtAddress, u16* Data);
extern	u16 EE_WriteVariable(u16 VirtAddress, u16 Data);
extern	void Fm1702Reset(u16 ms);
extern	u16	RequCpu(u16	MyDelayTime, u8	*pt, u16	*Atrlen);
extern	s16		RfApduA(u8 *req_buf, u16 req_len, u8 *resp_buf);
extern  u8 MIF_Halt(void);
extern  void	SoftOpenRf(void);
//extern	u16 Card_Apdu(u16* Length,  u8* Command,  u8* Response);
extern  u16 Card_Apdu(u16 Length, u8* uCommand,u8* Response)	;
extern  void	SoftCloRf(void);
extern	u16 Card_PowerOn(u8 RstBaud,u16 *Length,u8 * Atr);

u8	samslot = 0, ChBdFlag=0, BaudRd;;


u8 CardNowSta = 0; 
u8	CardRstSta[4] = {0, 0, 0, 0};
u8 tss[2];
u16	bhmc[200];
u32 IapFlag_Add=(0x08004000-0x400);
u8 SAM1_flag = 0;
u8 SAM2_flag = 0;

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void CMD_GetVerson(MSG_DAT* msg)
{
		
	 //u8 verson[] = "MT2-URF-R330-HK V1.02";
	 u8 verson[] = "MT2-URF-R330-SZSB V1.00";//2015/5/15 add by yzw
	 msg->sta = OP_OK;
	 //msg->len = __REV16(data_len);
	 msg->len = sizeof(verson)-1;
	 //memcpy(&msg->dat[0],verson,data_len);
	 memcpy(&msg->dat[0],verson, msg->len);

	 //msg->bSizeToSend += data_len ;
	 //msg->bSizeToSend += msg->len ;

}
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void CMD_ReadEEPROM(MSG_DAT* msg)
{
     u8  eeprom[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
				  };
	 u16 data_len = 16;

     msg->stx = 0x55;
	 msg->sta = OP_OK;
	 msg->len = __REV16(data_len);	 

	 memcpy(msg->dat,eeprom,data_len);
	 //msg->bSizeToSend += data_len ;
}
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void CMD_BeepCtrl(MSG_DAT* msg)
{
     u16 data_len = 0;	 
	 	 
	 BeepCtrl.cnt  = pc_cmd.dat[1];	 	 
	 BeepCtrl.flag = 1;

	 msg->sta = OP_OK;
	 msg->len = __REV16(data_len);
	 
	 //msg->bSizeToSend += data_len ;
	 
}
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
GET_PSD get_psd;

void CMD_SetKeyboard(MSG_DAT* msg)
{
     u16 data_len = 0;
	 
	 get_psd.num = pc_cmd.dat[0]+1;
	 get_psd.delay_sec = pc_cmd.dat[1];

	 msg->stx = 0x55;
	 msg->sta = OP_OK;
	 msg->len = __REV16(data_len);
	 
	 //msg->bSizeToSend += data_len ;


}
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void CMD_SetCodeKey(MSG_DAT* msg)
{
     u16 data_len = 0;	 
	 
	 memcpy(&get_psd.code_key[0], &pc_cmd.dat[0],8);

	 msg->stx = 0x55;
	 msg->sta = OP_OK;
	 msg->len = __REV16(data_len);
	 
	 //msg->bSizeToSend += data_len ;
}
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void CMD_SetKeyboard2(MSG_DAT* msg)
{
     u16 data_len = 0;	 
	 
	 get_psd.trans_mode = pc_cmd.dat[0];

	 msg->stx = 0x55;
	 msg->sta = OP_OK;
	 msg->len = __REV16(data_len);
	 
	 //msg->bSizeToSend += data_len ;

}
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void CMD_SetMagcard(MSG_DAT* msg)
{
     u16 data_len = 0;	 
	 
	 ReadMagCard_OverTime.cnt = pc_cmd.dat[0];//单位为秒
	 //ReadMagCard_OverTime.flag = 0;//先不开启
	 
	 msg->sta = OP_OK;
	 msg->len = __REV16(data_len);
	 
	 //msg->bSizeToSend += data_len ;
}
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void PC_CommandDispatcher(void)
{
	u8  MessageToSend = 0, i=0, j=0, k=0;
	s16 status1;
	u16 len, lenin, status;
	u32 delay_time;
//	u16 temp1,temp2;
//	u8  tempdata;
	u16 Delay=0xffff;
	
	//---通讯应答
	if(Cmd_Valid || Uart1_Cmd_Valid)
	{
	    Cmd_Valid = 0;
		Uart1_Cmd_Valid = 0;
		reader_msg.stx = 0x02;
		memset(reader_msg.dat,0,RD_DAT_SIZE);
		/*
		if((pc_cmd.cmd != 0x0003) && (pc_cmd.cmd != 0x3114))
		{	
			LedRedOff();
			LedGreenOn();
		}
		*/
		switch(pc_cmd.cmd)		 
		{
		    case CMD_GetVer:
			case CMD_RDVER:
				 CMD_GetVerson(&reader_msg);
		         MessageToSend = 1;
			    break;
			case	0x0002:
			case	0x3113:		//OK 蜂鸣： open(5)  close  times
				BeepOnTimes = pc_cmd.dat[0];
				BeepOffTime = pc_cmd.dat[1];
				BeepOnTime = pc_cmd.dat[2];
				if(BeepOnTime)
				{
					Tim3_Ch(BeepOnTime);
					//TIM_Cmd(TIM3, ENABLE);
					BeepOn();
					BeepFlag = 1;	   //0表示响，1表示关
				}
				reader_msg.sta = MW_OK;
				reader_msg.len = 0;
				MessageToSend = 1;
				break;
//=====================================================================//
			///////////////////下载接口////////////////
			case 0x00d0:
				status1 = 0;
				if(Usb_Uart1 == 0)
				    USB_Command_ReplyTogether(&reader_msg.stx,reader_msg.len);
				else	//串口发送				
					Uart1_TX_STA(reader_msg.sta, reader_msg.dat, reader_msg.len);
				//Uart1_TX(0, reader_msg.dat,0);
				while(Delay--);
				//if(FLASH_COMPLETE == FLASH_ErasePage(0x08004000-PAGE_SIZE))//清除下载状态标志和完整性标志  
				FLASH_Unlock();
				if(FLASH_COMPLETE == FLASH_ErasePage(0x08004000-0x400))//清除下载状态标志和完整性标志  
				{
					 //设置下载状态标志
					if(FLASH_ProgramHalfWord(IapFlag_Add, 0x2211)==FLASH_COMPLETE)
					{						
					 __set_FAULTMASK(1);      // 关闭所有中端
					 NVIC_SystemReset();// 软件复位	
					}
				}
				MessageToSend = 1;					
				break;

//=====================================================================//

			case	0x0004:
			case	0x3001:			//OK					 //baud 设置
				//if(Rec_Buffer[6] >6)
				if(pc_cmd.dat[0] >6)
				{
					status = MW_EMPTY;//CMDERR_PARAM;	//参数错误	//Send_Buffer[1] = 0x01;
					reader_msg.sta = __REV16(status);									
				}
				else
				{
					//Send_Buffer[1] = 0x00;		
					status1 = 0;	
					//status = pc_cmd.dat[0];
					BaudRd = pc_cmd.dat[0];
					FLASH_Unlock();
					status1 = EE_WriteVariable(VirtAddVarTab[0],pc_cmd.dat[0]);
					FLASH_Lock();
					if(status1 != 0x04)
					{
					 	status1 = WRITE_FLASH_ERR;
						reader_msg.sta = __REV16(status1);
					}	
						//Uart1_TX(status1, Send_Buffer, 0);
					else
					{
						reader_msg.sta = MW_OK;
					}
						//Uart1_TX(0, Send_Buffer, 0);
					Delay_Ms(2);
					if(status1 == 0x04)
					{
						ChBdFlag = 1;
					}
					//	Usart1_Chbaud(status);	   //Rec_Buffer[5]是功能模块选项，没有处理
				}
				reader_msg.len = 0;
				MessageToSend = 1;
				break;
			case	0xc130:				//OK		打开非接CPU卡	   02 00 04 c1 30 00 01 f0 03
					//tss[0] = 0xf1;
					Fm1702Reset(3);			//复位激活非接CPU卡
					delay_time = 0;
					//status = RequCpu(delay_time, Send_Buffer, &len);
					status = RequCpu(delay_time, reader_msg.dat, &len);
					//if(status != 0)
					//{
						//LedGreenOff();
					//}
					//tss[0] = 0;
					reader_msg.sta = __REV16(status);
					reader_msg.len = len;
					MessageToSend = 1;
					//Uart1_TX(status, Send_Buffer, len);
					//Uart1_TX(status, reader_msg.dat, len);
					break;

				case	0xc131:					   //OK		02 00 08 c1 31 00 00 84 00 00 08 7c 03
					lenin = pc_cmd.len;
					lenin -= 3;
					//status1 = RfApduA(Rec_Buffer+6, lenin, Send_Buffer + 2);
					//status1 = RfApduA(Rec_Buffer+6, lenin, Send_Buffer);
					status1 = RfApduA(pc_cmd.dat, lenin, reader_msg.dat);
					Delay_Ms(2);
					if(status1 < 0)
					{
						reader_msg.len = 0;
						status1 = MW_CPUERR;
						reader_msg.sta = __REV16(status1);
						//tss[0] = 0;			  
						//Uart1_TX(1, Send_Buffer, 0);
						//Uart1_TX(1, reader_msg.dat, 0);
					}
					else									   //成功了，不光灯
					{	reader_msg.len = status1;
						reader_msg.sta = MW_OK;
						//Uart1_TX(0, Send_Buffer, status1);
						//Uart1_TX(0, reader_msg.dat, status1);
					}
					MessageToSend = 1;
					//tss[0] = 0;
					break;
				case	0xc132:			//停卡操作吧	需要由测试协议来确定   02 00 03 c1 32 00 f3 03
					MIF_Halt();
					SoftCloRf();
					Delay_Ms(5);
					SoftOpenRf();
					reader_msg.len = 0;
					reader_msg.sta = MW_OK;
					MessageToSend = 1;
					break;
 
			/************************SAM1__U3************************/
			case	0xc230:	
					/*
					if(!( Get_Card_Status() & SAM3_STA))	 
					{
						CardRstSta[3] = 0;
						status = 0x1002;
						reader_msg.len = 0;	
						reader_msg.sta = __REV16(status);
					}
					*/
					//else
					{
						samslot = 3;
						status1 = Card_PowerOn(pc_cmd.unused,&len,reader_msg.dat);					
						if(status1)
						{
							status1 = MW_SAMRST_ERR;
							CardRstSta[3] = 0;
						}
						else
						{
							status1 =  MW_OK;
							CardRstSta[3] = 1;	  //OK
							SAM1_flag = 1;		  //write by yzwguet 2014/5/16

						}
						reader_msg.len = len;	
						reader_msg.sta = __REV16(status1);
					}
					MessageToSend = 1;
					break;
			case	0xc231:
					lenin = pc_cmd.len;
					lenin -= 3;
					/*
					if(!( Get_Card_Status() & SAM3_STA))	//if(!Get_Card_Status())
					{
						CardRstSta[3] = 0;
						status = 0x1002;
						reader_msg.len = 0;	
						reader_msg.sta = __REV16(status);
						MessageToSend = 1;
						break;
					}
					*/
					samslot = 3;
					//status = Card1_Apdu(&lenin , pc_cmd.dat, reader_msg.dat);
					if(SAM1_flag ==1)
					{
					
						status = Card_Apdu(lenin , &pc_cmd.dat[0], &reader_msg.dat[0]);
						reader_msg.len = (reader_msg.dat[0] << 8) + reader_msg.dat[1];
						memcpy(&reader_msg.dat[0],&reader_msg.dat[2],(reader_msg.len));
						if((status&0x9000) || (status&0x6000))
							status = MW_OK;
				    	else
							status = MW_SAMAPDU_ERR;
								
						reader_msg.sta = __REV16(status);
						MessageToSend = 1;
					}
					else
					{
						reader_msg.len = 0;
						status = MW_SAMAPDU_ERR;	
						reader_msg.sta = __REV16(status);
						MessageToSend = 1;					
					}

					break;	 

				case	0xc232:
					CardRstSta[3] = 0;	
					samslot = 3;
					GPIO_ResetBits(GPIO3_RESET, SC3_RESET);
					reader_msg.len = 0;	
					reader_msg.sta = MW_OK;
					MessageToSend = 1;
					SAM1_flag = 0;

					break;
			/************************SAM2__U2************************/
			case	0xc330:	
					/*
					if(!( Get_Card_Status() & SAM2_STA))	 
					{
						CardRstSta[2] = 0;
						status = 0x1002;
						reader_msg.len = 0;	
						reader_msg.sta = __REV16(status);
					}
					*/
					//else
					{
						samslot = 2;
						status1 = Card_PowerOn(pc_cmd.unused,&len,reader_msg.dat);					
						if(status1)
						{
							status1 = MW_SAMRST_ERR;
							CardRstSta[2] = 0;
						}
						else
						{
							status1 =  MW_OK;
							CardRstSta[2] = 1;
							SAM2_flag = 1;
						}
						reader_msg.len = len;	
						reader_msg.sta = __REV16(status1);
					}
					MessageToSend = 1;
					break;
			case	0xc331:
					lenin = pc_cmd.len;
					lenin -= 3;
					/*
					if(!( Get_Card_Status() & SAM2_STA))	//if(!Get_Card_Status())
					{
						CardRstSta[2] = 0;
						status = 0x1002;
						reader_msg.len = 0;	
						reader_msg.sta = __REV16(status);
						MessageToSend = 1;
						break;
					}
					*/
					samslot = 2;
					//status = Card1_Apdu(&lenin , pc_cmd.dat, reader_msg.dat);
					if(SAM2_flag ==1)
					{
						status = Card_Apdu(lenin , &pc_cmd.dat[0], &reader_msg.dat[0]);
						reader_msg.len = (reader_msg.dat[0] << 8) + reader_msg.dat[1];
						memcpy(&reader_msg.dat[0],&reader_msg.dat[2],(reader_msg.len));
						if((status&0x9000) || (status&0x6000))
							status = MW_OK;
						else
							status = MW_SAMAPDU_ERR;
						reader_msg.sta = __REV16(status);
						MessageToSend = 1;
					}
					else
					{
						reader_msg.len = 0;
						status = MW_SAMAPDU_ERR;
						reader_msg.sta = __REV16(status);
						MessageToSend = 1;					
					}


					break;
				case	0xc332:
					CardRstSta[2] = 0;	
					samslot = 2;
					GPIO_ResetBits(GPIO2_RESET, SC2_RESET);
					reader_msg.len = 0;	
					reader_msg.sta = MW_OK;
					MessageToSend = 1;
					SAM2_flag = 0;
					break;


			
#if 1
			/**************************序列号*************************/
			case   0x0008:					//		02 00 13 00 08 00 10 data bcc 03
					status1 = 0;
					FLASH_Unlock();
					k = (pc_cmd.dat[0]);
					//k = (Rec_Buffer[6]/2 + Rec_Buffer[6] % 2);
					for(i=0;i < k;i++)
					{								  //219 对应序列号的第一个数据地址
					
//						status1=EE_WriteVariable(VirtAddVarTab[1+i],(pc_cmd.dat[1+j]<<8)+pc_cmd.dat[1+j+1]);
						status1=EE_WriteVariable(VirtAddVarTab[8+i],(pc_cmd.dat[1+i]<<8)+0);
						//==================10H-2FH为生产序列号区域================//
					}
//===========================================
			
						             
//===========================================

					FLASH_Lock();
					if(status1 != 0x04)	
					{
						reader_msg.len = 0;
						 reader_msg.sta = __REV16(status1);
				         MessageToSend = 1;
					}
					else
					{
						 reader_msg.len = 0;
						 reader_msg.sta = MW_OK;
				         MessageToSend = 1;	
					}	
					break;
				case   0x0009:			 //   02 00 04 00 09 00 10 bcc 03

					//for(i=0;i<Rec_Buffer[6];i++)
					k = (pc_cmd.dat[0]);
					for(i=0;i<k;i++)
					{
//						EE_ReadVariable(VirtAddVarTab[1+i], &bhmc[i]);
						EE_ReadVariable(VirtAddVarTab[8+i], &bhmc[i]);
						//==================10H-2FH为生产序列号区域================//
					
						//Send_Buffer[j] = bhmc[i]>>8; 
						//Send_Buffer[j+1]= bhmc[i];
						reader_msg.dat[j++] = bhmc[i]>>8; 
						//reader_msg.dat[j+1]= bhmc[i];
					}
				//	j=i;
					//if(j!=Rec_Buffer[6])
//					if(i!=pc_cmd.dat[0]-2)
//				
//					{
//						reader_msg.len = 0;
//						status1 = 34;
//						 reader_msg.sta = __REV16(status1);
//						//Uart1_TX_STA(status1, Send_Buffer, 0);
//						 //Uart1_TX(0, Send_Buffer, 0);
//				         MessageToSend = 1;
//					}
						//USB_Command_ReplyTogether(34, Send_Buffer, 0);			 //写序列号失败
//					else
//					{
						 reader_msg.len = pc_cmd.dat[0];										 //V1.04修改了此语句  
						 //reader_msg.len = 0;
						 reader_msg.sta = MW_OK;
				         MessageToSend = 1;
//					}
						//USB_Command_ReplyTogether(0, Send_Buffer, j);
					break;
#endif
#if 0
			/**************************少量数据存储*************************/
			case   0x000a:					//		02 00 13 00 08 00 10 data bcc 03
					status1 = 0;
					FLASH_Unlock();
					k = (pc_cmd.dat[0]/2 + pc_cmd.dat[0] % 2);
					for(i=0;i < k;i++)
					{								  //15*2 对应序列号的第一个数据地址
						j=i*2;
						status1=EE_WriteVariable(VirtAddVarTab[16+i],(pc_cmd.dat[1+j]<<8)+pc_cmd.dat[1+j+1]);						
					}
					FLASH_Lock();
					if(status1 != 0x04)	
					{
						reader_msg.len = 0;
						 reader_msg.sta = __REV16(status1);
				         MessageToSend = 1;
					}
					else
					{
						 reader_msg.len = 0;
						 reader_msg.sta = MW_OK;
				         MessageToSend = 1;	
					}	
					break;
				case   0x000b:			 //   02 00 04 00 09 00 10 bcc 03
					//for(i=0;i<Rec_Buffer[6];i++)
					for(i=0;i<pc_cmd.dat[0];i++)
					{
						EE_ReadVariable(VirtAddVarTab[16+i], &bhmc[i]);
						//==================10H-2FH为生产序列号区域================//
						j=i*2;
						//Send_Buffer[j] = bhmc[i]>>8; 
						//Send_Buffer[j+1]= bhmc[i];
						reader_msg.dat[j] = bhmc[i]>>8; 
						reader_msg.dat[j+1]= bhmc[i];
					}
					j=i;
					//if(j!=Rec_Buffer[6])
					if(j!=pc_cmd.dat[0])
					{
						reader_msg.len = 0;
						status1 = 34;
						 reader_msg.sta = __REV16(status1);
						//Uart1_TX_STA(status1, Send_Buffer, 0);
						 //Uart1_TX(0, Send_Buffer, 0);
				         MessageToSend = 1;
					}
						//USB_Command_ReplyTogether(34, Send_Buffer, 0);			 //写序列号失败
					else
					{
						 reader_msg.len = j;
						 reader_msg.sta = MW_OK;
				         MessageToSend = 1;
					}
						//USB_Command_ReplyTogether(0, Send_Buffer, j);
					break;
#endif
#if 1
				case	0x000a:	   //02 00 xx 00 0a 00 addr(00-a0) lenH lenL  data bcc 03 写
					status1 = 0;   			
					k = pc_cmd.dat[2]*256+pc_cmd.dat[3];  													 

						FLASH_Unlock();
					
						for(i=0;i<k;i++)
						{								  			 //addH addL LenH LenL dat0 dat1 dat2    datn
															 //  0    1    2    3    4    5    6  
							status1=EE_WriteVariable(VirtAddVarTab[pc_cmd.dat[1]+i],(pc_cmd.dat[4+i]<<8)+0);							                    
							if(status1 != 0x04)	
							{
								reader_msg.len = 0;
								reader_msg.sta = __REV16(status1);
						        MessageToSend = 1;
								break;
							}
						}
					                    
						if(status1 != 0x04)	
						{
							reader_msg.len = 0;
							reader_msg.sta = __REV16(status1);
					        MessageToSend = 1;
							break;
						} 

					 reader_msg.len = 0;
					 reader_msg.sta = MW_OK;
			         MessageToSend = 1;	
					break;
				case	0x000b:	   //02 00 xx 00 0b 00 addr(00-a0) lenH lenL  data bcc 03 读

					k = pc_cmd.dat[2]*256+pc_cmd.dat[3];

					for(i=0;i<k;i++)
					{bhmc[i]=0;}

					for(i=0;i<k;i++)
					{
						EE_ReadVariable(VirtAddVarTab[pc_cmd.dat[1]+i], &bhmc[i]);				
						
						//=====================================31H开始为用户区================//
					
						reader_msg.dat[i] = bhmc[i]>>8;  					
					}					
				

				//	reader_msg.len = j;			
					reader_msg.len = pc_cmd.dat[2]*256+pc_cmd.dat[3];  
					reader_msg.sta = MW_OK;
			        MessageToSend = 1;
					break;
#endif
#if 1			
			/********************M1卡相关***************************/
			case	0xc14e:			//射频复位Rec_Buffer[6]*256+Rec_Buffer[7]返回数据	02 00 05 c1 4e 00 T1 T2 Bcc 03
					SoftCloRf();
					Delay_Ms(pc_cmd.dat[0]*256+pc_cmd.dat[1]);					   //OK
					SoftOpenRf();
					reader_msg.len = 0;
					 reader_msg.sta = MW_OK;
			         MessageToSend = 1;
					//Uart1_TX_STA(0, Send_Buffer, 0);
					//USB_Command_ReplyTogether(0, Send_Buffer, 0);
					break;
			case	0xc140:		//打开卡片,会返回卡片的ID号					///需要进一步处理,得到卡号
				SoftOpenRf();												//OK
				SeriNum = 0;
				status1 = Request(pc_cmd.dat[0]);
				if(status1 != 0)
				{
					reader_msg.len = 0;
					status1 = MW_NOTAG;
					 reader_msg.sta = __REV16(status1);
				}
					//USB_Command_ReplyTogether(22, Send_Buffer, 0);
				else
				{
					status1 = Mf500PiccCascAnticoll();		   //
					if(status1 != 0)
					{
						reader_msg.len = 0;
						//status1 = 8;
						 reader_msg.sta = __REV16(status1);
					}
						//USB_Command_ReplyTogether(8, Send_Buffer, 0);
					else
					{
					//==============write by yzwguet 2014/4/30===========================//
	   	                compare_temp=UIDCasc[0]^(~UIDCasc[1]);
	                    if(ONOFF_buf==0xAA)
	                     {
	                        if(mifare_SAK==compare_temp)
	                        {
							 reader_msg.len = SeriNum;
							 //reader_msg.sta = 2;
							 reader_msg.sta = MW_OK;
							 
							 memcpy(reader_msg.dat,UIDCasc,SeriNum); 
							
							}    
	                        else
	                        {
	                           	reader_msg.len = 0;
							 	reader_msg.sta = 40;  //不是目标卡
	                            MIF_Halt();
	                        }
	                     }
	                    else
						//=======================================					
						{
							 reader_msg.len = SeriNum;
							 reader_msg.sta = MW_OK;
							 memcpy(reader_msg.dat,UIDCasc,SeriNum); 						
						}
				         
					}				
						//USB_Command_ReplyTogether(0, UIDCasc, SeriNum);
				}
				MessageToSend = 1;
				break;
			case 0xc145:				 //   c245
					//status = MIF_Halt();
					MIF_Halt();
					reader_msg.len = 0;
					 reader_msg.sta = MW_OK;
			         MessageToSend = 1;
						//Uart1_TX_STA(0, Send_Buffer, 0);	   //调用halt
						//USB_Command_ReplyTogether(0, Send_Buffer, 0);
					break;
				//case 0xc135://
				case 0xc146://0xc135://					//读数据  OK c246ff04
				 
					//status1 = MIF_READ(Send_Buffer,Rec_Buffer[6]);		 //OK
					status1 = MIF_READ(reader_msg.dat,pc_cmd.dat[0]);		 //OK
					if(status1 != FM1702_OK)
					{
						reader_msg.len = 0;
						status1 = MW_READ;//25;
						 reader_msg.sta = __REV16(status1);
					}
						//USB_Command_ReplyTogether(25, Send_Buffer, 0);
		 			else
					{
						reader_msg.len = 16;
						 reader_msg.sta = MW_OK;				         
					}
						//USB_Command_ReplyTogether(0, Send_Buffer, 16);
					MessageToSend = 1;
					break;

				case 0xc147:		          //写数据	 OK

					//status1 = MIF_Write(&Rec_Buffer[7],Rec_Buffer[6]);
					status = MIF_Write(&pc_cmd.dat[1],pc_cmd.dat[0]);
					if(status != FM1702_OK)
					{
						reader_msg.len = 0;
						status1 = MW_WRITE;//26;
						 reader_msg.sta = __REV16(status1);
					}
						//USB_Command_ReplyTogether(26, Send_Buffer, 0);
					else
					{
						 reader_msg.len = 0;
						 reader_msg.sta = MW_OK;
					}	
						//USB_Command_ReplyTogether(0, Send_Buffer, 0);
					MessageToSend = 1;
					break;	 
				
				case 0xc148:	   //增值		.

					//status1 = MIF_Increment(&Rec_Buffer[7],Rec_Buffer[6]); // 数据头指针   块号
					status = MIF_Increment(&pc_cmd.dat[1],pc_cmd.dat[0]); // 数据头指针   块号
					if(status != FM1702_OK)
					{
						reader_msg.len = 0;
						status1 = MW_INC;//27;
						 reader_msg.sta = __REV16(status1);
					}
						//USB_Command_ReplyTogether(27, Send_Buffer, 0);
					else	
					{
						reader_msg.len = 0;
						 reader_msg.sta = MW_OK;
					}
						//USB_Command_ReplyTogether(0, Send_Buffer, 0);
					MessageToSend = 1;
					break;	
				
				case  0xc149:	  //减值操作
					//status = MIF_Decrement(&Rec_Buffer[7],Rec_Buffer[6]);
					status = MIF_Decrement(&pc_cmd.dat[1],pc_cmd.dat[0]); 
					if(status != FM1702_OK)
					{
						reader_msg.len = 0;
						status1 = MW_DEC;//28;
						 reader_msg.sta = __REV16(status1);
					}
						//USB_Command_ReplyTogether(28, Send_Buffer, 0);
					else
					{
						reader_msg.len = 0;
						 reader_msg.sta = MW_OK;	
					}	
						//USB_Command_ReplyTogether(0, Send_Buffer, 0);
					MessageToSend = 1;
					break;		
				
				case 0xc14a: 
					//status=MIF_Restore(Rec_Buffer[6]);
					status=MIF_Restore(pc_cmd.dat[0]);
					if(status != FM1702_OK)
					{
						reader_msg.len = 0;
						status1 = MW_RESTORE;//29;
						 reader_msg.sta = __REV16(status1);
					}
						//Uart1_TX_STA(29, Send_Buffer, 0);	   //调用重载失败
						//USB_Command_ReplyTogether(29, Send_Buffer, 0);
					else	
					{
						reader_msg.len = 0;
						 reader_msg.sta = MW_OK;
					}
						//Uart1_TX_STA(0, Send_Buffer, 0);
						//USB_Command_ReplyTogether(0, Send_Buffer, 0);
					MessageToSend = 1;
					break;				  
				
				case 0xc14b: 
					//status=MIF_Transfer(Rec_Buffer[6]);
					status=MIF_Transfer(pc_cmd.dat[0]);
					if(status != FM1702_OK)
					{
						reader_msg.len = 0;
						status1 = MW_TRANFER;//30;
						 reader_msg.sta = __REV16(status1);
					}
						//Uart1_TX_STA(30, Send_Buffer, 0);	   //调用传送失败
						//USB_Command_ReplyTogether(30, Send_Buffer, 0);
					else
					{
						reader_msg.len = 0;
						 reader_msg.sta = MW_OK;
					}	
						//Uart1_TX_STA(0, Send_Buffer, 0);
						//USB_Command_ReplyTogether(0, Send_Buffer, 0);
					MessageToSend = 1;
					break;
				//case 0xc134:	
				case 0xc15f:	 //效验密码	OK							  //OK
					//status=Load_key(&Rec_Buffer[8]);
					status=Load_key(&pc_cmd.dat[2]);
					if(status!=FM1702_OK)					   //调用loadkey失败
					{
						reader_msg.len = 0;
						status1 = MW_LOADKEY;//31;
						reader_msg.sta = __REV16(status1);
						MessageToSend = 1;
						break;
					}
						//Uart1_TX_STA(31, Send_Buffer, 0); 
						//USB_Command_ReplyTogether(31, Send_Buffer, 0);
					//status = Authentication(Rec_Buffer[7], Rec_Buffer[6]);
					status = Authentication(pc_cmd.dat[1], pc_cmd.dat[0]);	
					if(status!=FM1702_OK)
					{
					 	reader_msg.len = 0;
						status1 = MW_AUTH;//4;
						 reader_msg.sta = __REV16(status1);
					}
						//Uart1_TX_STA(4, Send_Buffer, 0);
						//USB_Command_ReplyTogether(4, Send_Buffer, 0);
					else
					{
						reader_msg.len = 0;
						 reader_msg.sta = MW_OK;
					}
						//Uart1_TX_STA(0, Send_Buffer, 0);
						//USB_Command_ReplyTogether(0, Send_Buffer, 0);
					MessageToSend = 1;
					break;
#endif
			case	0x0003:	  //bit 7对应绿灯，bit 6对应红灯   0关，1开
			case	0x3114:	  //bit 7对应绿灯，bit 6对应红灯   0关，1开
				if(pc_cmd.dat[0]&0xC0)
				{
					LED_ON();
				}
				else
				{
					LED_OFF();
				}
				reader_msg.len = 0;
				reader_msg.sta = MW_OK;
				MessageToSend = 1;
				//Uart1_TX_STA(status1, Send_Buffer, 0);
				//USB_Command_ReplyTogether(0, Send_Buffer, 0);
				break;
		    default:
				reader_msg.len = 0;	  //数据长度
				status = 0x0006;
				reader_msg.sta = __REV16(status);
				MessageToSend = 1;
			    break;
		}
		/*
		if((pc_cmd.cmd != 0x0003) && (pc_cmd.cmd != 0x3114))
		{	
			LedGreenOff();
			LedRedOn();
		}
		*/
		if((MessageToSend == 1 )  ) 
		{
			if(Usb_Uart1 == 0)
				USB_Command_ReplyTogether(&reader_msg.stx,reader_msg.len);
			else	//串口发送
			{
				Uart1_TX_STA(reader_msg.sta, reader_msg.dat, reader_msg.len);
				if(ChBdFlag)
				{
					ChBdFlag = 0;
					Delay_Ms(5);
					Usart1_Chbaud(BaudRd);		
				}
			}
		}
		
	}  
}
/**********************************************************************/
//stats: 是输出状态
//*pt  ：输出数据指指针
//len  : 输出数据的长度,
/**********************************************************************/
void Uart1_TX_STA(s16	stats, u8 *pt,u16 len)
	{	
		u8 temp,Bcc;
		u16	i,j, lenall = 0;
	
		for(j = 0; j < 300; j ++)		   //清
			Rec_Buffer[j] = 0;
		SerialPutChar(0x02);		   //发头
		lenall = len + 3;
		SerialPutChar(lenall >> 8);	   //发两字节的长度信息
		SerialPutChar(lenall);
		SerialPutChar(stats >> 8);	   //发送两字节的状态
		Bcc = (stats >> 8);
		SerialPutChar(stats);
		Bcc ^= stats;
		SerialPutChar(0);				//一字节的delay，此为0
		for(i = 0;i < len;i++)		  //后面的数据发送
		{
			temp = *pt++;
			Bcc ^= temp;
			SerialPutChar(temp);
		}
		SerialPutChar(Bcc);
		SerialPutChar(0x03);
	}
/******************* (C) COPYRIGHT 2010 Mingtech ***************END OF FILE****/

