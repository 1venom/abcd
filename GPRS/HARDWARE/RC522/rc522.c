#include "stm32f10x_gpio.h"
#include "rc522.h"
#include "io.h"




char Status = MI_NOP;
unsigned char  RevSection_Buf[4][16], SPRecData_Buf[16], RecType_Buf[2];
unsigned char Relay_Stat=0, RF_Stat=0, GPRS_Stat=0, LocalInformation_Flag=0, RetStat_Flag = 0;

//unsigned char  Section2_Pass_Buf[6] = {0x17, 0xBA, 0xBE, 0xDF, 0xD9, 0x5D};
unsigned char Section2_Pass_Buf[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

unsigned char  RoomNumber_Buf[3];//={0x00,0x00,0x00};
unsigned char  HotelTag_Buf[3];//={0x8a,0x5f,0xbb};
unsigned int Relay_Delay_Cnt,System_Delay_Cnt;				//延时时间参数，最大延时时间变量类型最大范围*定时时间S=延时时间S
struct{
	unsigned int Type;
	unsigned char UID[4];
}RFID;

char PcdRequest(unsigned char req_code,unsigned char *pTagType);
char PcdAnticoll(unsigned char *pSnr);
char PcdSelect(unsigned char *pSnr);
char PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr);
char PcdRead(unsigned char addr,unsigned char *pData);
char PcdComMF522(unsigned char Command,
                 unsigned char *pInData,
                 unsigned char InLenByte,
                 unsigned char *pOutData,
                 unsigned int  *pOutLenBit);
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData);
char M500PcdConfigISOType(unsigned char type);
void WriteRawRC(unsigned char Address, unsigned char value);
unsigned char ReadRawRC(unsigned char Address);
void SetBitMask(unsigned char reg,unsigned char mask);
void ClearBitMask(unsigned char reg,unsigned char mask) ;
char PcdReset(void);
void PcdAntennaOn(void);
void PcdAntennaOff(void);

extern unsigned char Card_Sendevent;
extern unsigned char ONCard_Sendevent;
//extern  uint8_t Relay_Stat;
//extern  uint8_t RF_Stat;					 
								 
void os_delay_us(u16 i)
{
	u16 j=0;
	for(j=0;j<2;j++)
	{
		for(i=0;i<=0;i--)
		{
			;
		}
	}
									 
}



/////////////////////////////////////////////////////////////////////
//功    能：寻卡
//参数说明: req_code[IN]:寻卡方式
//                0x52 = 寻感应区内所有符合14443A标准的卡
//                0x26 = 寻未进入休眠状态的卡
//          pTagType[OUT]：卡片类型代码
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRequest(unsigned char req_code,unsigned char *pTagType)
{
   char status;
   unsigned int  unLen;
   unsigned char ucComMF522Buf[MAXRLEN];
//  unsigned char xTest ;
   ClearBitMask(Status2Reg,0x08);				 //Clear MFCrypto1On
   WriteRawRC(BitFramingReg,0x07);

//  xTest = ReadRawRC(BitFramingReg);
//  if(xTest == 0x07 )
 //   { LED_GREEN  =0 ;}
 // else {LED_GREEN =1 ;while(1){}}
   SetBitMask(TxControlReg,0x03);			 //Open TXxRFEn

   ucComMF522Buf[0] = req_code;

   status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);
 //  os_printf("======== %u=========\n\n\n\n",status);
//     if(status  == MI_OK )
//   { LED_GREEN  =0 ;}
//   else {LED_GREEN =1 ;}
   if ((status == MI_OK) && (unLen == 0x10))
   {
       *pTagType     = ucComMF522Buf[0];
       *(pTagType+1) = ucComMF522Buf[1];
   }
   else
   {   status = MI_ERR;   }

   return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：防冲撞
