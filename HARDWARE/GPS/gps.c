
#include "gps.h"

unsigned int record1=0;   
unsigned char	devide_flag;		        //GPS数据逗号分隔符标志
unsigned char	speed_end;			//收到速度数据结束标志
unsigned char	dir_end;			//收到方向数据结束标志


unsigned char recv1_step=STAGE_SOHE;                       //串口接收指令步骤
unsigned char uart1_r_buf;                       //串口缓存
unsigned char rev1_buf_busy;                    //串口判忙
unsigned char temp1_buf[85];                   //串口接收数组


unsigned char  ew_flag;                        //东西标志
unsigned char  ns_flag;                        //南北标志
	








unsigned long Mid_Du;       //经纬度处理 度
unsigned long Mid_Fen;      //经纬度处理  分
unsigned long Mid_Vale;     //经纬度处理 中间变量






GPS_DataStructure gspdata;


void ini_GPS(void)
{
  char Lin0_No[16]="N:000.000000";//存储纬度
  char Lin1_Ea[16]="E:000.000000";//存储经度
  u8 i = 0;
	
	for(i=0;i<16;i++)
	{
		gspdata.s_N_weidu[i] = Lin0_No[i];
		gspdata.s_E_jingdu[i] = Lin1_Ea[i];
		
	}

	
}

//---
/*
	函数名：GPS_DataStructure* Gsp_GetData(void)
	描述：获取返回Gsp.c文件全局变量结构体变量
*/
GPS_DataStructure* Gsp_GetData(void)
{
	return &gspdata;
}


void dealGps(void)
{		
	Mid_Du=(gspdata.weijing[0]-0x30)*10000000+(gspdata.weijing[1]-0x30)*1000000;    //处理经度扩大10000000
	
	Mid_Fen=(gspdata.weijing[2]-0x30)*10000000+(gspdata.weijing[3]-0x30)*1000000+
		(gspdata.weijing[4]-0x30)*100000+(gspdata.weijing[5]-0x30)*10000+
			(gspdata.weijing[6]-0x30)*1000+(gspdata.weijing[7]-0x30)*100;          
	Mid_Fen=Mid_Fen/60;                                                      //分秒换算为小数位
	Mid_Vale=Mid_Du+Mid_Fen;         //最终为度格式000.00000000 非度分秒格式
	
	gspdata.f_N_weidu = (float)Mid_Vale/1000000;

	
                  
	gspdata.s_N_weidu[0]=Mid_Vale/10000000+0x30;                  //转化为字符
	gspdata.s_N_weidu[1]=(Mid_Vale/1000000)%10+0x30;
	gspdata.s_N_weidu[2]='.';
	gspdata.s_N_weidu[3]=(Mid_Vale/100000)%10+0x30;
	gspdata.s_N_weidu[4]=(Mid_Vale/10000)%10+0x30;
	gspdata.s_N_weidu[5]=(Mid_Vale/1000)%10+0x30;
	gspdata.s_N_weidu[6]=(Mid_Vale/100)%10+0x30;
	gspdata.s_N_weidu[7]=(Mid_Vale/10)%10+0x30;
	gspdata.s_N_weidu[8]=Mid_Vale%10+0x30;

	Mid_Du=(gspdata.weijing[8]-0x30)*100000000+(gspdata.weijing[9]-0x30)*10000000+(gspdata.weijing[10]-0x30)*1000000; //处理经度扩大10000000     

	Mid_Fen=(gspdata.weijing[11]-0x30)*10000000+(gspdata.weijing[12]-0x30)*1000000+
		(gspdata.weijing[13]-0x30)*100000+(gspdata.weijing[14]-0x30)*10000+
		(gspdata.weijing[15]-0x30)*1000+(gspdata.weijing[16]-0x30)*100; 
	Mid_Fen=Mid_Fen/60;                                                //分秒换算为小数位
	Mid_Vale=Mid_Du+Mid_Fen;                                          //最终为度格式000.00000000 非度分秒格式
		
 // printf("N:%ld",Mid_Vale);	  //22.993018E:113.395046
	gspdata.f_E_jingdu = (float)Mid_Vale/1000000;

	
   
	gspdata.s_E_jingdu[0]=Mid_Vale/100000000+0x30;                           //转化为字符
	gspdata.s_E_jingdu[1]=(Mid_Vale/10000000)%10+0x30;
	gspdata.s_E_jingdu[2]=(Mid_Vale/1000000)%10+0x30;
	gspdata.s_E_jingdu[3]='.';
	gspdata.s_E_jingdu[4]=(Mid_Vale/100000)%10+0x30;
	gspdata.s_E_jingdu[5]=(Mid_Vale/10000)%10+0x30;
	gspdata.s_E_jingdu[6]=(Mid_Vale/1000)%10+0x30;
	gspdata.s_E_jingdu[7]=(Mid_Vale/100)%10+0x30;
	gspdata.s_E_jingdu[8]=(Mid_Vale/10)%10+0x30;
	gspdata.s_E_jingdu[9]=Mid_Vale%10+0x30;
}



