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

/* ������ */
#define PC_DAT_SIZE          300
#define RD_DAT_SIZE          300

#define CMD_STATUS           0xb9 /* ��״̬ */
#define CMD_GetVer			 0x01 /* ���豸�汾�� */
#define CMD_RDVER			 0xb0 /* ���豸�汾�� */
#define CMD_READSNR          0xb7 /* ��EEPROM�е����к� */
#define CMD_WRITESNR		 0xb6 /* д���к� */

#define CMD_BEEPCTRL         0xb4 /* ���Ʒ����� */ 
#define CMD_VOICECTRL        0xdd /* �������� */
#define CMD_DISPLAYSHOW      0xde /* ��ʾ�ַ� */
#define CMD_DISPLAYCLR       0xdf /* ���� */

#define CMD_SET_KEYBOARD     0x01 /* ������Կ���Ⱥͳ�ʱʱ�� */
#define CMD_SET_CODEKEY      0x03 /* ������Կ */
#define CMD_SET_KEYBOARD2    0x04 /* ���ô���ģʽ*/
#define CMD_GETINPUT         0xcc /* ��ȡ��������*/

#define CMD_EXTKEYINPUT      0xca /*��ʾ���������벢��ȡ����*/
//#define CMD_EXTKEYINPUT2     0xce /*��ʾ���ٴ��������벢��ȡ����*/

#define CMD_SMARTCARDRESET   0x20 /* CPU/SAM����λ */
#define CMD_SMARTCARDAPDU    0x21 /* CPU/SAM��APDU */
#if 0
	#define CMD_SET_MAGCARD      0x02 /* ���ôſ���ʱʱ�� */
	#define CMD_READ_MAGCARD     0xee /* ���ſ� */
	
	#define CMD_SWRITE           0x60/*��д���ݣ�*/
	#define CMD_SREAD            0x61/*�������ݣ�*/
	#define CMD_PWRITE           0x62/*��д����ҳ��*/
	#define CMD_ERAL             0x78/*�����ſ�������*/
	#define CMD_WWC              0x70/*������λ������*/
	#define CMD_SER              0x76/*���������ݣ�*/
	#define CMD_WRAL             0x79/*�����ſ�����д�룩*/
	#define CMD_WPWR             0x6d/*��������λд��*/
	#define CMD_WPRD             0x6e/*��������λ����*/
	#define CMD_WRPB             0x6f/*���̻����ݣ�*/
	#define CMD_PRD              0x74/*��������λ��*/
	#define CMD_PWR              0x75/*��д����λ��*/
	#define CMD_CSC              0x64/*��У�����룩*/
	#define CMD_RSC              0x65/*�������룩*/
	#define CMD_WSC              0x66/*���������룩*/
	#define CMD_REC              0x67/*������������������*/
	#define CMD_CESC             0x68/*��У��������룩*/
	#define CMD_RESC             0x69/*�����������룩*/
	#define CMD_WESC             0x6a/*�����Ĳ������룩*/
	#define CMD_REEC             0x6b/*����������������������*/
	#define CMD_RECC             0x6c/*����������������*/
	
	#define CMD_PN532_TRN        0xf0/* PN532ͨ������ */
	
	#define CMD_POWERON          0xb2/* �����ϵ� */
	#define CMD_POWEROFF         0xb3/* �����µ� */
	
	#define CMD_SELE_CARD		 0xba/* ѡ������ */
#endif
/* ������ */
#define OP_OK                0 //(��ȷ����)  	
#define ERR_READ  	         0x80 //(������)
#define ERR_WRITE         	 0X81 //(д����)
#define ERR_COMAND    	     0X82 //(�������)
#define ERR_PASS	         0X83 //(�������)
#define ERR_NOCARD	         0X86 //(�޿�����)
#define ERR_BCC	             0X8A //(У��ʹ���)
#define ERR_CARDTYPE	     0X8B //(���ʹ���)
#define ERR_PULLCARD	     0X8C //(�ο�����)
#define ERR_GENERAL	         0X8D //��ͨ�ô���
#define ERR_HEAD	         0X8E //(����ͷ����)
#define ERR_LEN	             0X8F //(���ݳ��ȴ���)
#define ERR_COM	             0X95 //(���ڱ�ռ��)
#define ERR_CHECKSUM         0XCA //У��ʹ���CPU��
#define ERR_LENGTH	         0XCB //���ȴ���CPU��
#define ERR_OP_CODE	         0XCD //���������CPU��


#define ERR_CHECK_BIT	     0XCE //У��λ����CPU��
#define ERR_OVER_TIME	     0XCF //��ʱ����CPU��
#define ERR_CONTARALESS_APP	 0xA3 //�ǽӴ���Ӧ�ü�����
#define ERR_RESPONE_FORM	 0xA4 //��Ӧ��ʽ����
#define ERR_DATA_FORM	     0xA5 //���ݸ�ʽ����
#define ERR_DATA_OVER	     0xA6 //�������
#define ERR_NO_RESPONE	     0xA7 //����Ӧ
#define ERR_MAGCARD_DATA	 0xA8 //���������ݴ���

/******************* �Ӵ����������� ******************/
#define		CPU_STA		0x01		//����״̬λ
#define		SAM1_STA	0x02		//SAM1״̬λ
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

