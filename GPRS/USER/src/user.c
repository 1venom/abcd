#include "User.h"
#include "io.h"
#include "stm32f10x.h"
#include "string.h"
#include "stdio.h"
#include "modbus.h"
#include "stmflash.h"
#include "mqtt.h"
#include <json.h>
#include <stdlib.h>
#include "rc522.h"

uint8_t ReadFlag,WriteFlag;

int step = 0;
char flag = 0;
char reflag = 0;
char OldRF_Stat = 0;

extern void iap_load_app(u32 appxaddr);
static unsigned char Number[20]={0};                  
static unsigned char NumCount[20] = {0};              


static unsigned char online = 0;

unsigned char CREG[8]={0};
unsigned char Card_Sendevent = 0;//有卡是否上报数据，1有上报事件，0没有上报事件或上报完成
unsigned char ONCard_Sendevent=0;
unsigned char Char_Hex_flag = 0;
unsigned char Hex_char_flag = 0;
unsigned char Read_card = 0;
unsigned char Semaphore=0,waitSemaphore=0,restart=0;
unsigned char Version=0;
unsigned char GPRS=0;
unsigned char mycpuid[32],IDstring[2];




u8 Find(char *a)
{ 
	if(strstr((const char*)ucUart2Buff,a)!=NULL)
	    return 1;
    else
		return 0;
}

unsigned char check_signal = 0;
void Key(void)
{
	static unsigned char wait_time;
//	unsigned char signal[2];
//	int char_to_signal;
	unsigned char Clear_flash[32];
	unsigned char i;
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)==0)
	{
		wait_time+=uc100ms;
	}
  if(wait_time >= 10)
	{
		for(i=0;i<32;i++)
		{
		Clear_flash[i] = 0;
		}
	  Flash_Write(0x0800fc00, Clear_flash, 32);	
    Flash_Write(0x0800fc20, Clear_flash, 32);
    Flash_Write(0x0800ffc0, Clear_flash, 32);	
	 	wait_time =0;	
	 __disable_fault_irq();   
    NVIC_SystemReset();
	}		
}


/*******************************************************************************
* 函数名 : Second_AT_Command
* 描述   : 发送AT指令函数
* 输入   : 发送数据的指针
*******************************************************************************/

unsigned int Second_AT_Command(unsigned char *b,char *a,int count)         
{

	if(Number[count] == 0)
	{			
		memset((void*)ucUart2Buff, 0, sizeof(ucUart2Buff));
		Uart2SendString((char *)b);
		Number[count] = 1;
	}

	NumCount[count]+=uc100ms;                     
	if(NumCount[count] >= 20)                   
	{
		NumCount[count] = 0;
		if(count!=9) Number[count] = 0;
	}
	else if(NumCount[count] < 3)
	{
		return 0;
	}
					
	if(Find(a)==1)
	{
		Number[count] = 2;
    memset((void*)ucUart2SendBuff, 0, sizeof(ucUart2SendBuff));
		return 1;               //正确，则返回1
	}
	else
		return 0;               //不正确，则返回0

		
}


/*******************************************************************************
* 函数名 : GPRS_Config
* 描述   : GPRS配置
*******************************************************************************/

