#ifndef __GPS_H
#define __GPS_H
#include "sys.h"
#define STAGE_SOHE  0x01
#define STAGE_TYPE  0x02
#define STAGE_NONE  0x03
#define STAGE_DATA  0x04



//结构体
typedef struct
{
	unsigned char GPS_flg;        //0 没收到GPS信号 1收到GPS信号
	unsigned char	weijing[17];    //经纬度字符串
  unsigned char	speed[4];       //暂存速度
	unsigned char	time[6];        //暂存时间
	unsigned char	date[6];        //暂存日期
  unsigned char	dir[3];         //暂存方向	
  double f_E_jingdu;              //符点存经纬度数字
  double f_N_weidu;               //符点存经纬度数字
  char s_N_weidu[16];            //存储纬度
  char s_E_jingdu[16];           //存储经度

	
}GPS_DataStructure;



 
void ini_GPS(void);

void GPS_IRQHandler(u8 rbuf);   //放到串口中断里

GPS_DataStructure* Gsp_GetData(void);
void dealGps(void) ;//gps数据处理

#endif


