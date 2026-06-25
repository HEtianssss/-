#include "led.h"
#include "delay.h"
#include "sys.h"
#include "timer.h"
#include "key.h"
#include "usart.h"	
#include <string.h>
#include "gps.h"
#include "oled_iic.h"	
#include "adxl345.h"
#include "hc_sr04.h"
#include "sim900a.h"
#include "adc_dma.h"
#include "JQ8400_COM.h"

 u8 set_flg = 0;  //0正常模式  1设置距离  2 设置光照
 u8 set_juli = 30;  //1设置距离
 u8 set_light = 50; //2 设置光照
 
 //u8 diedao_flg = 0; //   1跌倒  0正常  
 
  u8 juli_time = 0;  //距离播报间隔时间
  u8 jishui_time = 0;  //积水播报间隔时间
	
 
 u16 juli = 0;
 u16 light = 0;
 
 GPS_DataStructure *Gsp;

short temperature = 0; 				//温度值


///******************
//发送短信单个数字字符
//********************/
void sendSMSByte(char s)			//短信发送一个字节
{
	switch(s)
	{
	case '.' :
		 printf("002E") ; 			//发送小数.
		break ;
	default :
		if(s<='9' && s >='0')
		{
			 printf("003%c",s ) ; 	//0-9发送字符
		}
		break ;
	}

}
///******************
//发送短信数字字符串
//c:小数的位数
//f:发送的具体数值
//********************/

void sendSMSNum(char c, double  f)
{
	

	char str[50] = {0} ;
	char *ptr = str;

	switch(c)
	{
	case 0 :
		sprintf(str,"%f", f ) ;
		break ;
	case 1 :
		sprintf(str,"%.1f", f ) ;
		break ;
	case 2 :
		sprintf(str,"%.0f", f ) ;
		break ;
	default :
		break ;
	}
	while(*ptr)
	{
		 sendSMSByte(*ptr++) ;
	}




}

/*********************************
短信发送经纬度，
***********************************/
void  send_gsm3(void) 
{
//	  u8 count0 = 80;
//	  u16 temp = 321;
	  double E = 22.504321;
	 // 15078935942
	  //                     
    printf("AT+CMGS=\"00310038003100300030003700350037003300310039\"\r\n");//电话话的UNICON编码
	
       delay_ms(1000);//延时1秒
	  printf("7ECF5EA6003A") ; //经度：
		sendSMSNum(0, Gsp->f_N_weidu) ;
	  printf("002C") ; //,
	  printf("7EAC5EA6003A") ; //纬度：
		sendSMSNum(0, Gsp->f_E_jingdu) ;
	  printf("3002") ; 				//。
	  printf("76F24EBA8DCC50128BF75C3D5FEB655152A9FF01FF01FF01") ; //盲人跌倒请尽快救助！！



    USART_SendData(USART1, 0x1a);
}

