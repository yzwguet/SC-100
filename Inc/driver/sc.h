/**
  ******************************************************************************
  * @file    SmartCard.h
  * @author  zhenhua mo
  * @version V1.0
  * @date    2009/12/29
  * @brief   This file is describing the hardware on board.                            
  ******************************************************************************
  */
#ifndef __SC_H
#define __SC_H

#include "type.h"
#include "stm32f10x.h"
#define __TIM4_DIER               0x0001               
#define TIMX_CR1_CEN         ((unsigned short)0x0001)

#define Card_OK OK1 
#define Card_Parameter          Parameter_ERR
#define Card_NoPower            Off_Power 
#define Card_OverTime           Over_Time 
#define Card_CRCERR             CRC_ERR  
#define Card_PPSERR             PPS_ERR  
#define Card_PPSNOSupport       PPS_NOT_Support 
#define Card_NotSupport         NOT_Support


#define Apdu_comm_0 0//CLA+INS+P1+P2
#define Apdu_comm_1 1//CLA+INS+P1+P2   +LE
#define Apdu_comm_2 2//CLA+INS+P1+P2   +LC  +[DATA]
#define Apdu_comm_3 3//CLA+INS+P1+P2   +LC  +[DATA]  +LE
#define T1_NAN      0
#define T1_PCB      0


 
/*******************************************************/
//------
#define IS_OVER     0
#define NOT_OVER    1


  
/*uart1 clock*/
#define SAM_UART_CLOCK     4000000
#define SAM_USB_UART_CLOCK 3600000
/*card  mode of operate-------*/
#define CARD_MODE_Special  0x00 //专用模式
#define CARD_MODE_Consult  0x01 //协商模式

#define DIRECT             0x3B  /* Direct bit convention */
#define INDIRECT           0x3F  /* Indirect bit convention */
#define SETUP_LENGTH       20
#define HIST_LENGTH        20
#define D_LENGTH           15

 
//#define SC_GET_A2R         0x00


#define default_data       0x00
#define other_data         0x01  


/* add for sc.c*/

#define OFF                1

#define SC_Voltage_18V     2
#define SC_Voltage_3V      1
#define SC_Voltage_5V      0

#define T0_PROTOCOL        0x00  /* T0 protocol */
#define T1_PROTOCOL        0x01  /* T0 protocol */


#define CRC_ER             0
 
#define OK1                 0
#define Parameter_ERR      1
#define Off_Power          0x11
#define Over_Time          0x12
#define CRC_ERR            0x13
#define PPS_ERR            0x14
#define PPS_NOT_Support    0x15
#define NOT_Support        0x16
#define Runtime_Error      0x21
#define Function_Err       0x31  

     
/*-----------------------------*/
typedef enum
{
  SC_POWER_ON = 0x00,
  SC_RESET_LOW = 0x01,
  SC_RESET_HIGH = 0x02,
  SC_ACTIVE = 0x03,	 
  SC_ACTIVE_ON_T0 = 0x04,
  SC_POWER_OFF = 0x05
} SC_State;
/* CARD Parameter structure -----------------------------------------------*/
typedef struct 
{
  u8 CARD_DIRECT;           /* 0x3b:DIRECT,0x3f: INDIRECT */
  u8 CARD_PROTOCOL;         /* 0:T0;1:T1;other: err*/
  u8 CARD_Voltage;          /*0:5V; 1:3V*/
  u32 CARD_F;               /*372:default*/
  u32 CARD_D;               /*1:default*/
  u8 CARD_Ipp;
  u8 CARD_PI;
  u8 CARD_N;
  u8 CARD_TRANSFER_STATION;
  u8 CARD_TYPE_STATION;//0合适，1无卡，2型号不符
  u8 CARD_W;
  //T1
  u8 CARD_CWI;
  u8 CARD_BWI;
  u8 CARD_IFSC;
  u8 CARD_IFSD;
} CARD_Parameter;

/* ATR structure - Answer To Reset -------------------------------------------*/
typedef struct
{
  u8 TS;               /* Bit Convention */
  u8 T0;               /* High nibble = Number of setup byte; low nibble = Number of historical byte */
  u8 T[SETUP_LENGTH];  /* Setup array */
  u8 H[HIST_LENGTH];   /* Historical array */
  u8 Tlength;          /* Setup array dimension */
  u8 Hlength;          /* Historical array dimension */
  u8 TCK;
  u8 D[D_LENGTH];
  u8 Dlength;
} SC_ATR;


extern __IO uint32_t CardInserted;



void SC_Test(void);

#endif /* __SC_H */
