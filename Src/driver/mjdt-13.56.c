//#include "type.h"			 
//#include "fm1702.h"
//#include "stm32f10x_it.h"
#include "string.h"
//#include "stm32f10x_iwdg.h"
#include "rc632.h"
uchar   mifare_SAK;   //write by yzwguet 2014/4/30
u8	volatile	Head = 0x0a;
u8	Heada=0x02;
u8	Headb=0x0b,KeyType;
u8   tagtype[2];
//extern	u8	tss[2];	
//extern	u8	CardRstSta[4];
u8	RfBuffer[300],buffe[50];
//u8  rfcard_workingBuffer[270];
u8	UID[10];
u8	UIDCasc[15];
u8	Rst_Data[35];
u16  tasendbuff[256];
u8	T2IR = 0;//¶¨Ê±Ã»ÓÐµ½,Ã»·¢ÉúÊ±¼äµ½ÖÐ¶Ï
//u8	RFCardAOrB = 0;	//b8,b7..b5(B¿¨),B4,B3...B1(A¿¨)
u16	Tim4Num;
u16	Tim4PlanTime;
u8	requidlen = 0;
volatile	u16	Total_Rec_Lenght;

void start_timeout(unsigned int _100ms);
void stop_timeout(void);
char M500PcdRfReset(unsigned char ms);
void M500PcdSetTmo(unsigned char tmoLength);
u8	CascNum = 0;
u8	SeriNum = 0;
char SetBitMask(unsigned char reg,unsigned char mask); //
char ClearBitMask(unsigned char reg,unsigned char mask); //
char M500PcdReset(void); 
u8 Select_Card(void); 
u8 RCAntiColl(u8 Casc_Cmd);
u8 RCSelect_Card(u8	Casc_Cmd);
char Mf500PiccCascAnticoll (void);
void	RfReset(void);
void	resetrctim(void);
void	reqprotim(void);
/**************************************
CPU¿¨¸´Î»µÃµ½ÑùÏàÐÅÏ¢µÄ¶¨Òå
**************************************/
//TAÏà¹ØÉèÖÃ
u8	rftl = 0;			//TL ³¤¶È×Ö½Ú
u8	rft0 = 0; 			//¸ñÊ½×Ö·û,°üº¬ÊÇ·ñÓÐta1..tc1;fsci	 piccÄÜ½ÓÊÕµÄ×î´óÖ¡³¤¶È
	u8	rffsci = 0;
u8	rfta1 = 0;			//ta1  ÖÐ¼ä»áÓÐrfds,rfdr ,b8Ö¸Ê¾Ë«ÏòÊý¾Ý´«ÊäÊÇ·ñÏàÍ¬,0Îª²»Í¬,b4Îª0 PCD¿ÉÒÔÀûÓÃPPSÑ¡Ôñ²»Í¬ÓÚ106µÄkbps
	u8	rfds = 0;		//·¢ËÍÒò×Ó
	u8	rfdr = 0;		//½ÓÊÕÒò×Ó
u8	rftb1 = 0;			//ta1  ÖÐ¼ä»áÓÐrffwi,rfsfgi
	u8	rffwi = 0;		//Ö¡µÈ´ýÊ±¼äÕûÊý,¾ö¶¨fwt
	u8	rfsfgi = 0;		//Æô¶¯Ö¡±£»¤Ê±¼äÕûÊý,¾ö¶¨sfgt,ÊÇatsºóµÄÏÂÒ»Ö¡Ç°ËùÐèÒªµÄÌØÊâ±£»¤Ê±¼ä
u8	rftc1 = 0;			//ta1  ÊÇ·ñÖ§³Önad,cid
	u8	rfnad = 0;		//0 Ê±²»Ö§³Ö£¬1Ê±Ö§³Ö
	u8	rfcid = 0;		//0 Ê±²»Ö§³Ö£¬1Ê±Ö§³Ö
u16  rffsc[16]={16, 24, 32, 40, 48, 64, 96, 128, 256, 256, 256, 256, 256, 256, 256, 256};	   //piccµÄÖ¡³¤¶È
u16	fsclen = 0;			//atsÖÐpicc¸æËßµÄËüÄÜ½ÓÊÕµÄ×î´óÖ¡µÄ³¤¶È
u32	rfsfgt = 0;			//Æô¶¯Ö¡±£»¤Ê±¼ä  atsºóµ½×¼±¸½ÓÊÕÏÂÒ»Ö¡Ç°µÄÌØÊâ±£»¤Ê±¼ä  0-14,15rfu tb1µÄµÍËÄÎ»
u32	rffwt  = 0;			//Ö¡µÈ´ýÊ±¼ä	pcd·¢ËÍµÄÖ¡Óëpicc»ØÓ¦µÄÖ¡¼äµÄ×î´óÑÓ³ÙÊ±¼ä  0-14,15rfu,tb1¸ßËÄÎ»

u8  ta_workingBuffer[270];       // Working buffer for card slot 1
u8	IPU_TANO1 = 0;
u8  Rf_IFD_Seq;  // Contains next Terminal I-block sequence number
u8  Rf_ICC_Seq;  // Contains next Card I-block sequence number
#define T1_BLOCK_RETRY    3    	/* Number of retries */									//T1Ä£Ê½ÏÂÖØÊÔµÄ´ÎÊý
#define TACODEIBLOCK(ns,mb)   (unsigned char)(0x02 | (ns)|((mb) << 4))		   //·Ö×éºÅÎªb1,Á´ºÅÎªb5
#define TACODERBLOCK(ns,mb)   (unsigned char)(0xa2 | (ns)|((mb) << 4))		   //·Ö×éºÅÎªb1,Á´ºÅÎªb5
//#define TANONIBLOCK   0x80    	/* Non I Block indicator */
//#define TASBLOCK      0x40    	/* S Block indicator     */
//#define TAMOREBIT     0x01    	/* More bit indicator    */
#define TAISIBLOCK(pcb)    ((pcb & 0x02) && (!(pcb & 0x20))&& (!(pcb & 0x40))&& (!(pcb & 0x80)))  //b8=0,b7=0,b6=0,b2=1.  
#define TAISRBLOCK(pcb)    ((pcb & 0x80) && (!(pcb & 0x40))&& (pcb & 0x20) &&(!(pcb&0x04))&& (pcb & 0x02))  //b8=1,b7=0,b6=1,b3=0,b2=1.  
u8  ta_sblock_sent=0;    	// global status flag used to maintain ta protocol state.
#define TANR(pcb)    	((pcb) & 0x01)   /* Retrieves R_Block Sequence number from pcb*/
#define TANS(pcb)    	((pcb) & 0x01)   /* Retrieves I_Block Sequence number from pcb*/
#define TAMORE(pcb)     (((pcb) & 0x10) >> 4)  /* Retrieves More data bit info from pcb*/
#define	SREQUEST_Deselt	0xc2
#define	SREQUEST_Wtx	0xf2

u8	BlockErr = 0;     //1±íÊ¾´«´íÁË
u8	tarecpcb;
u16	tareclen;
u8	exitapdu = 0;    //1±íÊ¾ÒªÌø³ö
//u8  tb_workingBuffer[256];       // Working buffer for card slot 2
u8	errflagcrcorpar = 0; //ta414.0/1  ta415.0/1 ta416.0/1
//u8	lsbugchuli = 0;	//tb004.0 ,tb202.8, tb305.2Ö®¼ä¹ØÓÚattribµÄÒ»¸öÎ»µÈ´ýµÄÊ±¼äÎÊÌâ´¦Àí.

