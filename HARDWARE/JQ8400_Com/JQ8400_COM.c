#include "JQ8400_COM.h"
#include "delay.h"
#include "usart.h"	

	u16 soud_del = 800;
//9600
//设置声音大小  num 0-30
// 例如：AA 13 01 14 D2 设置音量为 20 级
void SET_JQ8400_SOUND(u8 num)
{
	u8 tab[]={0xaa,0x13,0x01,0x08,0x00};
	u8 sum = 0;
	u8 i = 0;
	
	tab[3] = num;

	for(i=0;i<4;i++)
	sum = sum + tab[i]; 
	
  tab[4] = sum;
	for(i=0;i<5;i++)
    Usart_SendByte(USART3,tab[i]);
	
}


//例如: AA 07 02 00 08 BB 指定播放当前盘符第 8 首，
//播放指定声音
void paly_sound(u8 num)
{
	u8 tab[]={0xaa,0x07,0x02,0x00,0x08,0x00};
	u8 sum = 0;
	u8 i = 0;
	
	tab[3] = 0;
	tab[4] = num;
	
	for(i=0;i<5;i++)
	sum = sum + tab[i]; 
	
  tab[5] = sum;
	for(i=0;i<6;i++)
    Usart_SendByte(USART3,tab[i]);
	
}
