/**************************
 * ����ģ�鿪��ר��ͷ�ļ� * 
 **************************/
#ifndef __TYPE_H
#define __TYPE_H

//#include <stm32f10x_lib.h>
#include "stm32f10x.h"

/* ���ܿ��� */
//�˴�����usart2
#define		My_Sys_Tick			//����ϵͳ��������ʱ��ʱ
//#define		My_Watch_Dog		//���忴�Ź�
//#define		My_RTC				//����RTC
#define		My_Usb				//USB
#define		My_Usart1			//����ʹ���˴���1,����س�ʼ��	   ����1������
//#define		My_Usart2			//����ʹ���˴���2,����س�ʼ��
//#define		My_Usart3			//����ʹ���˴���3,����س�ʼ��
//#define		My_Tim1		 		//����ʹ���˶�ʱ��1,����س�ʼ��
#define		My_Tim2		 		//����ʹ���˶�ʱ��2,����س�ʼ��
#define		My_Tim3		 		//����ʹ���˶�ʱ��3,����س�ʼ��
//#define		My_Tim4		 		//����ʹ���˶�ʱ��3,����س�ʼ��
//#define		My_Tim4		 		//����ʹ���˶�ʱ��4,����س�ʼ��
//#define		My_Tim5		 		//����ʹ���˶�ʱ��5,����س�ʼ��
#define		My_Tda8007	 		//����ʹ����TDA8007,����س�ʼ��
#define		My_RC632		 	//����ʹ����RC632,����س�ʼ��
//#define		My_FlashE2		 	//����Flash��E2�õĳ�ʼ��
#define		My_Exti_8007	 		//����TDA8007�ⲿ�ж�3				
//#define		My_Exti_632	 		//����RC632�ⲿ�ж�3				 
#define		My_CloCard			//�ϵ�����п�������ʼ������IO

#define		My_Debug			//�ϵ�����п�������ʼ������IO
//#define		My_ABC				//ũ��Э�飬������c030֮���Լ��Ķ����Э��

#define		My_SmartCard		//����ʹ����SAM3�Ľӿ�USART3
#define		My_SmartCard2		//����ʹ����SAM2�Ľӿ�USART2

/* ������ */
/*
#define		CMD_SUCC		0
#define		CMDERR_CMD		0x0001		//���֧��
#define		CMDERR_LEN		0x0002		//���ȴ���
#define		CMDERR_PARAM	0x0003		//��������
#define		CMDERR_HEAD		0x0004		//����ͷ����
#define		CMDERR_CRC		0x0005		//У�����

#define		SCERR_NOCARD	0xC001		//�����޿�
#define		SCERR_APDU		0xC002		//APDUʧ��
*/
#endif

