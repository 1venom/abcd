/***************************************
name:          JSON C函数库 C文件
***************************************/
#include "stdio.h"	 	 
#include "string.h"	
#include <stdlib.h>
#include <ctype.h>
#include <json.h>
/***************************************
name:      函数声明
input:   
output:    
description:
***************************************/
char json_check(char *str,char *json_value);
char json_get_value(char *json,char *json_key , char *json_value);
char json_check_value(char *str1, char *str2);
/***************************************
name:       JSON 校验函数
input:      字符串
output:     合法JSON 返回1 不合法JSON 返回0
description:
***************************************/
char json_check(char *str,char *json_value)
{
       char *json_value_start;        
       char str_length;
       str_length = strlen(str);
				json_value_start = strstr(str,(const char *)"{");
       if(str[0] == '{' && str[str_length-1] == '}')
       {
							strncpy(json_value,json_value_start,str_length);
              return 1;
       }
       else
       {
              return 0;
       }
}
/***************************************
name:       JSON 获取键值函数
input:      JSON字符串 要获取的键名 获取键值的字符串
output:     如果获取成功返回1 获取失败返回0
description:
***************************************/
char json_get_value(char *json,char *json_key , char *json_value)
{
       char *json_key_start;        //定义键名开始的位置
       char *json_key_end;          //定义键名结束的位置
       char json_key_length;        //定义键名长度
       char *json_value_start;      //定义键值开始的位置
       char *json_value_end;        //定义键值结束的位置
       char json_value_length;      //定义键值长度
       json_key_start = strstr(json,json_key);       //获取键名开始的位置
       json_key_length = strlen(json_key);           //获取键名的长度
       json_key_end = json_key_start + json_key_length;       //获取键名结束的位置
       if(json_key_start != 0 && *(json_key_start - 1) == '\"' && *(json_key_end) == '\"' && *(json_key_end + 1) == ':' && *(json_key_end + 2) == '\"' )
       {
              json_value_start = json_key_end + 3;      //获取键值开始的位置
              json_value_end = strstr(json_value_start,"\"");       //获取键值结束的位置
              json_value_length = json_value_end - json_value_start;    //获取键值的长度
              strncpy(json_value,json_value_start,json_value_length);      //将键值存入数组指针
              json_value[json_value_length] = '\0';                  //补字符串结束符
              return 1;                                  
       }
       else
       {
              json_value[0] = '\0';
              return 0;
       }
}

 

/***************************************
name:       JSON 键值比对函数
input:      JSON 键值 要匹配的字符
output:     如果匹配成功返回1 失败返回0
description:
***************************************/

char json_check_value(char *str1, char *str2)
{
       if(strcmp(str1,str2) == 0)
       {
              return 1; 
       }
       else
       {
              return 0;
       }

}

/***************************************

***************************************/
char json_get_number_value(char *json,char *json_key , char *json_number_value)
{
       char *json_key_start;        
       char *json_key_end;  
       char json_key_length;   
       char *json_value_start;    
       char *json_value_end;   
       char json_value_length;  
       json_key_start = strstr(json,json_key);
       json_key_length = strlen(json_key);
       json_key_end = json_key_start + json_key_length;
       if(json_key_start != 0 && *(json_key_start - 1) == '/' && *(json_key_end) == '/')
       {
              json_value_start = json_key_end + 1;
              json_value_end = strstr(json_value_start,",");
              json_value_length = json_value_end - json_value_start;
              strncpy(json_number_value,json_value_start,json_value_length);
              json_number_value[json_value_length] = '\0';
              return 1;                                  
       }
       else
       {
              json_number_value[0] = '\0';
              return 0;
       }
}

void MQTTParameter(char *p,char *key_start,char *key_end,char *value)
{
	 char *msg_start;        
	 char *msg_end;  
	 char msg_length;
	
		msg_start = strstr(p,key_start);
		msg_start = msg_start + 1;
		msg_end= strstr(msg_start,key_end);
		msg_length = msg_end - msg_start;
		strncpy(value,msg_start,msg_length);
}

