#ifndef __JSON_H
#define	__JSON_H


char json_check(char *str,char *json_value); //JSON ????
char json_get_value(char *json,char *json_key , char *json_value); //JSON????
char json_check_value(char *str1, char *str2); //JSON ??????
char json_get_number_value(char *json,char *json_key , char *json_number_value);
void MQTTParameter(char *p,char *key_start,char *key_end,char *value);

#endif 




