/**
  ******************************************************************************
  * @file    cmd_process.h
  * @author  zhenhua mo
  * @version V1.0
  * @date    2011/07/09
  * @brief   This file is describing the command process.
  ******************************************************************************
  */

#ifndef __CMD_PROCESS_H
#define __CMD_PROCESS_H

#include "hw_config.h"

/* 命令码 */
#define PC_DAT_SIZE          300
#define RD_DAT_SIZE          300

#define CMD_STATUS           0xb9 /* 读状态 */
#define CMD_GetVer			 0x01 /* 读设备版本号 */
#define CMD_RDVER			 0xb0 /* 读设备版本号 */
#define CMD_READSNR          0xb7 /* 读EEPROM中的序列号 */
#define CMD_WRITESNR		 0xb6 /* 写序列号 */

#define CMD_BEEPCTRL         0xb4 /* 控制蜂鸣器 */ 
#define CMD_VOICECTRL        0xdd /* 控制语音 */
#define CMD_DISPLAYSHOW      0xde /* 显示字符 */
#define CMD_DISPLAYCLR       0xdf /* 清屏 */

#define CMD_SET_KEYBOARD     0x01 /* 设置密钥长度和超时时间 */
#define CMD_SET_CODEKEY      0x03 /* 设置密钥 */
#define CMD_SET_KEYBOARD2    0x04 /* 设置传输模式*/
#define CMD_GETINPUT         0xcc /* 获取键盘输入*/

#define CMD_EXTKEYINPUT      0xca /*提示请输入密码并获取密码*/
//#define CMD_EXTKEYINPUT2     0xce /*提示请再次输入密码并获取密码*/

#define CMD_SMARTCARDRESET   0x20 /* CPU/SAM卡复位 */
#define CMD_SMARTCARDAPDU    0x21 /* CPU/SAM卡APDU */
#if 0
	#define CMD_SET_MAGCARD      0x02 /* 设置磁卡超时时间 */
	#define CMD_READ_MAGCARD     0xee /* 读磁卡 */
	
	#define CMD_SWRITE           0x60/*（写数据）*/
	#define CMD_SREAD            0x61/*（读数据）*/
	#define CMD_PWRITE           0x62/*（写数据页）*/
	#define CMD_ERAL             0x78/*（整张卡擦除）*/
	#define CMD_WWC              0x70/*（带进位擦除）*/
	#define CMD_SER              0x76/*（擦除数据）*/
	#define CMD_WRAL             0x79/*（整张卡擦、写入）*/
	#define CMD_WPWR             0x6d/*（带保护位写）*/
	#define CMD_WPRD             0x6e/*（带保护位读）*/
	#define CMD_WRPB             0x6f/*（固化数据）*/
	#define CMD_PRD              0x74/*（读保护位）*/
	#define CMD_PWR              0x75/*（写保护位）*/
	#define CMD_CSC              0x64/*（校验密码）*/
	#define CMD_RSC              0x65/*（读密码）*/
	#define CMD_WSC              0x66/*（更改密码）*/
	#define CMD_REC              0x67/*（读密码错误记数器）*/
	#define CMD_CESC             0x68/*（校验擦除密码）*/
	#define CMD_RESC             0x69/*（读擦除密码）*/
	#define CMD_WESC             0x6a/*（更改擦除密码）*/
	#define CMD_REEC             0x6b/*（读擦除密码错误计数器）*/
	#define CMD_RECC             0x6c/*（读擦除计数器）*/
	
	#define CMD_PN532_TRN        0xf0/* PN532通道命令 */
	
	#define CMD_POWERON          0xb2/* 卡座上电 */
	#define CMD_POWEROFF         0xb3/* 卡座下电 */
	
	#define CMD_SELE_CARD		 0xba/* 选择卡类型 */
#endif
/* 错误码 */
#define OP_OK                0 //(正确返回)  	
#define ERR_READ  	         0x80 //(读错误)
#define ERR_WRITE         	 0X81 //(写错误)
#define ERR_COMAND    	     0X82 //(命令错误)
#define ERR_PASS	         0X83 //(密码错误)
#define ERR_NOCARD	         0X86 //(无卡错误)
#define ERR_BCC	             0X8A //(校验和错误)
#define ERR_CARDTYPE	     0X8B //(卡型错误)
#define ERR_PULLCARD	     0X8C //(拔卡错误)
#define ERR_GENERAL	         0X8D //（通用错误）
#define ERR_HEAD	         0X8E //(命令头错误)
#define ERR_LEN	             0X8F //(数据长度错误)
#define ERR_COM	             0X95 //(串口被占用)
#define ERR_CHECKSUM         0XCA //校验和错误（CPU）
#define ERR_LENGTH	         0XCB //长度错误（CPU）
#define ERR_OP_CODE	         0XCD //操作码错误（CPU）


#define ERR_CHECK_BIT	     0XCE //校验位错误（CPU）
#define ERR_OVER_TIME	     0XCF //超时错误（CPU）
#define ERR_CONTARALESS_APP	 0xA3 //非接触卡应用级错误
#define ERR_RESPONE_FORM	 0xA4 //响应格式错误
#define ERR_DATA_FORM	     0xA5 //数据格式错误
#define ERR_DATA_OVER	     0xA6 //数据溢出
#define ERR_NO_RESPONE	     0xA7 //无响应
#define ERR_MAGCARD_DATA	 0xA8 //磁条卡数据错误

/******************* 接触卡操作函数 ******************/
#define		CPU_STA		0x01		//大卡座状态位
#define		SAM1_STA	0x02		//SAM1状态位
#define		SAM2_STA	0x04		//SAM2
#define		SAM3_STA	0x08		//SAM3
/*
__packed typedef struct _CMD_DAT
{
	u8  stx;
	u8  cmd;
	u16 len;
	u8  card_type;
	u16 sector_addr;
	u16 offset_addr;
	u8  dat[PC_DAT_SIZE];
	
}CMD_DAT;
*/
__packed typedef struct _CMD_DAT
{
	u8  stx;
	u16 len;
	u16 cmd; 	
	u8  unused;
	u8  dat[PC_DAT_SIZE];
	u8  bcc;
	u8  etx;	
}CMD_DAT;
/*
__packed typedef struct _MSG_DAT
{    
	u8  stx;
	u8  sta;
	u16 len;
	u8  dat[RD_DAT_SIZE];
	u16 bSizeToSend;
}MSG_DAT;

__packed typedef struct _MSG_DAT
{    
	u8  stx;
	u16 len;
	u16  sta;
	u8  dat[RD_DAT_SIZE];
	u16 bSizeToSend;
}MSG_DAT;
*/
__packed typedef struct _MSG_DAT
{
	u8  stx;
	u16 len;
	u16 sta; 	
	u8  unused;
	u8  dat[RD_DAT_SIZE];
	u8  bcc;
	u8  etx;	
}MSG_DAT;
__packed typedef struct _TIME_CNT
{
    u8  flag;
	u32 cnt;
}TIME_CNT;


__packed typedef struct
{
     u8 trans_mode;
	 u8 delay_sec;
	 u8 num;
	 u8 psd_val[18];
	 u8 code_key[8];
	 TIME_CNT over_time;
}GET_PSD;


void PC_CommandDispatcher(void);
#endif /* __CMD_PROCESS_H */

