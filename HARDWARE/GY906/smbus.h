
#ifndef __SMBUS_H
#define __SMBUS_H	 

#include "sys.h"

/* SMBUS总线引脚资源在STM32上的映射 */
#define SMBUS_APB2Periph_GPIOx		RCC_APB2Periph_GPIOA
#define SMBUS_GPIOx 				GPIOB
#define SMBUS_SCL 					GPIO_Pin_0
#define SMBUS_SDA 					GPIO_Pin_1

/* 设置SDA引脚模式 */
#define SMBUS_SDA_IN()  {GPIOB->CRH&=0XFFF0FFFF;GPIOA->CRH|=8<<16;}
#define SMBUS_SDA_OUT() {GPIOB->CRH&=0XFFF0FFFF;GPIOA->CRH|=3<<16;}

/* 读写SMBUS引脚宏 */ 
#define SMBUS_WRITE_SCL    PBout(11) 		
#define SMBUS_WRITE_SDA    PBout(12) 			 
#define SMBUS_READ_SDA     PBin(12) 	

/*
 * 函数名：vSMBus_Init
 * 描述  ：初始化SMBUS
 * 输入  ：无
 * 输出  ：无
 * 返回  ：无
 * 调用  ：无 
 * 举例  ：无
 */
void vSMBus_Init(void);

/* 向从机写数据流 */
uint8_t ucSMBUS_StreamWrite(uint8_t ucSlaveAddr, uint8_t ucRegAddr \
							, uint8_t *pucRegDataBuf, uint16_t uiRegDataBufLength) ;

/* 从从机读入数据流 */
uint8_t ucSMBUS_StreamRead(uint8_t ucSlaveAddr, uint8_t ucRegAddr \
							, uint8_t *pucRegDataBuf, uint16_t uiRegDataBufLength) ;

#endif


