#include "sim900a.h"
#include "usart.h"	
#include "delay.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

////初始化短信模块
void ini_sim900A_en(void)
{
	   delay_ms(1000);delay_ms(1000);//延时大约2秒  
	   delay_ms(1000);delay_ms(1000);//延时大约2秒 
		 printf("AT+CSCS=\"UCS2\"\r\n");
     delay_ms(1000);delay_ms(1000);//延时大约2秒
    printf("AT+CMGF=1\r\n");
     delay_ms(1000);delay_ms(1000);//延时2秒
	  printf("AT+CSCA?\r\n");
     delay_ms(1000);delay_ms(1000);//延时2秒
	  printf("AT+CSMP=17,167,0,25\r\n");
     delay_ms(1000);delay_ms(1000);//延时2秒
	
}





////	static u8 sim900A_n = 0;
////	static uint32_t sim900A_time = 0 ;