//参数说明: pSnr[OUT]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdAnticoll(unsigned char *pSnr)
{
    char status;
    unsigned char i,snr_check=0;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];


    ClearBitMask(Status2Reg,0x08);
    WriteRawRC(BitFramingReg,0x00);
    ClearBitMask(CollReg,0x80);					//清除发生冲突的位

    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if (status == MI_OK)
    {
    	 for (i=0; i<4; i++)
         {
             *(pSnr+i)  = ucComMF522Buf[i];
             snr_check ^= ucComMF522Buf[i];
         }
         if (snr_check != ucComMF522Buf[i])
         {   status = MI_ERR;    }
    }

    SetBitMask(CollReg,0x80);
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：选定卡片
//参数说明: pSnr[IN]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdSelect(unsigned char *pSnr)
{
    char status;
    unsigned char i;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++)
    {
    	ucComMF522Buf[i+2] = *(pSnr+i);
    	ucComMF522Buf[6]  ^= *(pSnr+i);
    }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);

    ClearBitMask(Status2Reg,0x08);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);

    if ((status == MI_OK) && (unLen == 0x18))
    {   status = MI_OK;  }
    else
    {   status = MI_ERR;    }

    return status;
}


char PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    for (i=0; i<6; i++)
    {    ucComMF522Buf[i+2] = *(pKey+i);   }
    for (i=0; i<6; i++)
    {    ucComMF522Buf[i+8] = *(pSnr+i);   }
 //   memcpy(&ucComMF522Buf[2], pKey, 6);
 //   memcpy(&ucComMF522Buf[8], pSnr, 4);

    status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
    {   status = MI_ERR;   }

    return status;
}


char PcdRead(unsigned char addr,unsigned char *pData)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if ((status == MI_OK) && (unLen == 0x90))
 //   {   memcpy(pData, ucComMF522Buf, 16);   }
    {
        for (i=0; i<16; i++)
        {    *(pData+i) = ucComMF522Buf[i];   }
    }
    else
    {   status = MI_ERR;   }

    return status;
}

char PcdComMF522(unsigned char Command,
                 unsigned char *pInData,
                 unsigned char InLenByte,
                 unsigned char *pOutData,
                 unsigned int  *pOutLenBit)
{
    char status = MI_ERR;
    unsigned char irqEn   = 0x00;
    unsigned char waitFor = 0x00;
    unsigned char lastBits;
    unsigned char n;
    unsigned int i;
    switch (Command)
    {
       case PCD_AUTHENT:		 //验证密钥
          irqEn   = 0x12;
          waitFor = 0x10;
          break;
       case PCD_TRANSCEIVE:		//发送并接收数据
          irqEn   = 0x77;
          waitFor = 0x30;
          break;
       default:
         break;
    }

    WriteRawRC(ComIEnReg,irqEn|0x80);			//Set IRQ
    ClearBitMask(ComIrqReg,0x80);
    WriteRawRC(CommandReg,PCD_IDLE);			//取消当前命令
    SetBitMask(FIFOLevelReg,0x80);				//Clear FlushBuffer Bit

    for (i=0; i<InLenByte; i++)
    {   WriteRawRC(FIFODataReg, pInData[i]);    }
    WriteRawRC(CommandReg, Command);			 //发送并接收数据


    if (Command == PCD_TRANSCEIVE)
    {    SetBitMask(BitFramingReg,0x80);  }	 //启动数据的发送，与收发命令一起使用有效

//    i = 600;//根据时钟频率调整，操作M1卡最大等待时间25ms
		i = 6000;
    do
    {
         n = ReadRawRC(ComIrqReg);
         i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitFor));	 //等待发送完毕
    ClearBitMask(BitFramingReg,0x80);

    if (i!=0)
    {
         if(!(ReadRawRC(ErrorReg)&0x1B))
         {
             status = MI_OK;
             if (n & irqEn & 0x01)
             {   status = MI_NOTAGERR;   }
             if (Command == PCD_TRANSCEIVE)
             {
               	n = ReadRawRC(FIFOLevelReg);			//读取FIFO缓冲区中保存的字节数
              	lastBits = ReadRawRC(ControlReg) & 0x07;
                if (lastBits)
                {   *pOutLenBit = (n-1)*8 + lastBits;   }
                else
                {   *pOutLenBit = n*8;   }
                if (n == 0)
                {   n = 1;    }
                if (n > MAXRLEN)
                {   n = MAXRLEN;   }
                for (i=0; i<n; i++)
                {   pOutData[i] = ReadRawRC(FIFODataReg);    }
            }
         }
         else
         {   status = MI_ERR;   }

   }


   SetBitMask(ControlReg,0x80);           // stop timer now
   WriteRawRC(CommandReg,PCD_IDLE);
   return status;
}

