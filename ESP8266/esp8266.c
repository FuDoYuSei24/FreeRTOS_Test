#include "esp8266.h"
#include "delay.h"
#include "string.h"
#include "usart.h"
#include "usart3.h"

void esp8266_start_trans(void)
{
    esp8266_send_cmd("AT+CWMODE=2","OK",50);
	//Wifi模块重启
	esp8266_send_cmd("AT+RST","OK",20);
	delay_ms(1000);         //延时3S等待重启成功
	delay_ms(1000);
	delay_ms(1000);	
    //AP模式
	esp8266_send_cmd("AT+CWSAP=\"想学ESP8266吗\",\"84758783\",11,3","OK",200);
	esp8266_send_cmd("AT+CIPMUX=1","OK",20);
	esp8266_send_cmd("AT+CIPSERVER=1,8086","OK",200);
	esp8266_send_cmd("AT+CIFSR","OK",200);
	esp8266_send_cmd("AT+CIPSEND=0,8","OK",200);

	
	
}	


uint8_t esp8266_send_cmd(uint8_t *cmd,uint8_t *ack,uint16_t waittime)
{
	uint8_t res=0; 
	USART3_RX_STA=0;
	u3_printf("%s\r\n",cmd);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(esp8266_check_cmd(ack))
				{
					printf("ack:%s\r\n",(uint8_t*)ack);
					break;//得到有效数据  
				}
					USART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
}

uint8_t* esp8266_check_cmd(uint8_t *str)
{
	char *strx=0;
	if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
	return (uint8_t*)strx;
}