//#define ASSIGN_CURRENT_CARDSLOT_BUFFER() tb_workingBuffer;
//u8  t1_sblock_sent=0;    	// global status flag used to maintain t1 protocol state.
//FIFO²Ù×÷
u8 Judge_Req(u8 *buff)
{
	u8 temp1,temp2;
	
	temp1 = *buff;
	temp2 = *(buff + 1);
	//if(((temp1 == 0x03) || (temp1 == 0x04) || (temp1 == 0x05) || (temp1 == 0x53)) && (temp2 == 0x00))
	//change by hbp  S70 temp1 = 0x02
	if((temp1 != 0x00) && (temp2 == 0x00))
	{
		return TRUE;
	}
	return FALSE;
}
u8 Clear_FIFO(void)
{
	u8 temp;
	u16  i;
	
	temp =read_reg(Control);						 
	temp = (temp | 0x01);
	write_reg(Control, temp);
	for(i = 0; i < RF_TimeOut; i++)				 
	{
		temp = read_reg(FIFO_Length);
		if(temp == 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}

u8 Read_FIFO(u8 *buff)
{
	u8 i,temp;
	temp =read_reg(FIFO_Length);
	if (temp == 0)
	{
		return 0;
	}
	 
	if (temp >= 20)						 
	{									//Ôö¼ÓFIFO_LengthÔ½ÏÞÅÐ¶Ï
		temp = 24;						//lishiqiang 2008-10-20-0:10
	}
	 
	for(i = 0;i < temp; i++)
	{
 		//*(buff + i) =read_reg(FIFO);
		*(buff + i) =read_reg(FIFO1);
	}
	return temp;
 }
void Write_FIFO(u8 count,u8 *buff)
{
	u8 i;//,temp;
	
	for(i = 0; i < count; i++)
	{
		write_reg(FIFO1,*(buff + i));			 ////
		/*
		temp=read_reg(0x04);
		while(temp>48) 
		temp=read_reg(0x04);
		*/
	}
}
u8 Command_Send(u8 count,u8  * buff,u8 Comm_Set)
{
	u16  j;
	u8 temp,temp1;
	//u16	lsls = 0;			 //20101129

	write_reg(Command, 0x00);   
	temp=Clear_FIFO();
	if(temp==FALSE)  
		return FALSE;        
    if (count != 0)
    {	
		Write_FIFO(count, buff);
    }	 
	write_reg(Command,Comm_Set);					//ÃüÁîÖ´ÐÐ
	for(j = 0; j< RF_TimeOut; j++)				//¼ì²éÃüÁîÖ´ÐÐ·ñ
	{
		temp =read_reg(Command);
		temp1 = read_reg(0x07) & 0x80;
		if((temp == 0x00)||(temp1 == 0x80))  
		{
			return TRUE;
		}
	}
	return FALSE;
}
//´øÓÐrc632¶¨Ê±Æ÷µÄ·¢ËÍº¯Êý
u8 Command_Send_Timer(u8 count,u8  * buff,u8 Comm_Set)
{
	u8 temp,val;	//temp1,
	
	write_reg(InterruptEn,0x7F); 	// disable all interrupts
    write_reg(Int_Req,0x7F); 		// reset interrupt requests
    write_reg(0x2b,0x06);			//·¢ÍêÆô¶¯£¬ÊÕµ½µÚÒ»Î»¹Ø¶¨Ê±Æ÷
	write_reg(Command, 0x00);   
	temp=Clear_FIFO();
	if(temp==FALSE)  
		return FALSE;    
    if (count != 0)
    {	
		Write_FIFO(count, buff);
    }	 
	write_reg(Command,Comm_Set);		
	while((read_reg(0x07)&0x20) == 0)			//»¹Ã»ÓÐtimeout
	{
		temp = read_reg(Command);
		if(temp == 0x00)  
		{
			val = read_reg(0x09);
			write_reg(0x09,val | 0x04);
			resetrctim();
			return TRUE;
		}
	}
	val = read_reg(0x09);
	write_reg(0x09,val | 0x04);
	resetrctim();
	return FALSE;
}
//´øÓÐrc632¶¨Ê±Æ÷µÄ·¢ËÍº¯Êý
u8 Command_Send_Timer_apdu(u8	pcb, u8 count,u8  * buff,u8 Comm_Set)
{
	u16  i,j=0,k;	 //iÓÃÓÚµÝ£¬jÓÃ×÷ÊÕ°É£¬kÓÃ×÷ÑÓÊ±
	u8 temp,val,direction=0;//temp1,
	u8	taworking[260];
	
	//IWDG_ReloadCounter();
	//SerialPutChar(0xcc);
	//SerialPutChar(pcb);
	for(i = 0; i < count; i ++)
	{
		tasendbuff[i] = *buff++;
		//SerialPutChar(tasendbuff[i]);
	}
	//SerialPutChar(0xcc);

	BlockErr = 0;	//Çå³¬Ê±´íÎó±êÖ¾
	errflagcrcorpar = 0; //ta414.0/1 ta415.0/1 ta416.0/1
	write_reg(InterruptEn,0x7F); 	// disable all interrupts
    write_reg(Int_Req,0x7F); 		// reset interrupt requests
    
	resetrctim();
	write_reg(0x2b,0x06);			//·¢ÍêÆô¶¯£¬ÊÕµ½µÚÒ»Î»¹Ø¶¨Ê±Æ÷
	write_reg(Command, 0x00);   
	temp=Clear_FIFO();
	if(temp==FALSE)  
		return FALSE;			 //0
//	temp=read_reg(0x29);		//¿´¾¯½äÖµ	 0x04,³õÊ¼»¯ÖÐÓÐÅäÖÃ
//	temp=read_reg(0x04);			//0
//	temp=read_reg(0x03);			//µÍ
	write_reg(Command,Comm_Set);
	//write_reg(Command,0x1a);
	if(count==0)
		write_reg(FIFO1,pcb);
	else	if (count != 0)
    {	
		write_reg(FIFO1,pcb);		//ÏÈ·¢ËÍpcb
		for(i = 0; i < count; i++)
		{
			write_reg(FIFO1,tasendbuff[i]);
			temp=read_reg(0x04);
			while(temp>48) 
				temp=read_reg(0x04);
		}
    }
	while(1)
	{
		if(((read_reg(0x07)&0x20) != 0)&&(direction<2))	//Èô³¬Ê±£¬ÇÒ·¢ÍêÁËÊý¾Ý£¬Ôò´íÎóÍË³ö.
		{
			val = read_reg(0x09);
			write_reg(0x09,val | 0x04);
			resetrctim();
			direction = 0;
			BlockErr = 1;		 //ÖÃ³¬Ê±±êÖ¾£¬ÔÚ½ÓÊÕµÄ´¦ÀíÖÐÓÐÓÃµ½
			return FALSE;		//¾ùÐèÒªÍ£Ö¹¶¨Ê±Æ÷²¢Çå±êÖ¾	0
		}
		if((direction==0)&&(read_reg(0x04)==0))	//ÕâÀï´ú±í·¢ËÍÍêÁË
			direction=1;
		if((direction!=0)&&(read_reg(0x04)>0))	//ÕâÀï´ú±íÓÐÊý¾ÝÀ´ÁË
		{
			//SerialPutChar(read_reg(0x04));
			taworking[j++] = read_reg(0x02);
			//SerialPutChar(taworking[j-1]);
			direction=2;						//ÉèÖÃ±êÖ¾µÄµÚÈýÖÖ¾³½ç£¬ÒòÎª»¹ÓÐ¿ÉÄÜµ½0
			//for(k = 0x1ff; k > 0; k--);
			if(j>254)
			{
				tareclen = 254;
				val = read_reg(0x09);
				write_reg(0x09,val | 0x04);
				resetrctim();
				return	FALSE;	
			}	
		}		
		temp =read_reg(Command);
		if(temp == 0x00) 
		{
			while(read_reg(0x04))
			{
				//SerialPutChar(read_reg(0x04));
				taworking[j++] = read_reg(0x02);
				if(j>254)
				{
					tareclen = 254;
					val = read_reg(0x09);
					write_reg(0x09,val | 0x04);
					resetrctim();
					return	FALSE;	
				}
				//SerialPutChar(taworking[j-1]);
				if(read_reg(0x03)&0x01)		//µÍ±¨¾¯ÁË£¬ÑÓÊ±100us 9.4*8=75.2us
				{
					for(k = 0x5ff; k > 0; k--);
				}
			}
			//LedGreenOn();		/////
			val = read_reg(0x09);
			write_reg(0x09,val | 0x04);
			resetrctim();
			//SerialPutChar(0xdd);
			//for(i=0;i<j;i++)
			//SerialPutChar(taworking[i]);					//ÊÕµ½µÄËùÓÐÊý¾Ý.
			tarecpcb = taworking[0];		   				//pcb
			//if(PCB)
			if(j > 0)
				tareclen = j - 1;							   //ËùÓÐµÄÊý¾Ý
			else
				tareclen = 0;	
			//SerialPutChar(0xcd);						   //·¢³öÖÐ¼ä½»»¥µÄ¹ý³Ì×Ö½Ú
			//SerialPutChar(tarecpcb);				   				//len
			//SerialPutChar(tareclen);
			for(i=0;i<tareclen;i++)
			{
				ta_workingBuffer[i] = taworking[i+1];	//ta_workingBuffer
				//SerialPutChar(ta_workingBuffer[i]);
			}
			//SerialPutChar(0xcd);	   
			direction = 0;
			temp = read_reg(0x0a);
			if((temp&0x02) ||(temp&0x08))	//0x48(ta403.0) 0x42(ta403.1) 0x4a(ta403.2)
			{
				BlockErr = 1;		 //ÖÃ³¬Ê±±êÖ¾£¬ÔÚ½ÓÊÕµÄ´¦ÀíÖÐÓÐÓÃµ½
				errflagcrcorpar = 1;
				return	FALSE;
			}	
			return TRUE;						 //1
		}
	}
}
//Õë¶ÔÑ°¿¨µÄ·¢ËÍ
u8 Command_Send1(u8 count,u8 *buff,u8 Comm_Set)
{
	u16  j;
	u8 temp,temp1;
	
	write_reg(Command, 0x00);   
	temp=Clear_FIFO();
	if(temp==FALSE)  
		return FALSE;			           
    if (count != 0)
    {
	   Write_FIFO(count, buff);
    }
		 
	write_reg(Command, Comm_Set);					//ÃüÁîÖ´ÐÐ
	for(j = 0; j< 100; j++)						   //4.45ms
	{
		temp = read_reg(Command);
		temp1 = read_reg(0x07) & 0x80;
		if((temp == 0x00)||(temp1 == 0x80))  
		{
			return TRUE;
		}
	}
	return FALSE;		
}
u8 FM1702_Bus_Sel(void)
{
	u8 i,temp;
//	u16	j;
	
	write_reg(Page_Sel,0x80);		//0x00	Ò³Ð´¼Ä´æÆ÷ 
	for(i = 0; i < 0x1fff; i++)	  //0x7f
	{
		temp=read_reg(Command);		  //	0x01	ÃüÁî¼Ä´æÆ÷ 
		if(temp == 0x00)
		{
			write_reg(Page_Sel,0x00);
			return TRUE;
		}
		//for(j=250;j>0;j--);
	}
	return FALSE;
}

void Fm1702Reset(u16 ms)
{
	u8 temp;
	u16	i;

	if(ms)
	{
		temp=read_reg(TxControl)&~0x03;
		write_reg(TxControl,temp);
		for(temp=0;temp<ms;temp++)
			for(i=0x4e20;i>0;i--);	//20000,¸ø¸ö´ó¸ÅµÄÊ±¼ä	
		temp=read_reg(TxControl)|0x03;
		write_reg(TxControl,temp);
	}
	else
	temp=read_reg(TxControl)&~0x03;
	write_reg(TxControl,temp);
}
void write_int()
{
	write_reg(0x10,0x0);write_reg(0x11,0x5b);//write_reg(0x11,0x59);
	write_reg(0x12,0x3f);write_reg(0x13,0x3f);
	write_reg(0x14,0x19);write_reg(0x15,0x13);
	write_reg(0x16,0x3f);write_reg(0x17,0x3b);
	write_reg(0x18,0x0);write_reg(0x19,0x73);
	write_reg(0x1a,0x08);write_reg(0x1b,0xad);
	write_reg(0x1c,0xff);write_reg(0x1d,0x1e);
	write_reg(0x1e,0x01);write_reg(0x1f,0x07);
	write_reg(0x20,0x0);write_reg(0x21,0x06);
	write_reg(0x22,0x03);write_reg(0x23,0x63);
	write_reg(0x24,0x63);write_reg(0x25,0x0);
	write_reg(0x26,0x02);write_reg(0x27,0x0);
	write_reg(0x28,0x0);write_reg(0x29,0x04);
	write_reg(0x2a,0x07);write_reg(0x2b,0x02);
	write_reg(0x2c,0x6a);write_reg(0x2d,0x03);
	write_reg(0x2e,0x0);write_reg(0x2f,0x0);
}
void Init_FM1702(void)
{	
    u8	dat;
	u32	k;
	
	PD_H();
	Delay_Ms(30);
	PD_L();
	Delay_Ms(30);
	dat = read_reg(0x01);
  	k = 0;
	while(dat != 0x00)					   
	{ 
		dat = read_reg(0x01);
		k ++;
	}
	write_int();
	SPI_SCK_L();
	SPI_CS_H();
	SPI_SI_H();
	SPI_SO_H();
	write_reg(InterruptEn,0x7F); // disable all interrupts
    write_reg(Int_Req,0x7F); // reset interrupt requests
    write_reg(Command,0x00);
}
void	Init_Ta(void)
{
	u8	temp;

	write_reg(CoderControl,0x19);     // Miller coding, 106kbps
	write_reg(RxControl1,0x73);
	 
	write_reg(DecoderControl,0x08);   // Manchester Coding
	write_reg(CRCPresetLSB,0x63);     // set CRC preset to 0x6363
	write_reg(CRCPresetMSB,0x63);
	write_reg(RxThreshold,0xff);  	// set max MinLevel & ColLevel.
	
	write_reg(ChannelRedundancy,0x03);
	temp=read_reg(Control)&0xf7;
	write_reg(Control,temp);
	write_reg(Bit_Frame,0x07); 
	//temp=read_reg(TxControl)|0x01;
	//write_reg(TxControl,temp);
	write_reg(TxControl,0x5b);
	/*
	write_reg(TxControl,0x5b);
	//write_reg(TxControl,0x59);
	write_reg(CoderControl,0x19);     // Miller coding, 106kbps
	write_reg(RxControl1,0x73);
	write_reg(DecoderControl,0x08);   // Manchester Coding
	write_reg(CRCPresetLSB,0x63);     // set CRC preset to 0x6363
	write_reg(CRCPresetMSB,0x63);
	write_reg(RxThreshold,0xff);  	// set max MinLevel & ColLevel.

	write_reg(ChannelRedundancy,0x03);
	temp=read_reg(Control)&0xf7;
	write_reg(Control,temp);
	write_reg(Bit_Frame,0x07); 
	//temp=read_reg(TxControl)|0x03;
	temp=read_reg(TxControl)|0x01;
	write_reg(TxControl,temp);
	*/
}
//Ñ°¿¨¶ÌÖ¡,ÎÞcrc
u8	Request(u8	mode)			   
{
	u8	temp;

	CascNum = 0;
	SeriNum = 0;
	requidlen = 0;
	Init_Ta();
	write_reg(RxWait,0x07);
	reqprotim();
	(mode==0)?(RfBuffer[0]=0x26):(RfBuffer[0]=0x52);
	write_reg(TimerClock, 0x06);		 		      
	write_reg(TimerReload, 0x2b);	
	temp = Command_Send_Timer(1, RfBuffer, Transceive);
	if(temp == FALSE)
	{
		return FM1702_NOTAGERR;
	}
	temp = read_reg(0x0a);
	if((temp&0x02)==0x02) 			
		return FM1702_PARITYERR;
	if((temp&0x04)==0x04)			
		return FM1702_FRAMINGERR;
	if((temp&0x08)==0x08) 
		return FM1702_CRCERR;
	if((temp&0x01)==0x01) 
		return 15;	
	temp = Read_FIFO(RfBuffer);
	if (temp == 2)				
	{							
		tagtype[0] = RfBuffer[0];
        tagtype[1] = RfBuffer[1];	 
		if((tagtype[0] & 0xc0) == 0)
			requidlen = 1;
		if((tagtype[0] & 0xc0) == 0x40)
			requidlen = 2;
		if((tagtype[0] & 0xc0) == 0x80)
			requidlen = 3;
		if((tagtype[0] & 0xc0) == 0xc0)
			return 16;
		if(((tagtype[0]&0x1f)!=1)&&((tagtype[0]&0x1f)!=2)&&((tagtype[0]&0x1f)!=4)&&((tagtype[0]&0x1f)!=8)&&((tagtype[0]&0x1f)!=0x10))
			return 16;
		return FM1702_OK;
	}
	return 16;	
}
void Save_UID(u8 row, u8 col, u8 length)
{
	/*~~~~~~~~~~*/
	u8	i;
	u8	temp;
	u8	temp1;
	/*~~~~~~~~~~*/

	if((row == 0x00) && (col == 0x00))
	{
		for(i = 0; i < length; i++)
		{
			UID[i] = RfBuffer[i];
		}
	}
	else
	{
		temp = RfBuffer[0];
		temp1 = UID[row - 1];
		switch(col)
		{
			case 0:		temp1 = 0x00; row = row + 1; break;
			case 1:		temp = temp & 0xFE; temp1 = temp1 & 0x01; break;
			case 2:		temp = temp & 0xFC; temp1 = temp1 & 0x03; break;
			case 3:		temp = temp & 0xF8; temp1 = temp1 & 0x07; break;
			case 4:		temp = temp & 0xF0; temp1 = temp1 & 0x0F; break;
			case 5:		temp = temp & 0xE0; temp1 = temp1 & 0x1F; break;
			case 6:		temp = temp & 0xC0; temp1 = temp1 & 0x3F; break;
			case 7:		temp = temp & 0x80; temp1 = temp1 & 0x7F; break;
			default:	break;
		}

		RfBuffer[0] = temp;
		UID[row - 1] = temp1 | temp;
		for(i = 1; i < length; i++)
		{
			UID[row - 1 + i] = RfBuffer[i];
		}
	}
}
u8 Check_UID(void)
{
	/*~~~~~~~~~*/
	u8	temp;
	u8	i;
	/*~~~~~~~~~*/

	temp = 0x00;
	for(i = 0; i < 5; i++)
	{
		temp = temp ^ UID[i];
	}

	if(temp == 0)
	{
		return TRUE;
	}

	return FALSE;
}


void Set_BitFraming(u8 row, u8 col)
{
	switch(row)
	{
		case 0:		RfBuffer[1] = 0x20; break;		  //×Ö½ÚÊýÎª2
		case 1:		RfBuffer[1] = 0x30; break;
		case 2:		RfBuffer[1] = 0x40; break;
		case 3:		RfBuffer[1] = 0x50; break;
		case 4:		RfBuffer[1] = 0x60; break;		  //×Ö½ÚÊýÎª6
		default:	break;
	}

	switch(col)									 //Î»Ö¡´¦Àí
	{
		case 0:		write_reg(0x0F,0x00);  break;
		case 1:		write_reg(0x0F,0x11); RfBuffer[1] = (RfBuffer[1] | 0x01); break;
		case 2:		write_reg(0x0F,0x22); RfBuffer[1] = (RfBuffer[1] | 0x02); break;
		case 3:		write_reg(0x0F,0x33); RfBuffer[1] = (RfBuffer[1] | 0x03); break;
		case 4:		write_reg(0x0F,0x44); RfBuffer[1] = (RfBuffer[1] | 0x04); break;
		case 5:		write_reg(0x0F,0x55); RfBuffer[1] = (RfBuffer[1] | 0x05); break;
		case 6:		write_reg(0x0F,0x66); RfBuffer[1] = (RfBuffer[1] | 0x06); break;
		case 7:		write_reg(0x0F,0x77); RfBuffer[1] = (RfBuffer[1] | 0x07); break;
		default:	break;
	}
}
//ÔÚ¸ß¼¶µÄactiveÓ¦ÓÃÖÐÊÇÎÞÐ§µÄ
u8 MIF_Halt(void)
{
	/*~~~~~~~~~*/
	u8	temp;
	u16	i;
	/*~~~~~~~~~*/
	
	//write_reg(0x23,0x63);
	//write_reg(0x12,0x3f);
	reqprotim();
	write_reg(0x22,0x07);               //change by hbp 03-->07
	*RfBuffer = RF_CMD_HALT;
	*(RfBuffer + 1) = 0x00;
	write_reg(TimerClock, 0x06);		 		      
	write_reg(TimerReload, 0x2b);	   	 
	temp = Command_Send_Timer(2, RfBuffer, Transceive);
	if(temp == TRUE)
	{
		for(i = 0; i < 0x80; i++);

		return FM1702_OK;
	}
	else
	{
		temp = read_reg(0x0A);
		if((temp & 0x02) == 0x02)
		{
			return(FM1702_PARITYERR);	 //	0x05	
		}

		if((temp & 0x04) == 0x04)
		{
			return(FM1702_FRAMINGERR);	  //	20		// FM1702Ö¡´íÎó 
		}

		return(FM1702_NOTAGERR);	//0x01ÎÞ¿¨  
	}
}
void	startsfgi(u8	tmp)	
{
	u8	val;   

	switch(tmp)
	{
		case	0: 	write_reg(TimerClock, 0x07);write_reg(TimerReload, 0x23);val = read_reg(0x09);write_reg(0x09, val | 0x02);break; //4480     	330us
		case	1: 	write_reg(TimerClock, 0x07);write_reg(TimerReload, 0x46);val = read_reg(0x09);write_reg(0x09, val | 0x02);break; //8960					632us
		case	2: 	write_reg(TimerClock, 0x07);write_reg(TimerReload, 0x8c);val = read_reg(0x09);write_reg(0x09, val | 0x02);break; //17920					1.24ms
		case	3: 	write_reg(TimerClock, 0x08);write_reg(TimerReload, 0x8c);val = read_reg(0x09);write_reg(0x09, val | 0x02);break; //35840								2.45ms
		case	4: 	write_reg(TimerClock, 0x09);write_reg(TimerReload, 0x8c);val = read_reg(0x09);write_reg(0x09, val | 0x02);break; //71680 								4.86ms
		case	5: 	write_reg(TimerClock, 0x0a);write_reg(TimerReload, 0x8c);val = read_reg(0x09);write_reg(0x09, val | 0x02);break; //143360								9.69ms
		case	6: 	write_reg(TimerClock, 0x0b);write_reg(TimerReload, 0x8c);val = read_reg(0x09);write_reg(0x09, val | 0x02);break; //286720								19.36ms
		case	7: 	write_reg(TimerClock, 0x0c);write_reg(TimerReload, 0x8c);val = read_reg(0x09);write_reg(0x09, val | 0x02);break; //573440 									38.69ms  ta105.7,ta105.8Õâ¸öÊ±¼äÉÔ¶ÌÁËÒ»µã
		case	8: 	write_reg(TimerClock, 0x0d);write_reg(TimerReload, 0x8c);val = read_reg(0x09);write_reg(0x09, val | 0x02);break; //1146880									77.36ms
		case	9: 	write_reg(TimerClock, 0x0e);write_reg(TimerReload, 0x8c);val = read_reg(0x09);write_reg(0x09, val | 0x02);break; //2293760
		case	10: write_reg(TimerClock, 0x0f);write_reg(TimerReload, 0x8c);val = read_reg(0x09);write_reg(0x09, val | 0x02);break; //4587520		 //ÏÂÃæ²î±ð½ÏÐ¡£¬²»Ðè×÷´¦Àí	,ÔçÔÚÇ§·ÖÖ®Ò»ÍâÁË
		case	11: write_reg(TimerClock, 0x10);write_reg(TimerReload, 0x8c);val = read_reg(0x09);write_reg(0x09, val | 0x02);break; //9175040
		case	12: write_reg(TimerClock, 0x11);write_reg(TimerReload, 0x8c);val = read_reg(0x09);write_reg(0x09, val | 0x02);break; //18350080
		case	13: write_reg(TimerClock, 0x12);write_reg(TimerReload, 0x8c);val = read_reg(0x09);write_reg(0x09, val | 0x02);break; //36700160
		case	14: write_reg(TimerClock, 0x13);write_reg(TimerReload, 0x8c);val = read_reg(0x09);write_reg(0x09, val | 0x02);break; //73400320
		default:	write_reg(TimerClock, 0x09);write_reg(TimerReload, 0x8c);val = read_reg(0x09);write_reg(0x09, val | 0x02);break; //76180		
	}	
}
void	startsstdfwi(u8	tmp)	//(fwt+xfwt)*4096	±ê×¼µÄfwiÊ±¼äÓ¦ÓÃ
{
	u8	val;   //256*16*2^x	   ·ÖÆµ,¼ÆÊý,  8+4+x

	switch(tmp)
	{
		case	0: 	write_reg(TimerClock, 0x07);write_reg(TimerReload, 0x25);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
		case	1: 	write_reg(TimerClock, 0x07);write_reg(TimerReload, 0x47);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
		case	2: 	write_reg(TimerClock, 0x07);write_reg(TimerReload, 0x8e);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
		case	3: 	write_reg(TimerClock, 0x08);write_reg(TimerReload, 0x8e);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
		case	4: 	write_reg(TimerClock, 0x09);write_reg(TimerReload, 0x8e);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
		case	5: 	write_reg(TimerClock, 0x0a);write_reg(TimerReload, 0x8e);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
		case	6: 	write_reg(TimerClock, 0x0b);write_reg(TimerReload, 0x8e);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
		case	7: 	write_reg(TimerClock, 0x0c);write_reg(TimerReload, 0x8e);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
		case	8: 	write_reg(TimerClock, 0x0d);write_reg(TimerReload, 0x8e);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
		case	9: 	write_reg(TimerClock, 0x0e);write_reg(TimerReload, 0x8e);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
		case	10: write_reg(TimerClock, 0x0f);write_reg(TimerReload, 0x8e);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
		case	11: write_reg(TimerClock, 0x10);write_reg(TimerReload, 0x8e);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
		case	12: write_reg(TimerClock, 0x11);write_reg(TimerReload, 0x8e);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
		case	13: write_reg(TimerClock, 0x12);write_reg(TimerReload, 0x8e);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
		case	14: write_reg(TimerClock, 0x13);write_reg(TimerReload, 0x8e);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
		default:	write_reg(TimerClock, 0x09);write_reg(TimerReload, 0x8e);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
	}	
}
void	startsfwi(u8	tmp)
{
	u8	val;   

	switch(tmp)
	{
		case	0: 	write_reg(TimerClock, 0x07);write_reg(TimerReload, 0x27);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
		case	1: 	write_reg(TimerClock, 0x07);write_reg(TimerReload, 0x6c);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
		case	2: 	write_reg(TimerClock, 0x08);write_reg(TimerReload, 0x6c);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
		case	3: 	write_reg(TimerClock, 0x08);write_reg(TimerReload, 0xa2);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
		case	4: 	write_reg(TimerClock, 0x09);write_reg(TimerReload, 0xa2);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
		case	5: 	write_reg(TimerClock, 0x0a);write_reg(TimerReload, 0xa2);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
		case	6: 	write_reg(TimerClock, 0x0b);write_reg(TimerReload, 0x95);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
		case	7: 	write_reg(TimerClock, 0x0c);write_reg(TimerReload, 0x92);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
		case	8: 	write_reg(TimerClock, 0x0d);write_reg(TimerReload, 0x92);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
		case	9: 	write_reg(TimerClock, 0x0e);write_reg(TimerReload, 0x92);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
		case	10: write_reg(TimerClock, 0x0f);write_reg(TimerReload, 0x94);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
		case	11: write_reg(TimerClock, 0x10);write_reg(TimerReload, 0x93);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
		case	12: write_reg(TimerClock, 0x11);write_reg(TimerReload, 0x8f);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
		case	13: write_reg(TimerClock, 0x12);write_reg(TimerReload, 0x8f);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
		case	14: write_reg(TimerClock, 0x13);write_reg(TimerReload, 0x8f);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 
		default:	write_reg(TimerClock, 0x09);write_reg(TimerReload, 0xa2);val = read_reg(0x09);write_reg(0x09, val | 0x04);break; 		
	
	}	
}
/**********************************
//TA rst			
**********************************/
u8 Cpu_Rst(void)
{
	
	u8	temp,i,j, rfhisty[30];

	rfhisty[0] = rfhisty[0];
	reqprotim();	
	RfReset();					
	write_reg(0x23,0x63);
	write_reg(0x12,0x3f);
	write_reg(0x22,0x0f);	         
	write_reg(RxWait,0x07);
	RfBuffer[0]=0xe0;
	RfBuffer[1]=0x80;			
	write_reg(TimerClock, 0x09);
	write_reg(TimerReload, 0xba);	
	temp = Command_Send_Timer(2, RfBuffer, Transceive);
	if(temp == FALSE)
	{
		return(FM1702_NOTAGERR);
	}
	else
	{
		//atrlen = read_reg(0x04);
		temp = read_reg(0x0A);
		if((temp & 0x02) == 0x02) 
			return FM1702_PARITYERR;
		if((temp & 0x04) == 0x04) 
			return FM1702_FRAMINGERR;   
		if((temp & 0x08) == 0x08) 
			return FM1702_CRCERR;
		Total_Rec_Lenght = read_reg(0x04);
		Read_FIFO(Rst_Data);	
		rftl = 	Rst_Data[0];				
		if(rftl != Total_Rec_Lenght)		
		{
			Clear_FIFO();
			return	1;
		}
		if(Total_Rec_Lenght>1)	
		{
			rft0 = Rst_Data[1];	
			rffsci = rft0 & 0x0f;
			fsclen = rffsc[rffsci];		
			i = 2;
			if(rft0&0x10)				
			{
				rfta1 = Rst_Data[i++];
				rfds   = (rfta1& 0x70) >> 4;
				rfdr   = (rfta1& 0x07);
			}
			if(rft0&0x20)				
			{
				rftb1 = Rst_Data[i++];
				rffwi  = (rftb1&0xf0) >> 4;
				rfsfgi = (rftb1&0x0f);	
			}
			if(rft0&0x40)				
			{
				rftc1 = Rst_Data[i++];
				rfnad  = rftc1 &0x01;
				rfcid  = (rftc1 &0x02)>>1;
			}
			for(j = 0; j < (rftl - i); j++)		 
				rfhisty[j] = Rst_Data[i+j];	
		}
		Head = 0x02;
		IPU_TANO1 = 0;
		Rf_IFD_Seq = Rf_ICC_Seq = 0; 
		startsfgi(rfsfgi);
		return(FM1702_OK);
	}
}
//********************************************************************************/
//REFER the following sections of EMV4.1 book1 to understand this function better
//               Section  9.2.4.2.2 Timing for T=1
//               Section  9.2.4 Block Protocol T=1  
//			     Section  9.2.5.1 Protocol Rules for Error Handling
//********************************************************************************/
int sendTaBlock(u8 PCB, u8 length, u8 *buffer)
{
	u8	val;				
	
	val = read_reg(0x09);
	write_reg(0x09,val|0x04);
	resetrctim();
	startsfwi(rffwi);			  
	val = Command_Send_Timer_apdu(PCB, length, buffer, Transceive);
	return	val;
}
int sendTaSBlock(u8 PCB, u8 length, u8 *buffer)
{
	u8	val; 			
	
	val = Command_Send_Timer_apdu(PCB, length, buffer, Transceive);
	return	val;
}
u32	pow2n(u8	xy)
{
	u8	i;
	u32	ss;

	ss = 1;
	for(i = 0; i < xy; i++)
		ss *= 2;
	return	ss;
}
int TareceiveSBlock(uint8_t *rPCB, uint8_t *rLEN)
{
	u8	temp, temp2, i;
	s16	ret = 0;
//	u16	j;
	u32	wtxtime = 0, wtxtimetemp = 0;
	u8	prefp = 0, prereload = 0;

	exitapdu = 0;
	if(BlockErr)	  
	{
		if(errflagcrcorpar)		   
		{
			errflagcrcorpar = 0;
			startsfgi(rffwi);
			if(read_reg(0x05)&0x80)			
			{
				temp = read_reg(0x07);		
				while((temp&0x20) == 0)
					temp = read_reg(0x07);
			}
			temp = read_reg(0x09);
			write_reg(0x09,temp | 0x04);
			resetrctim();
			Delay_Ms(1);	
		}
		BlockErr = 0;
		return	-10;
	}
	*rPCB = tarecpcb;
	*rLEN = tareclen;
	if(((tarecpcb&0xc0) == 0)&&(((tarecpcb&0x02) == 0) ||((tarecpcb&0x04) != 0)||((tarecpcb&0x08) != 0))) 
		return	-30;
	if(((tarecpcb&0xc0) == 0x80)&&(((tarecpcb&0x20) == 0x00) ||((tarecpcb&0x02) == 0)||((tarecpcb&0x04) != 0)||((tarecpcb&0x08) != 0)))	 
		return	-30;
	if( tareclen > 253)		 
	{
		Delay_Ms(1);
		return	-31;
	}
	if(((tarecpcb&0xc0)==0x80)&&((tarecpcb&0x10) != 0))	  
		return	-30;
    switch (*rPCB)
    {
      case SREQUEST_Deselt :                //Card requests to ABORT, send ABORT error code
	  {										//to abort transaction
     	  return	-30;
	  }

      case SREQUEST_Wtx :	   			   //Card sends WTX Request
	  	 
		 temp = ta_workingBuffer[0];
		 if(temp == 0)				   
		 	return	-30;
		 prefp = 0;
		 prereload = 0;
		 if(temp > 59)
		 	temp = 59;
		 wtxtime = (4096 + 384) * pow2n(rffwi);	  
		 wtxtime *= temp;				 
		 if(wtxtime > 73400320)
		 	wtxtime = 73400320;
		 wtxtime +=	271200;		
		 wtxtimetemp = 	wtxtime;
		 for(i = 0; i < 22; i++)
		 {
		 	if((wtxtimetemp/2)>128)
			{
				wtxtimetemp /= 2;	
			}
			else
			{
				prereload = wtxtimetemp;	
				prefp = i;
				write_reg(TimerClock, prefp);
				write_reg(TimerReload, prereload);
				temp2 = read_reg(0x09);
				write_reg(0x09, temp2 | 0x04);
				i = 22;
			} 			
		 }
		 ret = sendTaSBlock(SREQUEST_Wtx, 1, ta_workingBuffer);		  
		 *rPCB = tarecpcb;
		 *rLEN = tareclen;
		 return	ret;
    }
   return	0;
}
/*******************************************************/
//Èë²Î:´«ÈëÊý¾ÝµÄÖ¸ÕëÓë³¤¶È	   *req_buf	  req_len
//³ö²Î:ÊÕµ½Êý¾ÝµÄÖ¸Õë		   *resp_buf
//·µ»Ø:ÊÕµ½Êý¾ÝµÄ³¤¶È		   return	xx
/*******************************************************/
#if	0
s16 RfApduA(u8 *req_buf, u16 req_len, u8 *resp_buf)
{
  int ret;
  int currentIBlockLen,more,retry,i_block_retry;
  int totalResLength;
//  uint8_t rNAD,rblock_sent=0;
  int cardiblock_err=0;
  uint8_t rPCB;
  uint8_t rLEN;
  u8	val, FbNum;
  // Get working buffer of current cardslot.
//  uint8_t *rbuffer=ta_workingBuffer;

 	FbNum = 0;
	Delay_Ms(1);  
  	IWDG_ReloadCounter();
	 
	if (IPU_TANO1 == 0)						
	{										
		if(read_reg(0x05)&0x80)				
		{
			val = read_reg(0x07);			
			while((val&0x20) == 0)
				val = read_reg(0x07);
		}
		val = read_reg(0x09);
		write_reg(0x09,val | 0x04);			
		resetrctim();						
		IPU_TANO1 = 1;
	}
	else
	{
		Delay_Ms(2);
		//reqprotim();
	}
  Delay_Ms(1);
  while (req_len) 
  {
//    IWDG_ReloadCounter();		
	currentIBlockLen = (req_len < (fsclen-3)) ? req_len : (fsclen-3);	 
  
    // We do chaining if request length is greater than IFSC
    req_len -= currentIBlockLen;
    more = req_len ? 1 : 0;     
    
    retry = T1_BLOCK_RETRY;

    i_block_retry=1;		
    cardiblock_err=0;	  

	sendTaBlock(TACODEIBLOCK(Rf_IFD_Seq,more), currentIBlockLen,req_buf);
	//make sure block gets sent successfully within three times
		
	while (retry--)
    {
	  ret = TareceiveSBlock(&rPCB,&rLEN);
	  
	  if ((ret >= 0 ))		//0xfffff8
      {

          if((TAISRBLOCK(rPCB)))	//Check whether card sent R-Block	
    	  {
				//Check IFDseq to see whether wants to re-transmit previous block
    	      	
				if(TANR(rPCB) != Rf_IFD_Seq)
    	    	{  
    	    	    if(((++i_block_retry)>3))	 
    	    	    {
    	    	    	retry=-1;
    	    	    	ret = -21;//ERR_MAX_RETRIED;
    	    	    	break;
    	    	    }
					sendTaBlock(TACODEIBLOCK(Rf_IFD_Seq,more), currentIBlockLen, req_buf);
    	    	    retry++; 
					if(i_block_retry==3)   
						retry++;	 
                	continue;
    	     	}
    	    	else 				
    	    	{   if (more) //Card sent ACK for previous i-block, send next chained i-block
    	    	    {							   
                	    Rf_IFD_Seq = 1 - Rf_IFD_Seq; 
    	            	break;  
    	    	    }							 
    	    	    else 		 //there could be error in icc response to last i-block
    	    	    {
    	    	    	return	-20;		 
    	    	    }
    	     	}
           	}								
    	    else if (TAISIBLOCK(rPCB)) //Card started sending APDU response, getinto receive APDU state machine
    	    { 
              	if (((TANS(rPCB) == Rf_IFD_Seq) && (more==0)))	
				{													 
					Rf_IFD_Seq = 1 - Rf_IFD_Seq;
					break; 
				}
				else
					return	-20;
           	}
        }
		if(ret == -30)
		{
			return	-20;
		}
		if(ret == -31)
		{
			return	-31;
		}
       //Card wants to abort T1 transaction, return with Abort Error Code.
       if(ret== -20)
	   {
    	    return -20;//ERR_ABORT_T1_TRANSACTION;
       }

    	if(retry > 0)							  
    	{									  
    	    FbNum ++;
			//if(FbNum > 20)		//³¬Ê±Ê±¼ä
			if(FbNum > 35)
			{
				FbNum = 0;
				return	-32;
			}
			else
				retry ++;
			if((cardiblock_err)) 
				ret=cardiblock_err;
          	//send R block saying we got invalid block
			sendTaBlock(TACODERBLOCK(Rf_IFD_Seq,more)|0x10, 0, 0);
			cardiblock_err =ret;
			continue;
    	}
    }

	//The transaction errors exceeded EMV limit, so, return error code
    if (retry < 0)		//1cf.061.00
    {							
    	return -21;//ERR_MAX_RETRIED;    
    }
    req_buf += currentIBlockLen;
  }

  totalResLength = 0;

  do
  {
    memcpy(resp_buf,ta_workingBuffer, rLEN);
    resp_buf += rLEN;
    totalResLength += rLEN;
    
    Rf_ICC_Seq = 1 - Rf_ICC_Seq;

    //Check whether there are more chained APDU responses blocks
    if (TAMORE(rPCB))			
    {
      sendTaBlock(TACODERBLOCK(Rf_ICC_Seq, 0), 0, (void*)0);
      retry = T1_BLOCK_RETRY;

      while (retry)
      {
        ret = TareceiveSBlock(&rPCB,&rLEN);
        if(ret >= 0)	   
    	{

          if (TAISRBLOCK(rPCB))  //Card did not receive our ACK properly, resend it
    	  {	
             return	-20;
    	  }
    	  else if (TAISIBLOCK(rPCB))	
    	  {								
    	      if (TANS(rPCB) == Rf_ICC_Seq)
    	      {
    	    	Rf_IFD_Seq = 1 - Rf_IFD_Seq;  //20101214 ls	ta202.x
				break;  /* A valid I-Block,break this loop */
    	      }
			  else
			  	ret = -30;	    
    	  }
    	}
		if(ret== -30)
		{										 
    	    return -20;//ERR_ABORT_T1_TRANSACTION;
    	}
		if(ret== -31)		 //tb411.4
		{										 
    	    return -31;//ERR_ABORT_T1_TRANSACTION;
    	}										
    	//if(ret== ERR_ABORT_T1_TRANSACTION)	//Card wants to ABORT Transaction, return with Error Code
    	if(ret== -20)
		{										
    	    return -20;//ERR_ABORT_T1_TRANSACTION;
    	}
    	if(--retry)							  
    	{
          	//send R block saying we got invalid block
    	    sendTaBlock(TACODERBLOCK(Rf_ICC_Seq, 0), 0,(void*)0);
    	}

      }

      if (!retry)		
      {
           return -21;//ERR_MAX_RETRIED;
      }
    }
    else		  
    {
    	break;	  
    }
  } while (1);
  return totalResLength;
}

#endif
s16 RfApduA(u8 *req_buf, u16 req_len, u8 *resp_buf)
{
  int ret;
  int currentIBlockLen,more,retry,i_block_retry;
  int totalResLength;
  //uint8_t rNAD,rblock_sent=0;
  int cardiblock_err=0;
  uint8_t rPCB;
  uint8_t rLEN;
  u8	val, FbNum;
  unsigned	long	RfTimeOut;	
  // Get working buffer of current cardslot.
  //uint8_t *rbuffer=ta_workingBuffer;

 	//write_reg(0x22,0x0f);
	FbNum = 0;
	Delay_Ms(1);  
  	//IWDG_ReloadCounter();
	 
	if (IPU_TANO1 == 0)						
	{										
		if(read_reg(0x05)&0x80)				
		{
			val = read_reg(0x07);			
			while((val&0x20) == 0)
				val = read_reg(0x07);
		}
		val = read_reg(0x09);
		write_reg(0x09,val | 0x04);			
		resetrctim();						
		IPU_TANO1 = 1;
	}
	else
	{
		Delay_Ms(2);
		//reqprotim();
	}
  Delay_Ms(1);
  while (req_len) 
  {
    //IWDG_ReloadCounter();		
	currentIBlockLen = (req_len < (fsclen-3)) ? req_len : (fsclen-3);	 
  
    // We do chaining if request length is greater than IFSC
    req_len -= currentIBlockLen;
    more = req_len ? 1 : 0;     
    
    retry = T1_BLOCK_RETRY;

    i_block_retry=1;		
    cardiblock_err=0;	  

	RfTimeOut = systicnum;
	sendTaBlock(TACODEIBLOCK(Rf_IFD_Seq, more), currentIBlockLen,req_buf);
	//make sure block gets sent successfully within three times
		
	while (retry--)
    {
	  ret = TareceiveSBlock(&rPCB,&rLEN);
	  
	  if ((ret >= 0 ))		//0xfffff8
      {

          if((TAISRBLOCK(rPCB)))	//Check whether card sent R-Block	
    	  {
				//Check IFDseq to see whether wants to re-transmit previous block
    	      	
				if(TANR(rPCB) != Rf_IFD_Seq)
    	    	{  
    	    	    if(((++i_block_retry)>3))	 
    	    	    {
    	    	    	retry=-1;
    	    	    	ret = -21;//ERR_MAX_RETRIED;
    	    	    	break;
    	    	    }
					sendTaBlock(TACODEIBLOCK(Rf_IFD_Seq,more), currentIBlockLen, req_buf);
    	    	    retry++; 
					if(i_block_retry==3)   
						retry++;	 
                	continue;
    	     	}
    	    	else 				
    	    	{   if (more) //Card sent ACK for previous i-block, send next chained i-block
    	    	    {							   
                	    Rf_IFD_Seq = 1 - Rf_IFD_Seq; 
    	            	break;  
    	    	    }							 
    	    	    else 		 //there could be error in icc response to last i-block
    	    	    {
    	    	    	return	-20;		 
    	    	    }
    	     	}
           	}								
    	    else if (TAISIBLOCK(rPCB)) //Card started sending APDU response, getinto receive APDU state machine
    	    { 
              	if (((TANS(rPCB) == Rf_IFD_Seq) && (more==0)))	
				{													 
					Rf_IFD_Seq = 1 - Rf_IFD_Seq;
					break; 
				}
				else
					return	-20;
           	}
        }
		if(ret == -30)
		{
			return	-20;
		}
		if(ret == -31)
		{
			return	-31;
		}
       //Card wants to abort T1 transaction, return with Abort Error Code.
       if(ret== -20)
	   {
    	    return -20;//ERR_ABORT_T1_TRANSACTION;
       }

    	if(retry > 0)							  
    	{									  
    	    FbNum ++;
			if((((rffwi < 11) || (rffwi > 14)) && ((systicnum - RfTimeOut) > 1400)) || \
			  (((rffwi >= 11) && (rffwi <= 14)) && (FbNum > 2)))
			{
				FbNum = 0;
				return	-32;
			}	
			else
				retry ++;			
			if((cardiblock_err)) 
				ret=cardiblock_err;
          	//send R block saying we got invalid block
			sendTaBlock(TACODERBLOCK(Rf_IFD_Seq,more)|0x10, 0, 0);	//20110617
			//sendTaBlock(TACODERBLOCK(Rf_IFD_Seq,more)& 0xef, 0, 0);
			cardiblock_err =ret;
			continue;
    	}
    }

	//The transaction errors exceeded EMV limit, so, return error code
    if (retry < 0)		//1cf.061.00
    {							
    	return -21;//ERR_MAX_RETRIED;    
    }
    req_buf += currentIBlockLen;
  }

  totalResLength = 0;

  do
  {
    memcpy(resp_buf,ta_workingBuffer, rLEN);
    resp_buf += rLEN;
    totalResLength += rLEN;
    
    Rf_ICC_Seq = 1 - Rf_ICC_Seq;

    //Check whether there are more chained APDU responses blocks
    if (TAMORE(rPCB))			
    {
      //sendTaBlock(TACODERBLOCK(Rf_ICC_Seq, 0), 0, (void*)0);
	  sendTaBlock(TACODERBLOCK(Rf_IFD_Seq, 0), 0, (void*)0);   //20110510
      retry = T1_BLOCK_RETRY;

      while (retry)
      {
        ret = TareceiveSBlock(&rPCB,&rLEN);
        if(ret >= 0)	   
    	{

          if (TAISRBLOCK(rPCB))  //Card did not receive our ACK properly, resend it
    	  {	
             return	-20;
    	  }
    	  else if (TAISIBLOCK(rPCB))	
    	  {								
    	      //if (TANS(rPCB) == Rf_ICC_Seq)
			  if (TANS(rPCB) == Rf_IFD_Seq)		//20110510
    	      {
    	    	Rf_IFD_Seq = 1 - Rf_IFD_Seq;  //20101214 ls	ta202.x
				break;  /* A valid I-Block,break this loop */
    	      }
			  else
			  	ret = -30;	    
    	  }
    	}
		if(ret== -30)
		{										 
    	    return -20;//ERR_ABORT_T1_TRANSACTION;
    	}
		if(ret== -31)		 //tb411.4
		{										 
    	    return -31;//ERR_ABORT_T1_TRANSACTION;
    	}										
    	//if(ret== ERR_ABORT_T1_TRANSACTION)	//Card wants to ABORT Transaction, return with Error Code
    	if(ret== -20)
		{										
    	    return -20;//ERR_ABORT_T1_TRANSACTION;
    	}
    	if(--retry)							  
    	{
          	//send R block saying we got invalid block
    	    sendTaBlock(TACODERBLOCK(Rf_ICC_Seq, 0), 0,(void*)0);
    	}

      }

      if (!retry)		
      {
           return -21;//ERR_MAX_RETRIED;
      }
    }
    else		  
    {
    	break;	  
    }
  } while (1);
  return totalResLength;
} 
/**************************************************************************/
//ÊäÈë²ÎÊý: 
//»Ø´«²ÎÊý: *pt  
//·µ»Ø×´Ì¬: 0x0000: ÎÞ¿¨·µ»Ø:0x3001,A,B³åÍ»·µ»Ø:0x3006,A¿¨³åÍ»·µ»Ø0x3009,B¿¨³åÍ»·µ»Ø0x3007
// 0x3006: µÈ¿¨³¬Ê±  0x3009: ÓÐ¶àÕÅ¿¨½øÈëÌìÏß
//·µ»ØÊý¾Ý: 1byte(type) + 1byte(UIDLEN) + 4byte(UID) + 1byte(ATRLen) + nbyte(ATR)
//ÐèÒª·µ»Ø¹Ø³¡µÄ¶¼ÒªÇå±êÖ¾£¬Í¨¹ýÁíÒ»±êÖ¾À´´¦Àí¹Ø³¡ÎÊÌâ
/**************************************************************************/
u16	RequCpu(u16	MyDelayTime, u8	*pt, u16	*Atrlen)
{
	u8	status, i;  //PreCard 0x01Ê±ÓÐa¿¨,0x10Ê±ÊÇÓÐb¿¨

	*Atrlen = 0;
	SoftOpenRf();
	Delay_Ms(2);
	status = Request(1);
	if(status != 0)			
		return	MW_NOTAG;//0x3001;

	*pt++ = 0x0a;			   //type
	status = Mf500PiccCascAnticoll();
	//if(status == 23)
	//	return	MW_ANTICOLL;//0x3009;		   //A³åÍ»
	if(status)
		return	status;//0x3006;
	*pt++ = SeriNum;
	for(i = 0; i < SeriNum; i++)
		*pt++ = UIDCasc[i];
	status = Cpu_Rst();
	if(status)
		return	MW_CPURST_ERR;//0x3006;		   //¼¤»îÊ§°Ü

	*pt++ = Total_Rec_Lenght;
    for(i = 0; i < Total_Rec_Lenght; i++)
    {
   		*pt++ = Rst_Data[i];
    }
	*Atrlen = Total_Rec_Lenght + SeriNum + 3;
	return	MW_OK;//0;
}
void	SoftCloRf(void)
{
	u8	temp;

	temp=read_reg(TxControl)&~0x03;
	write_reg(TxControl,temp);
	return;
}
void	SoftOpenRf(void)
{
	u8	temp;

	temp=read_reg(TxControl)|0x03;
	write_reg(TxControl,temp);
	return;
}
/********************************************
NXPÏà¹Ø
********************************************/
///////////////////////////////////////////////////////////////////////
//      M I F A R E   M O D U L E   C O N F I G U R A T I O N
///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//   S E T   A   B I T   M A S K 
///////////////////////////////////////////////////////////////////////

char SetBitMask(unsigned char reg,unsigned char mask) // 
{
   char  tmp = 0x0;

   tmp = read_reg(reg);
   write_reg(reg,tmp | mask);  // set bit mask
   return 0x0;
}

//////////////////////////////////////////////////////////////////////
//   C L E A R   A   B I T   M A S K 
///////////////////////////////////////////////////////////////////////
char ClearBitMask(unsigned char reg,unsigned char mask) // 
{
   char  tmp = 0x0;

   tmp = read_reg(reg);
   write_reg(reg,tmp & ~mask);  // clear bit mask
   return 0x0;
}
//////////////////////////////////////////////
//¼Æ Tim4PlanTime ¸ö100ms,Tim4NumÊÇÕýÔÚ×ßµ½ÄÄÀï
//////////////////////////////////////////////
void start_timeout(unsigned int _100ms)
{
	//u16	i;
	Tim4Num = 0;			//¼ÆÊý³õÖµÇå0
	Tim4PlanTime = _100ms;	//ÐèÒªÖÐ¶ÏµÄ´ÎÊý
	TIM_Cmd(TIM4, ENABLE);
	return;	
}
/*******************************/
//¶¨Ê±Æ÷Ê§ÄÜ£¬²¢ÇåÏà¹ØÊý¾Ý
/*******************************/
void stop_timeout(void)
{
	TIM_Cmd(TIM4, DISABLE);	
	Tim4Num = 0;		
	Tim4PlanTime = 0;
	return;
}
char Mf500PiccCascAnticoll (void)		   //Ö¸Áî,·À³åÍ»µÄÎ»£¬µÃµ½µÄÐòÁÐºÅ
{
/*~~~~~~~~~~~~~*/
	u8	temp;
	u8	FenZu = 0;
	/*~~~~~~~~~~~~~*/

	CascNum =0;
	do
	{
		if(CascNum > (requidlen - 1))	 //0,1,2   1 2 3
				return	MW_ANTICOLL;//1;				 //ÓëatrsÖÐÖ¸³öµÄ¼¶Êý²»Ò»ÖÂ
		temp = 	RCAntiColl(RF_CMD_ANTICOL + 2 * CascNum);
		if(temp == 0)
		{
			temp = 	RCSelect_Card(RF_CMD_ANTICOL + 2 * CascNum);
			if(temp == 15)
				return	MW_SELECT;//1;
			if(temp == 0)
			{
			 	FenZu = 0;
			}
			else	if(temp == 5)	 //ats·µ»Ø¸æËßËµÓÐ¼¶Áª
			{
				CascNum ++;
				FenZu = 1;	
			}
			else
				return	MW_SELECT;//1;	
		}
		else
			if(temp == 23)
				return	MW_ANTICOLL;//23;
		else	
			return	MW_ANTICOLL;//1;
	}
	while(FenZu &&(CascNum < 3));	   //ÓÐ¼¶Áª²ÅÔÙ´ÎÑ­»·£¬²¢ÔÚÈý¼¶ÒÔÄÚ
	return	MW_OK;//0;
							  //CascNum:1
}
u8 RCAntiColl(u8 Casc_Cmd)
{
	u8	temp;
	u8	i;
	u8	row, col;
	
	reqprotim();
	row = 0;
	col = 0;
	write_reg(0x23,0x63);
	write_reg(0x12,0x3f);
	write_reg(0x13,0x3f);
	write_reg(0x22,0x03);
	write_reg(RxWait,0x06);
	RfBuffer[0] = Casc_Cmd;
	RfBuffer[1] = 0x20;
	write_reg(TimerClock, 0x06);
	write_reg(TimerReload, 0x2b);	   
	temp = Command_Send_Timer(2, RfBuffer, Transceive);	
	if(temp == FALSE)				
		return(FM1702_BYTECOUNTERR);	
	temp = read_reg(0x04);
	if(temp > 5)			 
	{
		return 15;
	}
	Read_FIFO(RfBuffer);			   
	Save_UID(row, col, temp);		 
	if((UID[0] == 0x88)&&(((CascNum == 0)&&(requidlen == 1)) || ((CascNum == 1)&&(requidlen == 2)) || ((CascNum == 2)&&(requidlen == 3))))
		return	15;					
	if(UID[0] == 0x88)				
	{
		for(i = 1; i < 4; i++)
			UIDCasc[SeriNum++] = UID[i];	
	}
	else
		for(i = 0; i < 4; i++)			
			UIDCasc[SeriNum++] = UID[i];	  
	temp = read_reg(0x0A);				     
	if((temp & 0x02) == 0x02) 		//0x42 1701³öÏÖÎÊÌâµÄµØ·½£¬¿ÉÄÜÊÇÓëÐ¾Æ¬»òÕßÌìÏßÓÐ¹Ø	  µ÷ÊÔÊ±ÆÁ±Î
		return FM1702_PARITYERR;
	temp &= 0x01;		  
	if(temp == 0x00)
	{
		temp = Check_UID();		 
		if(temp == FALSE)
		{
			return(FM1702_SERNRERR);
		}
		return(FM1702_OK);
	}
	else
	{
		return(FM1702_ANTICOLLERR);			 
	}
}
u8 RCSelect_Card(u8	Casc_Cmd)
{
	/*~~~~~~~~~~~~*/
	u8	temp, i;
	/*~~~~~~~~~~~~*/

	//Delay_Ms(3);
	reqprotim();
	write_reg(0x23,0x63);
	write_reg(0x12,0x3f);
	write_reg(0x22,0x0f);	           
	write_reg(RxWait,0x07);			 
	RfBuffer[0] = Casc_Cmd;
	RfBuffer[1] = 0x70;
	for(i = 0; i < 5; i++)
	{
		RfBuffer[i + 2] = UID[i];
	}		      
	write_reg(TimerReload, 0x2b);	   // Êý¾Ý·ÖÎöµÃµ½µÄÊ±¼ä
	temp = Command_Send_Timer(7, RfBuffer, Transceive);
	if(temp == FALSE)
	{
		return(FM1702_NOTAGERR);
	}
	else									  //ta305²âÊÔ
	{
		temp = read_reg(0x0A);
		if((temp & 0x02) == 0x02) 
			return  15;
		if((temp & 0x04) == 0x04) 
			return(FM1702_FRAMINGERR);
		if((temp & 0x08) == 0x08) 
			return(FM1702_CRCERR);
		temp = read_reg(0x04);
		if(temp != 1) 
			return(FM1702_BYTECOUNTERR);
		Read_FIFO(RfBuffer);	/* ´ÓFIFOÖÐ¶ÁÈ¡Ó¦´ðÐÅÏ¢ */
	 	temp = *RfBuffer;
        mifare_SAK=temp;		//write by  yzwguet
		/* ÅÐ¶ÏÓ¦´ðÐÅºÅÊÇ·ñÕýÈ· */          //change by hbp
		//if((temp & 0x20) == 0)				//´ËÎ»ÐèÒªÎª1
		//	return	15;
		if(temp & 0x04)		//UID»¹ÓÐÐèÒªÈ·ÈÏ
			return	5;
		else
			return	0;
			 
	}
}
void	RfReset(void)
{
	rftl = 0;			//TL ³¤¶È×Ö½Ú
	rft0 = 0; 			//¸ñÊ½×Ö·û,°üº¬ÊÇ·ñÓÐta1..tc1;fsci
	rffsci = 0;
	rfta1 = 0;			//ta1  ÖÐ¼ä»áÓÐrfds,rfdr ,b8Ö¸Ê¾Ë«ÏòÊý¾Ý´«ÊäÊÇ·ñÏàÍ¬,0Îª²»Í¬,b4Îª0 PCD¿ÉÒÔÀûÓÃPPSÑ¡Ôñ²»Í¬ÓÚ106µÄkbps
	rfds = 0;		//·¢ËÍÒò×Ó
	rfdr = 0;		//½ÓÊÕÒò×Ó
	rftb1 = 0;			//ta1  ÖÐ¼ä»áÓÐrffwi,rfsfgi
	rffwi = 4;		//Ö¡µÈ´ýÊ±¼äÕûÊý	   Ä¬ÈÏÊÇ71680¸öclk
	rfsfgi = 0;		//Æô¶¯Ö¡±£»¤Ê±¼äÕûÊý
	rftc1 = 0;			//ta1  ÊÇ·ñÖ§³Önad,cid
	rfnad = 0;		//0 Ê±²»Ö§³Ö£¬1Ê±Ö§³Ö
	rfcid = 0;		//0 Ê±²»Ö§³Ö£¬1Ê±Ö§³Ö
	fsclen = 32;	//¿¨Æ¬Ä¬ÈÏÄÜ½ÓÊÕµ½64bytes×Ö½ÚÊý¾Ý£¬×Ô¼ºÁÙÊ±ÈÏÎª
	return;	
}
/*******************************************/
//ÇåRCµÄ¶¨Ê±Æ÷ÖÐ¶Ï
/*******************************************/
void	resetrctim(void)
{
	u8	temp;

	temp = 	read_reg(0x07);
	write_reg(0x07,temp|0x20);		//Çå±êÖ¾
	return;
}
/*******************************************/
//wk,anti,select,halt,reqb  5.3ms
/*******************************************/
void	reqprotim(void)	   
{
	u8	temp,i;
	u32	toutls = 0;

	toutls = systicnum; 
	for(i = 0xff; i > 0; i --);
	temp = read_reg(0x09);
	write_reg(0x09,temp | 0x04);
	resetrctim();					//stop timer
	write_reg(TimerClock, 0x0a);	//1024*70=76180/fc=5.3ms	 config  
	write_reg(TimerReload, 0x46);
	//Á¢¼´Æô¶¯¶¨Ê±Æ÷,µÈµ½Í£Ö¹ 
	for(i = 0xff; i > 0; i --);	
	temp = read_reg(0x09);
	write_reg(0x09,temp | 0x02);	   //start
	//LedGreenOn();
	for(i = 0xff; i > 0; i --);					 
	//while((read_reg(0x07)&0x20) ==0);	   //LG²âÊÔµÃµ½µÄÊ±¼äÊÇ5.29ms
	temp = read_reg(0x07);
	while((temp&0x20) == 0)
	{
		temp = read_reg(0x07);
		if(systicnum > (toutls + 10))
		break;	
	}
	//LedGreenOff();						   //reset
	resetrctim();
	temp = read_reg(0x09);
	write_reg(0x09,temp | 0x04);		  //stop
	return;
}


u8 MIF_READ(u8  *buff, u8 Block_Adr)
{
	/*~~~~~~~~~~~~~~~~~*/
	u8  	temp;
	/*~~~~~~~~~~~~~~~~~*/
	/*
	write_reg(0x23,0x63);
	write_reg(0x12,0x3f);
	write_reg(0x13,0x3f);
	write_reg(0x22,0x0f);
	*/
	write_reg(0x22,0x0f);
	buff[0] = RF_CMD_READ;
	buff[1] = Block_Adr;
	temp = Command_Send(2, buff, Transceive);
	if(temp == FALSE)
	{
		return FM1702_NOTAGERR;							   
	}
	temp = read_reg(0x0A);
	if((temp & 0x02) == 0x02) return FM1702_PARITYERR;
	if((temp & 0x04) == 0x04) return FM1702_FRAMINGERR;
	//if((temp & 0x08) == 0x08) return FM1702_CRCERR;
	
	temp = read_reg(0x04);
	if(temp == 0x10) 
	{	
		Read_FIFO(buff);
		return FM1702_OK;
	}
	else if(temp == 0x04)	 
	{
		Read_FIFO(buff);
		return FM1702_OK;
	}
	else
	{
		return FM1702_BYTECOUNTERR;
	}	  
}

u8 MIF_Write(u8 *buff, u8 Block_Adr)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	u8 temp;
	u8 *F_buff;
	/*~~~~~~~~~~~~~~~~~~~~*/
	/*
	write_reg(0x23,0x63);
	write_reg(0x12,0x3f);
	write_reg(0x22,0x07);
	*/
	write_reg(0x22,0x07);
	F_buff = buff + 0x10;
	*F_buff = RF_CMD_WRITE;
	*(F_buff + 1) = Block_Adr;
	temp = Command_Send(2, F_buff, Transceive);
	if(temp == FALSE)
	{
		return(FM1702_NOTAGERR);
	}
	temp = read_reg(0x04);
	if(temp == 0)
	{
		return(FM1702_BYTECOUNTERR);
	}
	Read_FIFO(F_buff);
	temp = *F_buff;
	switch(temp)
	{
	case 0x00:	return(FM1702_NOTAUTHERR); 
	case 0x04:	return(FM1702_EMPTY);
	case 0x0a:	break;
	case 0x01:	return(FM1702_CRCERR);
	case 0x05:	return(FM1702_PARITYERR);
	default:	return(FM1702_WRITEERR);
	}

	temp = Command_Send(16, buff, Transceive);
	if(temp == TRUE)
	{
		return(FM1702_OK);
	}
	else
	{
		temp = read_reg(0x0A);
		if((temp & 0x02) == 0x02)
			return(FM1702_PARITYERR);
		else if((temp & 0x04) == 0x04)
			return(FM1702_FRAMINGERR);
		else if((temp & 0x08) == 0x08)
			return(FM1702_CRCERR);
		else
			return(FM1702_WRITEERR);
	} 
}

u8 MIF_Increment(u8 *buff, u8 Block_Adr)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	u8	temp;
	u8 	*F_buff;
	/*~~~~~~~~~~~~~~~~~~~~*/
	/*
	write_reg(0x23,0x63);
	write_reg(0x12,0x3f);
	write_reg(0x22,0x07);
	*/
	write_reg(0x22,0x07);
	F_buff = buff + 4;
	*F_buff = RF_CMD_INC;
	*(F_buff + 1) = Block_Adr;
	temp = Command_Send(2, F_buff, Transceive);
	if(temp == FALSE)
	{
		return FM1702_NOTAGERR;
	}
	temp = read_reg(0x04);
	if(temp == 0)
	{
		return FM1702_BYTECOUNTERR;
	}
	Read_FIFO(F_buff);
	temp = *F_buff;			//·µ»Ø0x0a
	switch(temp)
	{
		case 0x00:	return(FM1702_NOTAUTHERR);	 
		case 0x04:	return(FM1702_EMPTY);
		case 0x0a:	break;
		//case 0x01:	return(FM1702_CRCERR);
		case 0x05:	return(FM1702_PARITYERR);
		default:	return(FM1702_INCRERR);
	}
	temp = Command_Send(4, buff, Transmit);
	if(temp == FALSE)
	{
		return FM1702_INCRERR;
	}
	return FM1702_OK;	
}

