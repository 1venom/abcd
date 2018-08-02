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
unsigned int Relay_Delay_Cnt,System_Delay_Cnt;				//��ʱʱ������������ʱʱ������������Χ*��ʱʱ��S=��ʱʱ��S
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
//��    �ܣ�Ѱ��
//����˵��: req_code[IN]:Ѱ����ʽ
//                0x52 = Ѱ��Ӧ�������з���14443A��׼�Ŀ�
//                0x26 = Ѱδ��������״̬�Ŀ�
//          pTagType[OUT]����Ƭ���ʹ���
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ�����ײ
//����˵��: pSnr[OUT]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdAnticoll(unsigned char *pSnr)
{
    char status;
    unsigned char i,snr_check=0;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];


    ClearBitMask(Status2Reg,0x08);
    WriteRawRC(BitFramingReg,0x00);
    ClearBitMask(CollReg,0x80);					//���������ͻ��λ

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
//��    �ܣ�ѡ����Ƭ
//����˵��: pSnr[IN]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
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
       case PCD_AUTHENT:		 //��֤��Կ
          irqEn   = 0x12;
          waitFor = 0x10;
          break;
       case PCD_TRANSCEIVE:		//���Ͳ���������
          irqEn   = 0x77;
          waitFor = 0x30;
          break;
       default:
         break;
    }

    WriteRawRC(ComIEnReg,irqEn|0x80);			//Set IRQ
    ClearBitMask(ComIrqReg,0x80);
    WriteRawRC(CommandReg,PCD_IDLE);			//ȡ����ǰ����
    SetBitMask(FIFOLevelReg,0x80);				//Clear FlushBuffer Bit

    for (i=0; i<InLenByte; i++)
    {   WriteRawRC(FIFODataReg, pInData[i]);    }
    WriteRawRC(CommandReg, Command);			 //���Ͳ���������


    if (Command == PCD_TRANSCEIVE)
    {    SetBitMask(BitFramingReg,0x80);  }	 //�������ݵķ��ͣ����շ�����һ��ʹ����Ч

//    i = 600;//����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
		i = 6000;
    do
    {
         n = ReadRawRC(ComIrqReg);
         i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitFor));	 //�ȴ��������
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
               	n = ReadRawRC(FIFOLevelReg);			//��ȡFIFO�������б�����ֽ���
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
//��MF522����CRC16����
/////////////////////////////////////////////////////////////////////
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData)
{
    unsigned char i,n;
    ClearBitMask(DivIrqReg,0x04);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);		//���FlushBuffer,дָ�뼰�Ĵ���ErrReg��BufferOvfl��־�����
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
//ÉèÖÃRC632µÄ¹¤×÷·½Ê½
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
//¹¦    ÄÜ£ºÐ´RC632¼Ä´æÆ÷
//²ÎÊýËµÃ÷£ºAddress[IN]:¼Ä´æÆ÷µØÖ·
//          value[IN]:Ð´ÈëµÄÖµ
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
//¹¦    ÄÜ£º¶ÁRC632¼Ä´æÆ÷
//²ÎÊýËµÃ÷£ºAddress[IN]:¼Ä´æÆ÷µØÖ·
//·µ    »Ø£º¶Á³öµÄÖµ
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
//¹¦    ÄÜ£ºÖÃRC522¼Ä´æÆ÷Î»
//²ÎÊýËµÃ÷£ºreg[IN]:¼Ä´æÆ÷µØÖ·
//          mask[IN]:ÖÃÎ»Öµ
/////////////////////////////////////////////////////////////////////
void SetBitMask(unsigned char reg,unsigned char mask)
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg,tmp | mask);  // set bit mask
}

/////////////////////////////////////////////////////////////////////
//¹¦    ÄÜ£ºÇåRC522¼Ä´æÆ÷Î»
//²ÎÊýËµÃ÷£ºreg[IN]:¼Ä´æÆ÷µØÖ·
//          mask[IN]:ÇåÎ»Öµ
/////////////////////////////////////////////////////////////////////
void ClearBitMask(unsigned char reg,unsigned char mask)
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask);  // clear bit mask
}

/////////////////////////////////////////////////////////////////////
//¹¦    ÄÜ£º¸´Î»RC522
//·µ    »Ø: ³É¹¦·µ»ØMI_OK
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

    WriteRawRC(ModeReg,0x3D);            //ºÍMifare¿¨Í¨Ñ¶£¬CRC³õÊ¼Öµ0x6363

    WriteRawRC(TReloadRegL,30);
    WriteRawRC(TReloadRegH,0);
    WriteRawRC(TModeReg,0x8D);
    WriteRawRC(TPrescalerReg,0x3E);
    WriteRawRC(TxAutoReg,0x40);
    return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//¿ªÆôÌìÏß
//Ã¿´ÎÆô¶¯»ò¹Ø±ÕÌìÏÕ·¢ÉäÖ®¼äÓ¦ÖÁÉÙÓÐ1msµÄ¼ä¸ô
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
//¹Ø±ÕÌìÏß
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff()
{
    ClearBitMask(TxControlReg, 0x03);
}


void InitRc522(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;

		RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA, ENABLE );//PORTBʱ��ʹ�� 


		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	 //IO-->PF0��PF1 �˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��PF0��PF1
    GPIO_ResetBits(GPIOA,GPIO_Pin_8);			             //PF1�����
    //GPIO_SetBits(GPIOF,GPIO_Pin_0);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	 //IO-->PF0��PF1 �˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��PF0��PF1
    GPIO_ResetBits(GPIOB,GPIO_Pin_12);			             //PF1�����
    //GPIO_SetBits(GPIOF,GPIO_Pin_0);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//GPIO_Mode_AF_PP;  //PB13/14/15����������� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB
	
	
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //PB13/14/15����������� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB
	
	
	
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
		{																										//Ϊ��֤�����ɹ���ѭ����ȡ����
			Status = PcdRequest(0x52, RecType_Buf);		    	//14443A
			if(Status == MI_OK)
				break;
		}
		if((Status == MI_OK) && (RF_Stat==0))							//�����꿨Ƭ���жϿ�Ƭ�Ƿ���Ч��Ƭ
		{
			if(LocalInformation_Flag == 1)
			{
				Status = PcdAnticoll(RFID.UID);				     	  //����ײ
				
				if(Status == MI_OK)
				{
					PcdSelect(RFID.UID);													//ѡ��Ƭ
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
//							if((RevSection_Buf[0][0]==HotelTag_Buf[0])&&(RevSection_Buf[0][1]==HotelTag_Buf[1])&&(RevSection_Buf[0][2]==HotelTag_Buf[2]))			//�ж�ϵͳ�����Ƿ���ȷ
//							{
//								RevSection_Buf[0][5] = RevSection_Buf[0][5] & 0x07;			//¥�ţ�����ǿͷ�ϵͳ�����5λΪ����־����3λΪ¥��
//								
//								switch(RevSection_Buf[0][3])
//								{
//									case 0x0A:				//�ͷ����ÿ�����Ҫ�жϷ����
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
//									case 0x0D:				//���˿�����Ҫ�жϷ����
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
//									case 0x0C:				//�ܿؿ�����ȡ���з���ĵ�
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
                            if((RevSection_Buf[1][12]==HotelTag_Buf[0])&&(RevSection_Buf[1][13]==HotelTag_Buf[1])&&(RevSection_Buf[1][14]==HotelTag_Buf[2]))			//�жϾƵ��ʶ���Ƿ���ȷ
                            {
                                switch(RevSection_Buf[0][0])							//ʶ��Ƭ����
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

