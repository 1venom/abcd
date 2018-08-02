/***************************************
name:          JSON C������ C�ļ�
***************************************/
#include "stdio.h"	 	 
#include "string.h"	
#include <stdlib.h>
#include <ctype.h>
#include <json.h>
/***************************************
name:      ��������
input:   
output:    
description:
***************************************/
char json_check(char *str,char *json_value);
char json_get_value(char *json,char *json_key , char *json_value);
char json_check_value(char *str1, char *str2);
/***************************************
name:       JSON У�麯��
input:      �ַ���
output:     �Ϸ�JSON ����1 ���Ϸ�JSON ����0
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
name:       JSON ��ȡ��ֵ����
input:      JSON�ַ��� Ҫ��ȡ�ļ��� ��ȡ��ֵ���ַ���
output:     �����ȡ�ɹ�����1 ��ȡʧ�ܷ���0
description:
***************************************/
char json_get_value(char *json,char *json_key , char *json_value)
{
       char *json_key_start;        //���������ʼ��λ��
       char *json_key_end;          //�������������λ��
       char json_key_length;        //�����������
       char *json_value_start;      //�����ֵ��ʼ��λ��
       char *json_value_end;        //�����ֵ������λ��
       char json_value_length;      //�����ֵ����
       json_key_start = strstr(json,json_key);       //��ȡ������ʼ��λ��
       json_key_length = strlen(json_key);           //��ȡ�����ĳ���
       json_key_end = json_key_start + json_key_length;       //��ȡ����������λ��
       if(json_key_start != 0 && *(json_key_start - 1) == '\"' && *(json_key_end) == '\"' && *(json_key_end + 1) == ':' && *(json_key_end + 2) == '\"' )
       {
              json_value_start = json_key_end + 3;      //��ȡ��ֵ��ʼ��λ��
              json_value_end = strstr(json_value_start,"\"");       //��ȡ��ֵ������λ��
              json_value_length = json_value_end - json_value_start;    //��ȡ��ֵ�ĳ���
              strncpy(json_value,json_value_start,json_value_length);      //����ֵ��������ָ��
              json_value[json_value_length] = '\0';                  //���ַ���������
              return 1;                                  
       }
       else
       {
              json_value[0] = '\0';
              return 0;
       }
}

 

/***************************************
name:       JSON ��ֵ�ȶԺ���
input:      JSON ��ֵ Ҫƥ����ַ�
output:     ���ƥ��ɹ�����1 ʧ�ܷ���0
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

