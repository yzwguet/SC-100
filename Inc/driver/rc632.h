
#ifndef	__RC632_H
#define	__RC632_H

#include "fm1702.h"

#include "hw_config.h"
#include "platform_config.h"


/******************RC632 IO初始化***********************/
#define		RC632_SCK_Pin		GPIO_Pin_9
#define		RC632_GP_SCK		GPIOB

#define		RC632_MISO_Pin		GPIO_Pin_8
#define		RC632_GP_SO			GPIOA

#define		RC632_MOSI_Pin		GPIO_Pin_8
#define		RC632_GP_SI			GPIOB

#define		RC632_CS_Pin		GPIO_Pin_7
#define		RC632_GP_CS			GPIOB

#define		RC632_PD_Pin		GPIO_Pin_4
#define		RC632_GP_PD			GPIOB
//#define		RC632_INT_Pin		GPIO_Pin_0

#define 	SPI_SCK_L()  		GPIO_ResetBits(RC632_GP_SCK, RC632_SCK_Pin)   
#define 	SPI_SCK_H()  		GPIO_SetBits(RC632_GP_SCK, RC632_SCK_Pin)

#define 	SPI_SO()    		GPIO_ReadInputDataBit(RC632_GP_SO,RC632_MISO_Pin)	
#define		SPI_SO_H()			GPIO_SetBits(RC632_GP_SO, RC632_MISO_Pin)

#define		SPI_SI_L()  		GPIO_ResetBits(RC632_GP_SI, RC632_MOSI_Pin)   
#define 	SPI_SI_H()  		GPIO_SetBits(RC632_GP_SI, RC632_MOSI_Pin)

#define 	SPI_CS_L()   		GPIO_ResetBits(RC632_GP_CS, RC632_CS_Pin)   
#define 	SPI_CS_H()   		GPIO_SetBits(RC632_GP_CS, RC632_CS_Pin)

#define		PD_H()				GPIO_SetBits(RC632_GP_PD, RC632_PD_Pin)
#define		PD_L()				GPIO_ResetBits(RC632_GP_PD, RC632_PD_Pin)

/*********************RC632相关变量与函数***************/
/* 外部调用 */
extern	u8	UIDCasc[15];
extern	u8	SeriNum;




void Rc632_Spi_Config(void);
void Rc632_Exti_Config(void);

//extern	s16		Apdu(u8 *buf,u16 len);
extern	u16		RequCpu(u16	MyDelayTime, u8	*pt, u16	*Atrlen);
extern	s16		RfApduA(u8 *req_buf, u16 req_len, u8 *resp_buf);

extern	void	Fm1702Reset(u16 ms);
extern	void	Init_FM1702(void);
extern	u8		RequestA(u8	mode);
extern	u8		AntiColl(void);
extern	u8		Select_Card(void);
extern	u8		Cpu_Rst(void);
extern	char	Mf500PiccCascAnticoll (void);
extern	u8		Load_key(u8 *uncoded_keys);

extern	u8		Authentication(u8 SecNR,u8 mode);
extern	u8		MIF_READ(u8 *buff,u8 Block_Adr);
extern	u8		MIF_Write(u8 *buff, u8 Block_Adr);
extern	u8		MIF_Increment(u8 *buff, u8 Block_Adr);
extern	u8		MIF_Decrement(u8 *buff, u8 Block_Adr);
extern	u8		MIF_Restore(u8 Block_Adr);
extern	u8		MIF_Transfer(u8 Block_Adr);
extern	u8		Write_E2(u8 lsb, u8 msb, u8 count, u8	*buff);
extern  u8		Read_E2(u8 lsb, u8 msb, u8 count, u8	*buff);

extern	u8		RequestB(void);
extern	u8		Attrib(void);
extern	u8		MIF_Halt(void);

extern	void	SoftCloRf(void);
extern	void	SoftOpenRf(void);
extern	u8		Request(u8	mode);



extern  void	write_reg(unsigned char SpiAdd,unsigned char  Dat);
extern	u8		read_reg(unsigned char SpiAdd);
extern  u16 GetRFStatus(void);
#endif