void GPRS_Config(unsigned char mode)                     
{
	static unsigned char wait = 0;
	unsigned char i=0;
	unsigned char sedbuffer[100] ={0};
  unsigned char deviceSecret[32]={0};
  unsigned char productKey[32]={0};
  unsigned char flash[32] = {"Dianrui"};
  char *p;

	if(mode==1)
	{
        switch(step)
        {
            case 0:
							if(wait == 0)
							{
                if(Second_AT_Command("AT\r\n","OK",1)==1)      
								wait = 1;
                
							}
								 if(wait>=15)
							{
               step = 1;	
							}
							if(Find("+PBREADY")==1)	
							{								
							step = 1;
							}
							else wait+=uc1s;
                break;
            case 1:
                if(Second_AT_Command("ATE0\r\n","OK",2)==1)    
                step = 2;
                break;
            case 2:
                if(Second_AT_Command("AT+CLOUDDISCONN\r\n","OK",3)==1)         
                    step = 3;
                break;	
            case 3:

                if(Second_AT_Command("AT+CREG?\r\n","+CREG",4)==1)
                {
                    if(Find("0,0")==1)
                    {
                        step = 3;
                    }
                    else 
                        step = 4;
                }
                break;			
            case 4:
                if(Second_AT_Command(MqttParameter.Cloudhdauth,"+CLOUDHDAUTH: OK",5)==1)  
                    step = 5;
                break;	
            case 5:
                if(Second_AT_Command("AT+CLOUDCONN=60,0,4\r\n","OK",6)==1)
                    step = 6;
                break;
            case 6:
                if(Second_AT_Command(MqttParameter.SubscribeTopic,"+CLOUDSUBACK: <OK>",7)==1)  
                {				
                    step = 7;
									  online=1;
									  LED2G=0;//Version=1;
                    for(i=5;i<12;i++)
                    {
                        NumCount[i]=0;
                        Number[i] = 0;
                    }                
                }
                break;
            default :break;
        }
    }        
	else if(mode==2)
	{
		 switch(step)
		 {
			case 0:
							if(wait == 0)
							{
                if(Second_AT_Command("AT\r\n","OK",1)==1)      
								wait = 1;
                
							}
								 if(wait>=15)
							{
               step = 1;	
							}
							if(Find("+PBREADY")==1)	
							{								
							step = 1;
							}
							else wait+=uc1s;
				break;
			case 1:
				if(Second_AT_Command("ATE0\r\n","OK",2)==1)   
					step = 2;
				break;						
			case 2:
				
					for(i=0;i<6;i++)
					{	
	
						sprintf((char*)IDstring,"%X",((*(uint16_t*)(0x1ffff7e8+2*i))>>4)&0x0f);
						mycpuid[0+4*i] = IDstring[0];
						sprintf((char*)IDstring,"%X",((*(uint16_t*)(0x1ffff7e8+2*i))>>0)&0x0f);
						mycpuid[1+4*i] = IDstring[0];					
						sprintf((char*)IDstring,"%X",((*(uint16_t*)(0x1ffff7e8+2*i))>>12)&0x0f);
						mycpuid[2+4*i] = IDstring[0];
						sprintf((char*)IDstring,"%X",((*(uint16_t*)(0x1ffff7e8+2*i))>>8)&0x0f);
						mycpuid[3+4*i] = IDstring[0];				
					}	
			
			   sprintf((char*)sedbuffer,"AT+HTTPPARA=url,official.mqtt.api.gzdianrui.com/devGateway/deviceRegister?deviceName=%s\r\n",mycpuid);
				if(Second_AT_Command(sedbuffer,"OK",3)==1)  
					step = 3;
				break;
			case 3:
				if(Second_AT_Command("AT+HTTPSETUP\r\n","OK",4)==1) 
					step = 4;
				break;				
			case 4:	
				
				if(Second_AT_Command("AT+HTTPACTION=0\r\n","+HTTPCLOSED",5)==1) 
					step = 5;				
				break;
            case 5:			
                p=strstr((const char*)ucUart2Buff,"\"deviceSecret\":\"");
                if(p!=NULL)
                {
                    for(i=0;i<32;i++)
                    {
                        
                        deviceSecret[i] = *(p+16+i);
                        if((*(p+17+i))=='"')
                            break;
                                            
                    }				
                }
                p=strstr((const char*)ucUart2Buff,"\"productKey\":\"");
                if(p!=NULL)
                {
                    for(i=0;i<32;i++)
                    {
                        
                        productKey[i] = *(p+14+i);
                        if((*(p+15+i))=='"')
                            break;
                                            
                    }				
                }
                Flash_Write(0x0800fc00, productKey, 32);	
                Flash_Write(0x0800fc20, deviceSecret, 32);
                Flash_Write(0x0800ffc0, flash, 32);	

                if(productKey[0]!=0)
                {
                    __disable_fault_irq();   
                    NVIC_SystemReset();
                }            
                break;
			 default :break;			
		 }						
   }		
}

