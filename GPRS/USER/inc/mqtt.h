#ifndef __MQTT_H
#define __MQTT_H

#include "user.h"

//AT
#define AT "AT\r\n"

//ATE0 close echo function
#define ATE0 "ATE0\r\n"


//AT_CCID ���SIM��
#define AT_CCID "AT+CCID\r\n"

/********************���ų�ʼ��******************************/
//AT_CMGF ���ö���Ϊ�ı�ģʽ
#define AT_CMGF "AT+CMGF=1\r\n"

//AT_CNMI  �����յ��������ģʽ�
#define AT_CNMI "AT+CNMI=2,2,0,0,0\r\n"

//AT_CPMS ���ö��Ŵ洢��SM����
#define AT_CPMS "AT+CPMS=\"SM\"\r\n"

/**************************************************/
//AT+CREG?
#define AT_CREG "AT+CREG?\r\n"

/*******************************************
*AT+CLOUDHDAUTH=<productKey> ,<deviceName>,<deviceSecret><CR>
*productKey:��Ʒkey�����裬�ַ������ͣ���󳤶�11Byte
*deviceName:�豸���֣����裬�ַ������ͣ���󳤶�32Byte
*deviceSecret:�豸��Կ�����裬�ַ������ͣ���󳤶�32Byte
*******************************************/
#define AT_CLOUDHDAUTH_mesage "AT+CLOUDHDAUTH=%s\r\n"
#define AT_CLOUDHDAUTH "AT+CLOUDHDAUTH=%s,%s,%s\r\n"

/*******************************************
*AT+CLOUDCONN=<keepAlive>,<clean>,<version><CR>
*keepAlive:ʱ�����ã�����ָ������Χ60-180��
*clean:�Ƿ����session���������ͣ�0-����� 1-���
*version:mqtt�汾3 = 3.1 ��4 = 3.1.1
*******************************************/
#define AT_CLOUDCONN "AT+CLOUDCONN=60,0,4\r\n"

/****************************************
*AT+CLOUDSUB=<topic>,<qos><CR>
*topic������topic���ַ�
*qos��topic��Ӧ��qos������
****************************************/
#define AT_CLOUDSUB "AT+CLOUDSUB=/%s/%s/get,0\r\n"

/****************************************
*AT+CLOUDPUB=<topic>,<qos>,<message><CR>
*topic������������
*qos��������Ϣ��Qos�ȼ�(0��Ӧ�ٶȽϿ�)
*msg����Ϣ���ݣ��������1024�ֽ�
****************************************/
#define AT_CLOUDPUB "AT+CLOUDPUB=/%s/%s/update,0,%s\r\n"
#define AT_CLOUDPUBVersionMESSAGE "AT+CLOUDPUB=/%s/%s/update,0,{productKey:%s,deviceName:%s,code:version,message:YF10}\r\n"
#define AT_CLOUDPUBYCMESSAGE "AT+CLOUDPUB=/%s/%s/update,0,{productKey:%s,deviceName:%s,code:power,message:01}\r\n"//�п�
#define AT_CLOUDPUBNCMESSAGE "AT+CLOUDPUB=/%s/%s/update,0,{productKey:%s,deviceName:%s,code:power,message:00}\r\n"//�޿�

/****************************************
*AT+CLOUDSTATE?<CR>
*+CLOUDSTATE:0 :��ǰMQTT�Ͽ�
*+CLOUDSTATE:1 :��ǰMQTT����
****************************************/
//AT+CLOUDSTATE=/1000146090/Ndevice_T1/neo001,0,hello
#define AT_CLOUDSTATE "AT+CLOUDSTATE?\r\n"

/****************************************
*AT+CLOUDSTATE?<CR>
*+CLOUDSTATE:0 :��ǰMQTT�Ͽ�
*+CLOUDSTATE:1 :��ǰMQTT����
****************************************/
#define AT_CLOUDDISCONN "AT+CLOUDDISCONN\r\n"


#define DATE "{productKey:%s,deviceName:%s,code:door,message:%s}"
#define SEMAPHORE "{productKey:%s,deviceName:%s,code:semaphore,message:%s}"
#define CardDATE "{productKey:%s,deviceName:%s,code:power,message:%s}"

#define TOPIC_RRPC_REQ       "AT+CLOUDPUB=/sys/%s/%s/rrpc/request/%s"
#define TOPIC_RRPC_RSP       "AT+CLOUDPUB=/sys/%s/%s/rrpc/response/%s,0,{\"message\":\"ok!\"}\r\n"

//#define AT_CLOUDHDAUTH "AT+CLOUDHDAUTH=iXTESaYy3TG,TESTN10,B2fyrABwvjtgaWftFKbsWKStt7LWRqPW\r"


//#define productKey "iXTESaYy3TG"
//#define deviceName "TESTN10"
//#define deviceSecret "B2fyrABwvjtgaWftFKbsWKStt7LWRqPW"
/*************************************************/

typedef struct
{
	unsigned char ProductKey[50];
	unsigned char DeviceName[50];
	unsigned char Cloudhdauth[100];
	unsigned char PublishTopic[150];
	unsigned char SubscribeTopic[100];
	unsigned char Code[10];
	unsigned char Message[150];
	unsigned char Number[64];
	unsigned char char_Hex_data[100];
	unsigned char Hex_char_data[500];
	unsigned char PublishYC[200];
	unsigned char PublishNC[200];
    unsigned char DeviceSecret[33];
    unsigned char Light[50];
    unsigned char Version[150];    
	int length;
}mqttparameter;

#endif


