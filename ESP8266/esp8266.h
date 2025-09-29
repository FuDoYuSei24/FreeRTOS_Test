#ifndef __ESP8266_H__
#define __ESP8266_H__
#include "sys.h" 

void esp8266_start_trans(void);
uint8_t esp8266_send_cmd(uint8_t *cmd,uint8_t *ack,uint16_t waittime);
uint8_t* esp8266_check_cmd(uint8_t *str);



#endif




