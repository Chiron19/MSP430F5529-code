/*
 * AD.c
 *
 *  Created on: 2014-7-15
 *      Author: dean
 */

#include "ad.h"

void AD_Init(void)
{
	ADC12CTL0 |= ADC12MSC;									//自动循环采样转换
	ADC12CTL0 |= ADC12ON;									//启动ADC12模块
	ADC12CTL1 |= ADC12CONSEQ_3;								//选择单通道循环采样转换//选择序列通道多次循环采样转换
	ADC12CTL1 |= ADC12CSTARTADD_0;                          //开始地址使用寄存器0
	ADC12CTL1 |= ADC12SHP;									//采样保持模式
	ADC12MCTL0 |= ADC12INCH_5; 								////存储寄存器0选择通道P6.5，连接拨码电位器
	                                                        //存储寄存器1选择通道P6.0红外传感器电阻
    ADC12MCTL1 |= ADC12INCH_0 + ADC12EOS;//End of sequence回到寄存器0
	ADC12CTL0 |= ADC12ENC;                                  //ADC转换使能
	ADC12CTL0 |= ADC12SC;
}
