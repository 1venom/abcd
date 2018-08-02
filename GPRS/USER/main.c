#include "stm32f10x.h"
#include "sysrun.h"
#include "User.h"
#include "stmflash.h"
#include "string.h"
#include "modbus.h"
#include "mqtt.h"
#include "rc522.h"


char Tohex(char p)
{
      if(p>='0'&&p<='9')
      {
        p=(p-'0');	
      }
      else  if(p>='a'&&p<='f')
      {
        p=(p-'a'+10);	
      }
      else  if(p>='A'&&p<='F')
      {
        p=(p-'A'+10);	
      }
      return p;
}


int main(void)
{
	unsigned char flash[32]={0};
	unsigned char flash_erasure[32]={0};
	unsigned char datatemp[40]={0};
	char i=0;
	NVIC_SetVectorTable( NVIC_VectTab_FLASH, 0x08000000 );
	SystemUserInit();
	Flash_Read(0x0800ffc0, (uint8_t *)flash, 16);
	if(strcmp((const char*)flash,"Dianrui")==0)
	{
		Flash_Read(0x0800fc00, (uint8_t *)datatemp,32);
		memcpy(MqttParameter.ProductKey,datatemp,32);
		Flash_Read(0x0800fc20, (uint8_t *)MqttParameter.DeviceSecret, 32);
		Flash_Read(0x0800fc40, (uint8_t *)MqttParameter.Light, 32);
		if(MqttParameter.Light[1]==0x0A)
		{
			for(i=0;i<sizeof(MqttParameter.Light);i++)
			MqttParameter.Light[i]=Tohex(MqttParameter.Light[i]);
			HotelTag_Buf[0]=MqttParameter.Light[4]*0x10+MqttParameter.Light[5];
			HotelTag_Buf[1]=MqttParameter.Light[6]*0x10+MqttParameter.Light[7];
			HotelTag_Buf[2]=MqttParameter.Light[8]*0x10+MqttParameter.Light[9];
			RoomNumber_Buf[0]=MqttParameter.Light[14]*0x10+MqttParameter.Light[15];
			RoomNumber_Buf[1]=MqttParameter.Light[16]*0x10+MqttParameter.Light[17];
			RoomNumber_Buf[2]=MqttParameter.Light[18]*0x10+MqttParameter.Light[19];
			LocalInformation_Flag=1;
		}
		GPRS=1;
	}
	else
	{
		GPRS=2;
	}
	MQTT_Init();
 	
   while (1)
    { 
		SystemAction();	
		User();	
		if(*(uint8_t*)(0x0800ffe0)=='u'&&*(uint8_t*)(0x0800ffe1)=='p'&&Systick_1ms<10000)
		Flash_Write(0x0800ffe0, flash_erasure, 32);				
	}			
}