/////////////////////////////////////////////////////////////////////
//用MF522计算CRC16函数
/////////////////////////////////////////////////////////////////////
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData)
{
    unsigned char i,n;
    ClearBitMask(DivIrqReg,0x04);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);		//清除FlushBuffer,写指针及寄存器ErrReg的BufferOvfl标志被清除
    for (i=0; i<len; i++)
    {   WriteRawRC(FIFODataReg, *(pIndata+i));   }
    WriteRawRC(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do
    {
        n = ReadRawRC(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));
    pOutData[0] = ReadRawRC(CRCResultRegL);
    pOutData[1] = ReadRawRC(CRCResultRegM);
}

//////////////////////////////////////////////////////////////////////
//脡猫脰脙RC632碌脛鹿陇脳梅路陆脢陆
//////////////////////////////////////////////////////////////////////
char M500PcdConfigISOType(unsigned char type)
{
   if (type == 'A')                     //ISO14443_A
   {
       ClearBitMask(Status2Reg,0x08);


       WriteRawRC(ModeReg,0x3D);//3F

       WriteRawRC(RxSelReg,0x86);//84

       WriteRawRC(RFCfgReg,0x7F);   //4F

   	   WriteRawRC(TReloadRegL,30);//tmoLength);// TReloadVal = 'h6a =tmoLength(dec)
	   WriteRawRC(TReloadRegH,0);
       WriteRawRC(TModeReg,0x8D);
	   WriteRawRC(TPrescalerReg,0x3E);
  //     PcdSetTmo(106);
	   os_delay_us(1);
       PcdAntennaOn();
   }
   else{ return MI_NOTAGERR; }

   return MI_OK;
}


/////////////////////////////////////////////////////////////////////
//鹿娄    脛脺拢潞脨麓RC632录脛麓忙脝梅
//虏脦脢媒脣碌脙梅拢潞Address[IN]:录脛麓忙脝梅碌脴脰路
//          value[IN]:脨麓脠毛碌脛脰碌
/////////////////////////////////////////////////////////////////////

void WriteRawRC(unsigned char Address, unsigned char value)
{
    unsigned char  i, ucAddr;

    os_delay_us(1);
    MF522_SCK_0;
    os_delay_us(1);
    MF522_NSS_0;
    os_delay_us(1);
    ucAddr = ((Address<<1)&0x7E);

    for(i=8;i>0;i--)
    {
    	if((ucAddr&0x80)==0x80)
    		MF522_SI_1;
    	else
    		MF522_SI_0;

    	os_delay_us(1);

        MF522_SCK_1;
        os_delay_us(1);
        ucAddr <<= 1;
        MF522_SCK_0;
        os_delay_us(1);
    }

    for(i=8;i>0;i--)
    {
    	  os_delay_us(1);
    	if((value&0x80)==0x80)
    		MF522_SI_1;
    	else
    		MF522_SI_0;
    	  os_delay_us(1);
        MF522_SCK_1;
        os_delay_us(1);
        value <<= 1;
        MF522_SCK_0;
        os_delay_us(1);
    }
    os_delay_us(1);
    MF522_NSS_1;
    os_delay_us(1);
    MF522_SCK_1;
}

/////////////////////////////////////////////////////////////////////
//鹿娄    脛脺拢潞露脕RC632录脛麓忙脝梅
//虏脦脢媒脣碌脙梅拢潞Address[IN]:录脛麓忙脝梅碌脴脰路
//路碌    禄脴拢潞露脕鲁枚碌脛脰碌
/////////////////////////////////////////////////////////////////////
unsigned char ReadRawRC(unsigned char Address)
{
     unsigned char i,ucAddr;
     unsigned char ucResult=0;

     os_delay_us(1);
     MF522_SCK_0;
     os_delay_us(1);
     MF522_NSS_0;
     os_delay_us(1);
     ucAddr = ((Address<<1)&0x7E)|0x80;

     for(i=8;i>0;i--)
     {
    	  os_delay_us(1);
     	if((ucAddr&0x80)==0x80)
     		MF522_SI_1;
     	else
     		MF522_SI_0;
     	  os_delay_us(1);
         MF522_SCK_1;
         os_delay_us(1);
         ucAddr <<= 1;
         os_delay_us(1);
         MF522_SCK_0;
         os_delay_us(1);
     }

     for(i=8;i>0;i--)
     {
         MF522_SCK_1;
         os_delay_us(1);
         ucResult <<= 1;
         ucResult|=MF522_SO;
         os_delay_us(1);
         MF522_SCK_0;
         os_delay_us(1);
     }

     MF522_NSS_1;
     os_delay_us(1);
     MF522_SCK_1;
     os_delay_us(1);
     return ucResult;
}

