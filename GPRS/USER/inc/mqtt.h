#ifndef __MQTT_H
#define __MQTT_H

#include "user.h"

//AT
#define AT "AT\r\n"

//ATE0 close echo function
#define ATE0 "ATE0\r\n"


//AT_CCID 检查SIM卡
#define AT_CCID "AT+CCID\r\n"

/********************短信初始化******************************/
//AT_CMGF 设置短信为文本模式
#define AT_CMGF "AT+CMGF=1\r\n"

//AT_CNMI  设置收到短刑嵝涯Ｊ脚
#define AT_CNMI "AT+CNMI=2,2,0,0,0\r\n"

//AT_CPMS 设置短信存储在SM卡内
#define AT_CPMS "AT+CPMS=\"SM\"\r\n"

/**************************************************/
//AT+CREG?
#define AT_CREG "AT+CREG?\r\n"

/*******************************************
*AT+CLOUDHDAUTH=<productKey> ,<deviceName>,<deviceSecret><CR>
*productKey:产品key，必需，字符串类型，最大长度11Byte
*deviceName:设备名字，必需，字符串类型，最大长度32Byte
*deviceSecret:设备秘钥，必需，字符串类型，最大长度32Byte
*******************************************/
#define AT_CLOUDHDAUTH_mesage "AT+CLOUDHDAUTH=%s\r\n"
#define AT_CLOUDHDAUTH "AT+CLOUDHDAUTH=%s,%s,%s\r\n"

/*******************************************
*AT+CLOUDCONN=<keepAlive>,<clean>,<version><CR>
*keepAlive:时间设置，必须指定，范围60-180秒
*clean:是否清除session，数字类型，0-不清除 1-清除
*version:mqtt版本3 = 3.1 ；4 = 3.1.1
*******************************************/
#define AT_CLOUDCONN "AT+CLOUDCONN=60,0,4\r\n"

/****************************************
*AT+CLOUDSUB=<topic>,<qos><CR>
*topic：订阅topic，字符
*qos：topic对应的qos，数字
****************************************/
#define AT_CLOUDSUB "AT+CLOUDSUB=/%s/%s/get,0\r\n"

/****************************************
*AT+CLOUDPUB=<topic>,<qos>,<message><CR>
*topic：发布的主题
*qos：发布消息的Qos等级(0响应速度较快)
*msg：消息内容，长度最大1024字节
****************************************/
#define AT_CLOUDPUB "AT+CLOUDPUB=/%s/%s/update,0,%s\r\n"
#define AT_CLOUDPUBVersionMESSAGE "AT+CLOUDPUB=/%s/%s/update,0,{productKey:%s,deviceName:%s,code:version,message:YF10}\r\n"
#define AT_CLOUDPUBYCMESSAGE "AT+CLOUDPUB=/%s/%s/update,0,{productKey:%s,deviceName:%s,code:power,message:01}\r\n"//有卡
#define AT_CLOUDPUBNCMESSAGE "AT+CLOUDPUB=/%s/%s/update,0,{productKey:%s,deviceName:%s,code:power,message:00}\r\n"//无卡

/****************************************
*AT+CLOUDSTATE?<CR>
*+CLOUDSTATE:0 :当前MQTT断开
*+CLOUDSTATE:1 :当前MQTT连接
****************************************/
//AT+CLOUDSTATE=/1000146090/Ndevice_T1/neo001,0,hello
#define AT_CLOUDSTATE "AT+CLOUDSTATE?\r\n"

/****************************************
*AT+CLOUDSTATE?<CR>
*+CLOUDSTATE:0 :当前MQTT断开
*+CLOUDSTATE:1 :当前MQTT连接
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


