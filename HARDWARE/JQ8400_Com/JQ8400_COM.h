#ifndef __JQ8400_COM_H
#define __JQ8400_COM_H

#include "sys.h"


//设置声音大小  num 0-30
// 例如：AA 13 01 14 D2 设置音量为 20 级
void SET_JQ8400_SOUND(u8 num);
//例如: AA 07 02 00 08 BB 指定播放当前盘符第 8 首，
//播放指定声音
void paly_sound(u8 num);
	


#endif