u8 MIF_Decrement(u8 *buff, u8 Block_Adr)
{
	u8	temp;
	u8 	*F_buff;
	/*
	write_reg(0x23,0x63);
	write_reg(0x12,0x3f);
	write_reg(0x22,0x07);
	*/
	write_reg(0x22,0x07);
	F_buff = buff + 4;
	*F_buff = RF_CMD_DEC;
	*(F_buff + 1) = Block_Adr;
	temp = Command_Send(2, F_buff, Transceive);
	if(temp == FALSE)
	{
		return FM1702_NOTAGERR;
	}

	temp = read_reg(0x04);
	if(temp == 0)
	{
		return FM1702_BYTECOUNTERR;
	}
	Read_FIFO(F_buff);
	temp = *F_buff;
	switch(temp)
	{
		case 0x00:	return(FM1702_NOTAUTHERR);	// ÔÝÊ±ÆÁ±ÎµôÐ´´íÎó
		case 0x04:	return(FM1702_EMPTY);
		case 0x0a:	break;
		case 0x01:	return(FM1702_CRCERR);
		case 0x05:	return(FM1702_PARITYERR);
		default:	return(FM1702_DECRERR);
	}
	temp = Command_Send(4, buff, Transmit);
	if(temp == FALSE)
	{
		return(FM1702_DECRERR);
	}
	return FM1702_OK;
}
 