void show_oled(void)
{
	static uint32_t oledcount = 0 ;
  char TS[14] = {0} ;

	

	static u8 gsm_flg = 0;
	
	
	
	

//　／／	×１００　／4096
	
	

	if((GetTick() -  oledcount ) > 1000)		//500ms一测，控制测量次数
	{	
		oledcount = GetTick();
		

	
		if(ADC_ConvertedValue[1]>500)  //检测积水
		{
			jishui_time++;
			if(jishui_time==1)
				 paly_sound(2);//前方路面有水，注意避障
			
			 zhen_dong= 1;  //震动0关  1开
		}
		else
		{
			jishui_time = 0;
			 zhen_dong= 0;   //震动 0关  1开
		}
		
     juli = Hcsr04GetLength();
//		juli = ADC_ConvertedValue[1]/10;
		sprintf(TS,"距离:%3d %3d ",juli,set_juli) ;
		OLED_ShowCH(0,0,(u8*)TS);	/*在第1页，第1列，显示一串16x16点阵汉字或8x16的ASCII字*/
		
		if(juli<set_juli)
		{
			juli_time++;
			if(juli_time>10)
			{
				juli_time = 0;
				
			}
			if(juli_time==1)
				paly_sound(3);//前方有障碍物，注意避障
		}
		else
		{
			juli_time = 0;
		}
		
		light = ADC_ConvertedValue[0]*100/2662;
		if(light>100)
			light= 0;
		else
			light = 100-light;
		sprintf(TS,"光照:%3d %3d",light,set_light) ;
		OLED_ShowCH(0,2,(u8*)TS);	/*在第1页，第1列，显示一串16x16点阵汉字或8x16的ASCII字*/
			
		if(light<set_light)
			open_led();
		else
			close_led();
		
		if(Get_ADXL345())
		{
		
			OLED_ShowCH(128-18,0,"跌");	/*在第1页，第1列，显示一串16x16点阵汉字或8x16的ASCII字*/
			OLED_ShowCH(128-18,2,"倒");	/*在第1页，第1列，显示一串16x16点阵汉字或8x16的ASCII字*/
			if(gsm_flg==0 && time_1ms>10000) //大于10秒
			{
				paly_sound(1);
			  send_gsm3();
				gsm_flg = 1;
			}
			
			if(time_1ms>15000)
			{
				paly_sound(1);
				time_1ms = 1000;
			}

		}
		else
		{
	
			OLED_ShowCH(128-18,0,"正");	/*在第1页，第1列，显示一串16x16点阵汉字或8x16的ASCII字*/
			OLED_ShowCH(128-18,2,"常");	/*在第1页，第1列，显示一串16x16点阵汉字或8x16的ASCII字*/
			time_1ms = 0;

			gsm_flg = 0;
		}	
		


		    sprintf(TS,"N:%s",(u8*)Gsp->s_N_weidu) ;
		    OLED_ShowCH(0,6,(u8*)TS);  /*显示一串16x16点阵汉字或8x16的ASCII字.以下雷同*/	
		
				sprintf(TS,"E:%s",(u8*)Gsp->s_E_jingdu) ;
		    OLED_ShowCH(0,4,(u8*)TS);	/*在第1页，第1列，显示一串16x16点阵汉字或8x16的ASCII字*/
		
	//	UART_SendStr(USART1 ,Gsp->s_N_weidu,12); //发送经度		
		
	}

	
}
//一键报警
//void task_key(void)
//{
// 	if(key2==0)
//	   send_gsm3();
//}

//一键报警
void task_key(void)
{
	u8 key = KEY_Scan();
	
	if(key==1)
	{
		set_flg++;
		 if(set_flg>2)
			 set_flg = 0;
	}
	
	if(set_flg==1) //调距离
	{
		OLED_ShowCH(64,0,">");
		OLED_ShowCH(64,2," ");
		
		if(key==2)
			set_juli++;
		else if(key==3)
		  set_juli--;
	}
	else if(set_flg==2) //调光照
	{
		OLED_ShowCH(64,0," ");
		OLED_ShowCH(64,2,">");		
		
		if(key==2)
			set_light++;
		else if(key==3)
			set_light--;
	}	
	else
	{
		OLED_ShowCH(64,0," ");
		OLED_ShowCH(64,2," ");				
	}
 	if(key==4)   //按键4一键报警
	   send_gsm3();
}


int main(void)
 {	
	 
   Gsp = Gsp_GetData();

	 
	delay_init();	    	 //延时函数初始化
	NVIC_Configuration();//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	 //使能GPIOB时钟和复用功能（要先打开复用才能修改复用功能）
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);						 //要先开时钟，再重映射；这句表示关闭JTAG，使能SWD。	 
//  BUZZER_Init();
	delay_ms(500);//延时	 
	KEY_Init();
	 LED_Init();
	 zhendong_Init();
	ADCx_Init();
	 Hcsr04Init();
	uart_init(9600);
	uart2_init(9600);	 
	uart3_init(9600);	
   SET_JQ8400_SOUND(30);	 
  OLED_Init();	
	OLED_Clear();	 
	OLED_ShowCH(0,0,"wait");
	 ini_sim900A_en(); 

  Hcsr04GetLength();
   ADXL345_Init();
	 
   zhen_dong= 0;
	TIM3_Int_Init(9,7199);//10Khz的计数频率，计数到10为1ms
//  paly_sound(1);
	while(1)
	{  
		  task_key();
		  show_oled();
	    if(Gsp->GPS_flg==1)		//gps有效
      {
				Gsp->GPS_flg=0;					
				dealGps();	
			UART_SendStr(USART1 ,Gsp->s_E_jingdu,12); //发送纬度
			UART_SendStr(USART1 ,Gsp->s_N_weidu,12); //发送经度		
			//	printf("%.0f\r\n",Gsp->f_E_jingdu);
			}
      Angle();
		


	}
}
