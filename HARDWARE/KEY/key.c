#include "key.h"
#include "delay.h"
	    
//按键初始化函数 
//PA15和PC5 设置成输入
void KEY_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//使能PORTA,PORTC时钟

//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//关闭jtag，使能SWD，可以用SWD模式调试
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;//PA5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA15
	
} 

u8 KEY_Scan(void)
{
  u8 keyy = 0;
	if(key1==0)
	{
		keyy = 1;
		delay_ms(20);	
		while(key1==0);				//等待释放
	}
	
	if(key2==0)
	{
		keyy = 2;
		delay_ms(20);	
		while(key2==0);				//等待释放
	}
	
	if(key3==0)
	{
		keyy = 3;
		delay_ms(20);	
		while(key3==0);				//等待释放
	}	
	
	if(key4==0)
	{
		keyy = 4;
		delay_ms(20);	
		while(key4==0);				//等待释放
	}	
	
	return keyy;
}
	