u8 MIF_Restore(u8 Block_Adr)
{
 	u8 temp,i;
	/*
 	write_reg(0x23,0x63);
	write_reg(0x12,0x3f);
	write_reg(0x22,0x07);
	*/
	write_reg(0x22,0x07);
	*ta_workingBuffer=RF_CMD_RESTORE;
	*(ta_workingBuffer+1)=Block_Adr;
	temp=Command_Send(2,ta_workingBuffer,Transceive);
	if(temp==FALSE)  return FM1702_NOTAGERR;
	temp = read_reg(0x04);
	if(temp==0)  return FM1702_BYTECOUNTERR;
	Read_FIFO(ta_workingBuffer);
	temp=*ta_workingBuffer;
	switch(temp)
	{
	  case 0x00:return (FM1702_NOTAUTHERR);
      case 0x04:return (FM1702_EMPTY);
	  case 0x0a:break;
	  case 0x01:return (FM1702_CRCERR);
	  case 0x05:return (FM1702_PARITYERR);
	  default:return (FM1702_RESTERR);
	}
	for(i=0;i<4;i++)
	   ta_workingBuffer[i]=0x00;
	temp=Command_Send(4,ta_workingBuffer,Transmit);
	if(temp==FALSE) return (FM1702_RESTERR);
	return FM1702_OK;
}
 