void GPS_IRQHandler(u8 rbuf)
{
    uart1_r_buf=rbuf;                      //提取接收buf
	  rev1_buf_busy=0x00;                         //判断 防止break问题
	  switch(recv1_step)
	  {
	  case STAGE_SOHE: if(uart1_r_buf == '$')     //具体参考GPS标准协议NMEA0183
	  {
	    rev1_buf_busy=0x01;
	    if(uart1_r_buf == '$')              //查看收到$
	    {
	      recv1_step=STAGE_TYPE;            //跳转到下一步
	      record1=0;                        //计数清零
	    }
	    else
	    {
	      recv1_step=STAGE_SOHE;        //恢复初始化
	      record1=0;
	    }
	  }
	  break;
	  case STAGE_TYPE: if(rev1_buf_busy == 0x00)
	  {

	    rev1_buf_busy=0x01;
	    temp1_buf[record1]=uart1_r_buf;
	    record1++;
	    if(record1 == 0x05)
	    {                                                 //确认开头$GPRMC
//	      if((temp1_buf[0] == 'G') && (temp1_buf[1] == 'P') && (temp1_buf[2] == 'R') && (temp1_buf[3] == 'M') && (temp1_buf[4] == 'C'))
	      if((temp1_buf[0] == 'G') && (temp1_buf[1] == 'N') && (temp1_buf[2] == 'R') && (temp1_buf[3] == 'M') && (temp1_buf[4] == 'C'))					
	      {
	        recv1_step=STAGE_NONE;    //跳转到下一步
	        record1=0;
	      } 
	      else
	      {
	        recv1_step=STAGE_SOHE;//恢复初始化
	        record1=0;
	      }
	    }
	  }
	  break;                                    //接收数据格式 $GNRMC,235952.000,A,2259.57139,N,11323.69061,E,0.00,0.00,280923,,,A,V*0D
	  case STAGE_NONE: if(rev1_buf_busy == 0x00)//接收数据格式:$GPRMC,054347.00,A,3202.04770,N,11846.23632,E,0.000,0.00,221013,,,A*67
	  {
	    rev1_buf_busy=0x01;
	    record1++;
	    if((record1 > 0x01) && (record1 < 0x08))                                                                                    
	    {
	      gspdata.time[record1-2]=uart1_r_buf;			//存储时间					
	    }
	    if((uart1_r_buf == ',') && (record1 > 0x07) && (record1 < 0x010))   //||((uart1_r_buf == ',') && (record1==0x02))
	    {
	      record1=0xcc;
	    }
	    if(record1 ==  0xcd)
	    {
	      record1=0;
	      devide_flag=2;
	      speed_end=0x00;
	      dir_end=0x00;
	      if(uart1_r_buf == 'A')  //gps收到数据 且有效
	      { 
	        recv1_step=STAGE_DATA;    //跳转到下一步
	      }
	      else
	      {
	        gspdata.GPS_flg=0;
	        recv1_step=STAGE_SOHE;    //无效恢复初始化
	        record1=0;
	      }
	    }
	  }
	  break;
	  case STAGE_DATA:  if(rev1_buf_busy == 0x00)
	  {
	    rev1_buf_busy=0x01;
	    record1++;
	    if(uart1_r_buf == ',')    //判断为逗号
	    { 
	      devide_flag++;      //逗号 次数记录
	      record1=0;
	    }
	    if(devide_flag == 3)
	    {
	      if((record1 > 0) && (record1 < 5))
	      {
	        gspdata.weijing[record1-1]=uart1_r_buf;	    //存储经纬度 此处为纬度					
	      }
	      if((record1 > 5) && (record1 < 10))             //跳过小数点的存储
	      {
	        gspdata.weijing[record1-2]=uart1_r_buf;	   //存储经纬度 此处为纬度														
	      }
	    }
	    if(devide_flag == 4)
	    {
	      if(record1 > 0)
	      {
	        ns_flag=uart1_r_buf;            //接受纬度 NS标志
	      }
	    }
	    if(devide_flag == 5)
	    {
	      if((record1 > 0) && (record1 < 6))
	      {
	        gspdata.weijing[record1+7]=uart1_r_buf;	  //跳过小数点的存储										
	      }
	      if((record1 > 6) && (record1 < 11))                //
	      {
	        gspdata.weijing[record1+6]=uart1_r_buf;       //存储经纬度	 此处为经度																	
	      }
	    }
	    if(devide_flag == 6)
	    {
	      if(record1 > 0)
	      {
	        ew_flag=uart1_r_buf;            //经纬度 EW标志
	      }
	    }
	    if(devide_flag == 7)
	    {
	      if(speed_end == 0x00)
	      {
	        if((record1 > 0) && (uart1_r_buf != '.'))
	        {
	          gspdata.speed[record1-1]=uart1_r_buf;   //接受速率
	        }
	        else if(uart1_r_buf == '.')
	        {
	          record1--;
	          speed_end=0xff;
	        }
	      }
	      else if(speed_end == 0xff)
	      {
	        speed_end=0xfe;
	        gspdata.speed[record1-1]=uart1_r_buf;
	        gspdata.speed[3]=gspdata.speed[record1-1];
	        gspdata.speed[2]=gspdata.speed[record1-2];
	        if(record1 > 2)
	        {
	          gspdata.speed[1]=gspdata.speed[record1-3];
	        }
	        else
	        {
	          gspdata.speed[1]=0x30;
	        }
	        if(record1 > 3)
	        {
	          gspdata.speed[0]=gspdata.speed[record1-4];
	        }
	        else
	        {
	          gspdata.speed[0]=0x30;
	        }
	      }
	    }
	    if(devide_flag == 8)
	    {
	      if(dir_end == 0x00)
	      {
	        if((record1 > 0) && (uart1_r_buf != '.'))
	        {
	          gspdata.dir[record1-1]=uart1_r_buf;   //存储方向
	        }
	        else if(uart1_r_buf == '.')
	        {
	          record1--;
	          dir_end=0xff;
	        }
	      }
	      else if(dir_end == 0xff)
	      {
	        dir_end=0xfe;
	        if(record1 == 2)
	        {
	          gspdata.dir[2]=gspdata.dir[record1-2];
	          gspdata.dir[1]=0x30;
	          gspdata.dir[0]=0x30;
	        }
	        if(record1 == 3)
	        {
	          gspdata.dir[2]=gspdata.dir[record1-2];
	          gspdata.dir[1]=gspdata.dir[record1-3];
	          gspdata.dir[0]=0x30;
	        }
	      }
	    }
	    if(devide_flag == 9)
	    {
	      if((record1 > 0) && (record1 < 7))
	      {
	        gspdata.date[record1-1]=uart1_r_buf;
	      }
	    }
	    if(uart1_r_buf == 0x0d)
	    {
	      recv1_step=STAGE_SOHE;    //接受完成 信号确定
	      record1=0;                //恢复初始化状态 为下一次做准备
	      devide_flag=0;
	      gspdata.GPS_flg=1;         //GPS信号有效
	    }
	  }
	  break;
	  }				
}