void Even_Processing(void)
{			
  char amd[300] = {0};
  
	if(reflag)
	{
   if(Second_AT_Command(MqttParameter.PublishTopic,"+CLOUDPUBACK: <OK>",12)==1)
      {
         reflag=0;
      }
	}else if(Char_Hex_flag)
	{
		Char_Hex_flag=0;
		Uart3SendData((char *)MqttParameter.char_Hex_data,MqttParameter.length);
	}else if(Hex_char_flag)
	{
			Hex_char_flag=0;
			Uart2SendString((char *)MqttParameter.Hex_char_data);

	}else if(Card_Sendevent)
	{
		Uart2SendString((char *)MqttParameter.PublishYC);
		Card_Sendevent=0;


	}else if(ONCard_Sendevent)
	{
		Uart2SendString((char *)MqttParameter.PublishNC);ONCard_Sendevent=0;

	}
    else if(Version)
	{
        Version=0;
        Uart2SendString((char *)MqttParameter.Version);
	}
    else if(restart)
	{
        restart=0;
        __disable_fault_irq();   
        NVIC_SystemReset();
	}
    else if(waitSemaphore)
	{
	snprintf(amd, sizeof(amd), SEMAPHORE,MqttParameter.ProductKey,MqttParameter.DeviceName,CREG);
	snprintf((char*)MqttParameter.PublishTopic, sizeof(MqttParameter.PublishTopic), AT_CLOUDPUB,MqttParameter.ProductKey,MqttParameter.DeviceName,amd);
  Uart2SendString((char *)MqttParameter.PublishTopic);
	memset(amd,0,strlen(amd));
	memset(CREG,0,strlen((char*)CREG));
  waitSemaphore=0;Semaphore=0;
	}
}

//用户函数
void User(void)	
{
	static unsigned char checkonline = 0,led=0;
	char *p;

	if(Systick_1ms<2000)
		return ;
	
	Read_card += uc10ms;
	if(Read_card>=20)
	{
		Read_card=0;
		readCard();	
	}
	if(!Char_Hex_flag&&!Hex_char_flag&&!ONCard_Sendevent&&!Card_Sendevent&&!reflag)
	{
		checkonline += uc1s;
		if((checkonline>=15)&&online)
		{
			checkonline = 0;
			Uart2SendString((char *)"AT+CLOUDSTATE?\r\n");
		}
        if(Find("+CLOUDPUBACK: <OK>")==1)
        {
            memset(ucUart2Buff,0,strlen((char*)ucUart2Buff));
            checkonline=0;
        }
        if(Semaphore)
        {
            if(Second_AT_Command("AT+CREG?\r\n","+CREG",8)==1)
            {
                if(Find("0,0")==1)
                {
                    step = 7;
                }
                else
                {
                    p=strstr((const char*)ucUart2Buff,":");
                    if(p!=NULL)
                    {
						p++;
						CREG[0] = *p++;
						CREG[1] = *p++;
						CREG[2] = *p++;
						CREG[3] = *p++;
						waitSemaphore=1;
                    }
                }                
            }
        }        

	}
	if(Find("+CLOUDSTATE:0")==1)
	{
		online = 0;
		{
			Uart1SendString("Off-line!");
			step = 6;
		}
	}

	if(!online)
	{
		led +=uc100ms;
		if(led>=5)
		{
			LED2G=~LED2G;
			led=0;
		}
	}
	 Key();
	if(Relay_Delay_Cnt==0)
	{
		if((RF_Stat==0) && (GPRS_Stat==0))
		{
			RelayOUT = 0;
			Relay_LED = 1;
			Relay_Stat = 0;
		}
	}
	GPRS_Config(GPRS); 
	
	if(online)
	{
		Even_Processing();
		if(ucUart2ReceTimeOut<=1)
		{
			flag = json_parsing((char*)ucUart2Buff);
			if(flag)
			{
				reflag=1;
				snprintf((char*)MqttParameter.PublishTopic, sizeof(MqttParameter.PublishTopic),TOPIC_RRPC_RSP,MqttParameter.ProductKey,MqttParameter.DeviceName,MqttParameter.Number);
				if(json_get_value((char*)ucUart2Buff,"code", (char*)MqttParameter.Code))
				{
					Uart1SendData((const char*)ucUart2Buff,ucUart2ReceLen);
					if(!strcmp((char*)MqttParameter.Code,"door"))//开门
					{
						if(json_get_value((char*)ucUart2Buff,"message", (char*)MqttParameter.Message))
						{
								Char_Hex(MqttParameter.Message,strlen((char*)MqttParameter.Message));
						}
					}
					else if(!strcmp((char*)MqttParameter.Code, "Door"))//开门开灯
					{
						GPRS_Stat = 1;
						Relay_LED = 0;
						RelayOUT = 1;
						Relay_Stat = 1;
						if(json_get_value((char*)ucUart2Buff,"message", (char*)MqttParameter.Message))
						{
								Char_Hex(MqttParameter.Message,strlen((char*)MqttParameter.Message));
						}
					}
					else if(!strcmp((char*)MqttParameter.Code, "electricity"))//电量查询
					{
						if(json_get_value((char*)ucUart2Buff,"message", (char*)MqttParameter.Message))
						{
								Char_Hex(MqttParameter.Message,strlen((char*)MqttParameter.Message));
						}
					}
					else if(!strcmp((char*)MqttParameter.Code, "light"))
					{
						if(json_get_value((char*)ucUart2Buff,"message", (char*)MqttParameter.Message))
						{
							Flash_Write(0x0800fc40, MqttParameter.Message, 32);
							//加重启
							restart=1;
						}
					}
					else if(!strcmp((char*)MqttParameter.Code, "update"))//升级APP
					{
						Flash_Write(0x0800ffe0, MqttParameter.Code, 32);
						restart=1;
						//加重启
					}
					else if(!strcmp((char*)MqttParameter.Code, "open"))//开灯
					{
							GPRS_Stat = 1;
							Relay_LED = 0;
							RelayOUT = 1;
							Relay_Stat = 1;
					}
					else if(!strcmp((char*)MqttParameter.Code, "close"))//关灯
					{
						GPRS_Stat = 0;
						if(RF_Stat==0)
						{
							Relay_LED = 1;
							RelayOUT = 0;
							Relay_Stat = 0;
						}
					}
					else if(!strcmp((char*)MqttParameter.Code, "semaphore"))//查询2G信号
					{
						   Semaphore=1;
					}
					else if(!strcmp((char*)MqttParameter.Code, "version"))//查询APP版本
					{
						   Version=1;
					}
					
					memset(MqttParameter.Code,0,strlen((char*)MqttParameter.Code));
					memset(MqttParameter.Message,0,strlen((char*)MqttParameter.Message));
				}	
				memset(ucUart2Buff,0,ucUart2ReceLen);
				flag = 0;
			}
		}
		if((ucUart3ReceTimeOut<=1)&&ReadFlagUart3)
		{
			
			Hex_char((u8*)ucUart3Buff,ucUart3ReceLen);
			Hex_char_flag = 1;
		}
	}	
}