u8 MIF_Transfer(u8 Block_Adr)
{
	/*~~~~~~~~~*/
	u8	temp;
	/*~~~~~~~~~*/
	/*
	write_reg(0x23,0x63);
	write_reg(0x12,0x3f);
	write_reg(0x22,0x07);
	*/
	write_reg(0x22,0x07);
	ta_workingBuffer[0] = RF_CMD_TRANSFER;
	ta_workingBuffer[1] = Block_Adr;
	temp = Command_Send(2, ta_workingBuffer, Transceive);
	if(temp == FALSE)
	{
		return FM1702_NOTAGERR;
	}
	temp = read_reg(0x04);
	if(temp == 0)
	{
		return FM1702_BYTECOUNTERR;
	}
	Read_FIFO(ta_workingBuffer);
	temp = *ta_workingBuffer;
	switch(temp)
	{
		case 0x00:	return(FM1702_NOTAUTHERR); 
		case 0x04:	return(FM1702_EMPTY);
		case 0x0a:	return(FM1702_OK);
		case 0x01:	return(FM1702_CRCERR);
		case 0x05:	return(FM1702_PARITYERR);
		default:	return(FM1702_TRANSERR);
	}
}
void M500HostCodeKey(unsigned char *uncoded)   
{
//    char  status = 0;
    u8  cnt = 0,ln  = 0,hn  = 0;
	u8	i,saveu[6];

	for(i=0;i<6;i++)
	saveu[i]=uncoded[i];      
    
    for (cnt = 0; cnt < 6; cnt++)
    {
        ln = saveu[cnt] & 0x0F;
        hn = saveu[cnt] >> 4;
        buffe[cnt * 2 + 1] = (~ln << 4) | ln;
        buffe[cnt * 2 ] = (~hn << 4) | hn;
    }
	return;
}
u8 Load_key(u8 *uncoded_keys)
{
    u8 temp;
    
    M500HostCodeKey(uncoded_keys);  
	temp = Command_Send(12, buffe, LoadKey);
	if(temp==FALSE)	 
		return(FM1702_LOADKEYERR);
	temp = read_reg(0x0A) & 0x40;
	if (temp == 0x40)
	{
		return FM1702_NOTAGERR;		 //ÓÐÎÊÌâ  µ»Ø0£¬Õý³£·µ»Ø1
	}
	else
	return FM1702_OK;
}
u8 Authentication(u8 SecNR,u8 mode)
{  
	u8  i,temp,temp1;	//
	/*
	write_reg(0x23,0x63);
	write_reg(0x12,0x3f);
	write_reg(0x13,0x3f);
	temp1=read_reg(Control);temp1&=0xf7;write_reg(Control,temp1); *///Control=temp1;	 
	if(mode==0)
	ta_workingBuffer[0]=RF_CMD_AUTH_LA;
	else 
	ta_workingBuffer[0]=RF_CMD_AUTH_LB;
	if(SecNR<128)	 			//s50Óës70Ç°32¸öÉÈÇø
		ta_workingBuffer[1]=(SecNR/4)*4+3;
	else	if((SecNR>127)&&(SecNR<256))   //s70ºó8¸öÉÈÇø
		ta_workingBuffer[1]=16*((SecNR-128)/16+1)+127;
	else	return	FM1702_EMPTY;
	//	sendbyte(buffer[1]);while(1);	
	for(i=0;i<4;i++)
		ta_workingBuffer[2+i]=UID[i];
	write_reg(0x22,0x0f);	 //ChannelRedundancy=0x0f;
	temp=Command_Send(6,ta_workingBuffer,Authent1);
	if(temp==FALSE)
		return FM1702_NOTAGERR;
	temp=read_reg(ErrorFlag);  
	//if((temp&0x02)==0x02) return FM1702_PARITYERR;
	if((temp&0x04)==0x04) return FM1702_FRAMINGERR;  //return 0;
	//if((temp&0x08)==0x08) return FM1702_CRCERR;
	
	temp=Command_Send(0,ta_workingBuffer,Authent2);
	if(temp==FALSE) return FM1702_NOTAGERR;
		temp=read_reg(ErrorFlag);
	//if((temp&0x02)==0x02) return FM1702_PARITYERR;
	if((temp&0x04)==0x04) return FM1702_FRAMINGERR;
	//if((temp&0x08)==0x08) return FM1702_CRCERR;
	temp1=read_reg(Control);temp1&=0x08;
	if(temp1==0x08) return FM1702_OK;		  
		return FM1702_AUTHERR;	
}  

u16 GetRFStatus(void)
{
	uint8_t tempSend[2];
	tempSend[0] = 0xb2;
	tempSend[1] = 0x00;
	if(Command_Send(0x01 , &tempSend[0] , Transceive) == 0x00)// if Command_Send£¨£© return 0 means that this function excution failed.
	{
		tempSend[0] = 0xb2;
		tempSend[1] = 0x00;
		if(Command_Send(0x01 , &tempSend[0] , Transceive) == 0x00) return 1;//there is not a rf card
	}
	return 0;//card is there
}