/////////////////////////////////////////////////////////////////////
//鹿娄    脛脺拢潞脰脙RC522录脛麓忙脝梅脦禄
//虏脦脢媒脣碌脙梅拢潞reg[IN]:录脛麓忙脝梅碌脴脰路
//          mask[IN]:脰脙脦禄脰碌
/////////////////////////////////////////////////////////////////////
void SetBitMask(unsigned char reg,unsigned char mask)
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg,tmp | mask);  // set bit mask
}

/////////////////////////////////////////////////////////////////////
//鹿娄    脛脺拢潞脟氓RC522录脛麓忙脝梅脦禄
//虏脦脢媒脣碌脙梅拢潞reg[IN]:录脛麓忙脝梅碌脴脰路
//          mask[IN]:脟氓脦禄脰碌
/////////////////////////////////////////////////////////////////////
void ClearBitMask(unsigned char reg,unsigned char mask)
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask);  // clear bit mask
}

/////////////////////////////////////////////////////////////////////
//鹿娄    脛脺拢潞赂麓脦禄RC522
//路碌    禄脴: 鲁脡鹿娄路碌禄脴MI_OK
/////////////////////////////////////////////////////////////////////
char PcdReset(void)
{  

    MF522_RST_1;
    os_delay_us(100);
    MF522_RST_0;
    os_delay_us(100);
    MF522_RST_1;
    os_delay_us(100);
    WriteRawRC(CommandReg,PCD_RESETPHASE);
    os_delay_us(100);

    WriteRawRC(ModeReg,0x3D);            //潞脥Mifare驴篓脥篓脩露拢卢CRC鲁玫脢录脰碌0x6363

    WriteRawRC(TReloadRegL,30);
    WriteRawRC(TReloadRegH,0);
    WriteRawRC(TModeReg,0x8D);
    WriteRawRC(TPrescalerReg,0x3E);
    WriteRawRC(TxAutoReg,0x40);
    return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//驴陋脝么脤矛脧脽
//脙驴麓脦脝么露炉禄貌鹿脴卤脮脤矛脧脮路垄脡盲脰庐录盲脫娄脰脕脡脵脫脨1ms碌脛录盲赂么
/////////////////////////////////////////////////////////////////////
void PcdAntennaOn()
{
    unsigned char i;
    i = ReadRawRC(TxControlReg);
    if (!(i & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
}

/////////////////////////////////////////////////////////////////////
//鹿脴卤脮脤矛脧脽
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff()
{
    ClearBitMask(TxControlReg, 0x03);
}


void InitRc522(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;

		RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA, ENABLE );//PORTB时钟使能 


		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	 //IO-->PF0、PF1 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化PF0、PF1
    GPIO_ResetBits(GPIOA,GPIO_Pin_8);			             //PF1输出低
    //GPIO_SetBits(GPIOF,GPIO_Pin_0);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	 //IO-->PF0、PF1 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化PF0、PF1
    GPIO_ResetBits(GPIOB,GPIO_Pin_12);			             //PF1输出低
    //GPIO_SetBits(GPIOF,GPIO_Pin_0);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//GPIO_Mode_AF_PP;  //PB13/14/15复用推挽输出 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB
	
	
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //PB13/14/15复用推挽输出 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB
	
	
	
	PcdReset();
	PcdAntennaOff();
	os_delay_us(1);
	PcdAntennaOn();
	Status = M500PcdConfigISOType('A');
}



void readCard(void)
{
	static u8 initflag = 0;
	u8 i = 0;
//	LocalInformation_Flag = 1;
	if(initflag==0)
	{
		InitRc522();
		initflag = 1;		
	}
		for(i=0;i<3;i++)
		{																										//为保证读卡成功，循环读取三次
			Status = PcdRequest(0x52, RecType_Buf);		    	//14443A
			if(Status == MI_OK)
				break;
		}
		if((Status == MI_OK) && (RF_Stat==0))							//搜索完卡片，判断卡片是否有效卡片
		{
			if(LocalInformation_Flag == 1)
			{
				Status = PcdAnticoll(RFID.UID);				     	  //防冲撞
				
				if(Status == MI_OK)
				{
					PcdSelect(RFID.UID);													//选择卡片
					Status = PcdAuthState(PICC_AUTHENT1A, 0x14, Section2_Pass_Buf, RFID.UID);
					if(Status == MI_OK)
					{
						for(i=0;i<4;i++)
						{
							Status = PcdRead(0x14+i, RevSection_Buf[i]);
							if(Status == MI_OK)
							{
								Status = MI_NOP;
							}
						}
						if(i>=3)
						{
//							if((RevSection_Buf[0][0]==HotelTag_Buf[0])&&(RevSection_Buf[0][1]==HotelTag_Buf[1])&&(RevSection_Buf[0][2]==HotelTag_Buf[2]))			//判断系统密码是否正确
//							{
//								RevSection_Buf[0][5] = RevSection_Buf[0][5] & 0x07;			//楼号：如果是客房系统，则高5位为清洁标志，低3位为楼号
//								
//								switch(RevSection_Buf[0][3])
//								{
//									case 0x0A:				//客房设置卡，需要判断房间号
//									{
//										if((RevSection_Buf[0][5]==RoomNumber_Buf[0]) && (RevSection_Buf[0][6]==RoomNumber_Buf[1]) && (RevSection_Buf[0][7]==RoomNumber_Buf[2]))
//										{
//												RF_LED = 0;
//												RF_Stat = 1;
//												Relay_LED = 0;
//												RelayOUT = 1;
//												Relay_Stat = 1;
//												
//										}
//										break;
//									}
//									case 0x0D:				//客人卡，需要判断房间号
//									{
//										if((RevSection_Buf[0][5]==RoomNumber_Buf[0]) && (RevSection_Buf[0][6]==RoomNumber_Buf[1]) && (RevSection_Buf[2][0]==RoomNumber_Buf[2]))
//										{
//												RF_LED = 0;
//												RF_Stat = 1;
//												Relay_LED = 0;
//												RelayOUT = 1;
//												Relay_Stat = 1;

//										}
//										break;
//									}
//									case 0x0C:				//总控卡，可取所有房间的电
//									{
//												RF_LED = 0;
//												RF_Stat = 1;
//												Relay_LED = 0;
//												RelayOUT = 1;
//												Relay_Stat = 1;
//										break;
//									}
//									default:	break; //Power_RF_ON();
//								}
//							}
                            if((RevSection_Buf[1][12]==HotelTag_Buf[0])&&(RevSection_Buf[1][13]==HotelTag_Buf[1])&&(RevSection_Buf[1][14]==HotelTag_Buf[2]))			//判断酒店标识码是否正确
                            {
                                switch(RevSection_Buf[0][0])							//识别卡片类型
                                {
                                    case 0x01:
                                    {
                                        if((RevSection_Buf[0][4] == RoomNumber_Buf[0])&&(RevSection_Buf[0][5] == RoomNumber_Buf[1])&&(RevSection_Buf[0][6] == RoomNumber_Buf[2]))
                                        {
											Power_RF_ON();
                                        }
                                        break;
                                    }
                                    case 0x02:
                                    {
										Power_RF_ON();
                                        break;
                                    }
                                    case 0x03:
                                    {
										Power_RF_ON();
                                        break;
                                    }
                                    case 0x04:
                                    {
										Power_RF_ON();
                                        break;
                                    }
                                    case 0x05:
                                    {
                                        Power_RF_ON();
                                        break;
                                    }
                                }
                            }

						}
						
					}

				}
			}else
			{
					Card_Sendevent = 1;
					Power_RF_ON();
					
			}
			
		}else if((Status != MI_OK) && (RF_Stat==1))
		{
			RF_LED = 1;
			RF_Stat = 0;
			if((Relay_Stat==1) && (GPRS_Stat==0))
			{
				Relay_Delay_Cnt = 250;
			}
			ONCard_Sendevent=1;
		}
}
void Power_RF_ON(void)
{
    if(RF_Stat!=1)
    {
       Card_Sendevent = 1; 
    }
    RF_LED = 0;
    RF_Stat = 1;
    Relay_LED = 0;
    RelayOUT = 1;
    Relay_Stat = 1;
}