void MQTT_Init(void)
{
    int i;
    for(i=0;i<6;i++)
    {	
        sprintf((char*)IDstring,"%X",((*(uint16_t*)(0x1ffff7e8+2*i))>>4)&0x0f);
        mycpuid[0+4*i] = IDstring[0];
        sprintf((char*)IDstring,"%X",((*(uint16_t*)(0x1ffff7e8+2*i))>>0)&0x0f);
        mycpuid[1+4*i] = IDstring[0];					
        sprintf((char*)IDstring,"%X",((*(uint16_t*)(0x1ffff7e8+2*i))>>12)&0x0f);
        mycpuid[2+4*i] = IDstring[0];
        sprintf((char*)IDstring,"%X",((*(uint16_t*)(0x1ffff7e8+2*i))>>8)&0x0f);
        mycpuid[3+4*i] = IDstring[0];				
    }
  memcpy((char*)MqttParameter.DeviceName,mycpuid,24);   
	snprintf((char*)MqttParameter.Cloudhdauth, sizeof(MqttParameter.Cloudhdauth), AT_CLOUDHDAUTH, MqttParameter.ProductKey,MqttParameter.DeviceName,MqttParameter.DeviceSecret);
	snprintf((char*)MqttParameter.SubscribeTopic, sizeof(MqttParameter.SubscribeTopic), AT_CLOUDSUB, MqttParameter.ProductKey,MqttParameter.DeviceName);
	snprintf((char*)MqttParameter.Version, sizeof(MqttParameter.Version), AT_CLOUDPUBVersionMESSAGE, MqttParameter.ProductKey,MqttParameter.DeviceName, MqttParameter.ProductKey,MqttParameter.DeviceName);
	snprintf((char*)MqttParameter.PublishYC, sizeof(MqttParameter.PublishYC), AT_CLOUDPUBYCMESSAGE, MqttParameter.ProductKey,MqttParameter.DeviceName, MqttParameter.ProductKey,MqttParameter.DeviceName);
	snprintf((char*)MqttParameter.PublishNC, sizeof(MqttParameter.PublishNC), AT_CLOUDPUBNCMESSAGE, MqttParameter.ProductKey,MqttParameter.DeviceName, MqttParameter.ProductKey,MqttParameter.DeviceName);
}

