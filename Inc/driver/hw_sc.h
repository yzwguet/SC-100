
/********************************************************************
*******************���ܿ�Ӳ���ӿ���س�ʼ��**************************
****************************20111216 LSQ*****************************
********************************************************************/

#ifndef __HW_SC_H
#define __HW_SC_H

//����1�����ܿ��ӿ�
#define SC1_RESET                 GPIO_Pin_7          //���ĵ�һ��
#define GPIO1_RESET               GPIOC
#define SC1_USART                 USART1
#define SC1_USART_TxPin           GPIO_Pin_9
#define SC1_USART_ClkPin          GPIO_Pin_8
//����2�����ܿ��ӿ�
#define SC2_RESET                 GPIO_Pin_3          //���ĵ�һ��
#define GPIO2_RESET               GPIOA
#define SC2_USART                 USART2
#define SC2_USART_TxPin           GPIO_Pin_2
#define SC2_USART_ClkPin          GPIO_Pin_4
  
//����3�����ܿ��ӿ�
#define SC3_RESET                 GPIO_Pin_13              
#define GPIO3_RESET               GPIOB
#define SC3_USART                 USART3
#define SC3_USART_TxPin           GPIO_Pin_10
#define SC3_USART_ClkPin          GPIO_Pin_12

#define SC_3_5V                   GPIO_Pin_7              
#define GPIO3_3_5V                GPIOA
#define SC_CMDVCC                 GPIO_Pin_14              
#define GPIO3_SC_CMDVCC           GPIOB


#endif /* __SC_H */
