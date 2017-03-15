#include "rc632.h"

//u8	wrtsta,rdsta;

void Rc632_Spi_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);

	/*****************************RC632********************************/
     
	GPIO_InitStructure.GPIO_Pin = RC632_PD_Pin ;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(RC632_GP_PD, &GPIO_InitStructure);

	GPIO_SetBits(RC632_GP_PD, RC632_PD_Pin);	//PD = 1
	
	GPIO_InitStructure.GPIO_Pin = RC632_SCK_Pin ;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//GPIO_Mode_IN_FLOATING;//;    //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 最高输出速率50Hz,可变不可少
	GPIO_Init(RC632_GP_SCK, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  RC632_MOSI_Pin ;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_Init(RC632_GP_SI, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  RC632_CS_Pin ;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_Init(RC632_GP_CS, &GPIO_InitStructure);
	
	//MISO
	GPIO_InitStructure.GPIO_Pin = RC632_MISO_Pin;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//GPIO_Mode_IN_FLOATING;//;     
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;  
	GPIO_Init(RC632_GP_SO, &GPIO_InitStructure);

	GPIO_ResetBits(RC632_GP_SO,RC632_MISO_Pin);

	Init_FM1702();
}

void Rc632_Exti_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	
	//中断输出 PA0	
	//NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
	
	/* Configure one bit for preemption priority */
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	/* Configure Key Button GPIO Pin as input floating (Key Button EXTI Line) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_0);

	/* Enable the EXTI0 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn ;//EXTI0_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Connect Key Button EXTI Line to Key Button GPIO Pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);
	
	/* Configure Key Button EXTI Line to generate an interrupt on falling edge */  
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//EXTI_Trigger_Rising_Falling;		//EXTI_Trigger_Rising;			//EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	Delay_Ms(20);							//8007 delay 105p ，复位上电时间在6.5ms左右，此处加上电延时防止中断误动作
	EXTI_Init(&EXTI_InitStructure);
}

void Delay(vu32 nCount)		//精确延时
{
  for(; nCount != 0; nCount--);
}

//时钟平时为低,其他平时为高
/******************************/
//字节读
//SPI模式0,注意uchar
/******************************/	 
unsigned char SPI_STM32ReadByte(void)
{
	u8 i,rByte=0;
	
	 
	for(i=0;i<8;i++)
	{
		SPI_SCK_H();
		rByte <<= 1;
		rByte |= SPI_SO();
		SPI_SCK_L();
	}
	/*
	SPI_SCK_H();
	rByte <<= 1;
	rByte |= SPI_SO();
	SPI_SCK_L();
	*/
	return rByte;
	
}

/******************************/
//字节写
//时钟上来就是低的,先处理数据,再拉高时钟发出,
 //再拉低时钟,保证正常传输和下次时钟还是低的.
/******************************/

void SPI_STM32WriteByte(u8 wByte)		   
{											  
	u8 i;
	
	if(wByte&0x80)	 
		SPI_SI_H();    
	else		
		SPI_SI_L();    
	SPI_SCK_H(); 	i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=i; 
	SPI_SCK_L();
	if(wByte&0x40)	 
		SPI_SI_H();    
	else		
		SPI_SI_L();    
	SPI_SCK_H();	i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;
	SPI_SCK_L();
	if(wByte&0x20)	 
		SPI_SI_H();   
	else		
		SPI_SI_L();    
	SPI_SCK_H();    i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;
	SPI_SCK_L();
	if(wByte&0x10)	 
		SPI_SI_H();    
	else		
		SPI_SI_L();    
	SPI_SCK_H();    i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;
	SPI_SCK_L();
	if(wByte&0x08)	 
		SPI_SI_H();    
	else		
		SPI_SI_L();   
	SPI_SCK_H();    i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;
	SPI_SCK_L();
	if(wByte&0x04)	 
		SPI_SI_H();    
	else		
		SPI_SI_L();   
	SPI_SCK_H();    i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;
	SPI_SCK_L();
	if(wByte&0x02)	 
		SPI_SI_H();   
	else		
		SPI_SI_L();    
	SPI_SCK_H();    i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;
	SPI_SCK_L();
	if(wByte&0x01)	 
		SPI_SI_H();    
	else		
		SPI_SI_L();   
	SPI_SCK_H();    i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;i=0;
	SPI_SCK_L();	
}

unsigned char read_reg(unsigned char SpiAdd)
{
	unsigned char RecData;
	SpiAdd = SpiAdd<<1;
	SpiAdd = SpiAdd | 0x80; 
	SPI_CS_L();	
	SPI_STM32WriteByte(SpiAdd);
	RecData = SPI_STM32ReadByte();
	SPI_CS_H();		
	return(RecData);
} 

void write_reg(unsigned char SpiAdd,unsigned char  Dat)
{
	SpiAdd= SpiAdd<<1;
	SpiAdd= SpiAdd &0x7f;
	SPI_CS_L(); 
	SPI_STM32WriteByte(SpiAdd);
	SPI_STM32WriteByte(Dat);
	SPI_CS_H();
}