int json_parsing(char* root)
{
	if(json_get_number_value(root,"request", (char*)MqttParameter.Number))
	{
		return 1;
	}	
	return 0;
}
/*********************************************************************************/
//                          16进制转字符                                          //
/*********************************************************************************/
void Hex_char(u8 *p,int length)
{
	char amd[300] = {0};
	char toChar[200]; 
	unsigned char    i;
	char   toChar_ge[1]; 
	char   toChar_shi[1]; 

	for(i=0;i<length;i=i+1)
	{
		
		toChar_ge[0]=*p;  
		toChar_shi[0]=toChar_ge[0]>>4;
		toChar_ge[0]=toChar_ge[0]&0x0f;

		if(toChar_ge[0]<=9)
		{
			toChar_ge[0]=toChar_ge[0]+'0';  
		}
		else 
		{
			toChar_ge[0]=toChar_ge[0]+'A'-10; 
		}

		if(toChar_shi[0]<=9)
		{
			toChar_shi[0]=toChar_shi[0]+'0';  
		}
		else 
		{
			toChar_shi[0]=toChar_shi[0]+'A'-10;  
		}

		toChar[2*i]=toChar_shi[0]; 
		toChar[2*i+1]=toChar_ge[0];
		p++;
	}	
	Uart1SendData((const char*)toChar,strlen(toChar));
	snprintf(amd, sizeof(amd), DATE,MqttParameter.ProductKey,MqttParameter.DeviceName,toChar);
	snprintf((char*)MqttParameter.Hex_char_data, sizeof(MqttParameter.Hex_char_data), AT_CLOUDPUB,MqttParameter.ProductKey,MqttParameter.DeviceName,amd);
	memset(amd,0,strlen(amd));
	memset(toChar,0,strlen(toChar));
	ReadFlagUart3=0;
}


/*********************************************************************************/
//                           字符转16进制                                         //
/*********************************************************************************/
void Char_Hex(u8*p,int length)
{
	char toHEX[200]; 
	unsigned char    i;
	char   toHEX_ge[1]; 
	char   toHEX_shi[1]; 
	
  
	if(length > 0) 
	{
		for(i=0;i<length+2;i=i+2)
		{
			toHEX_shi[0]=*(p+i);
			toHEX_ge[0]=*(p+i+1);

			if(toHEX_shi[0]>='0'&&toHEX_shi[0]<='9')
			{
				toHEX_shi[0]=(toHEX_shi[0]-'0');	
			}
			else if(toHEX_shi[0]>='a'&&toHEX_shi[0]<='f')
			{
				toHEX_shi[0]=(toHEX_shi[0]-'a'+10);	
			}
			else if(toHEX_shi[0]>='A'&&toHEX_shi[0]<='F')
			{
				toHEX_shi[0]=(toHEX_shi[0]-'A'+10);	
			}

			if(toHEX_ge[0]>='0'&&toHEX_ge[0]<='9')
			{
				toHEX_ge[0]=(toHEX_ge[0]-'0');	
			}
			else if(toHEX_ge[0]>='a'&&toHEX_ge[0]<='f')
			{
				toHEX_ge[0]=(toHEX_ge[0]-'a'+10);	
			}
			else if(toHEX_ge[0]>='A'&&toHEX_ge[0]<='F')
			{
				toHEX_ge[0]=(toHEX_ge[0]-'A'+10);	
			}

			toHEX[i/2]=toHEX_shi[0]*16+toHEX_ge[0];
			
		}
		memcpy(MqttParameter.char_Hex_data,toHEX,length/2);
		MqttParameter.length=length/2;
		Char_Hex_flag = 1;
		memset(toHEX,0,200);
		length = 0;
	}
}

