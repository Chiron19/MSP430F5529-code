/*
 * key.c
 *
 *  Created on: 2018-4-12
 *
 */
#include "key.h"
#include <msp430.h>

StrKeyFlag KeyFlag;

void delay(uint16 t)
{
	uint16 i,j,k;

	for(j=t;j>0;j--)
		for(i=250;i>0;i--)
			k=0;
}

/************************IO口初始化********************************/

void IO_Init(void)
{
	P8DIR |= BIT1;              			// 设置P8.1口为输出模式  控制LED灯
	P8OUT &= ~BIT1;

	P3DIR |= BIT6;              			// 设置P3.6口为输出模式  控制蜂鸣器
	P3OUT &= ~BIT6;

	P2DIR &= ~BIT6;
	P2DIR &= ~BIT3;                         // 设置P2.3,P2.6口为输入方向
	P2REN |= BIT3 + BIT6;                   // P2.3,P2.6口上拉电阻使能
	P2OUT |= BIT3 + BIT6;                   // 选中P2.3,P2.6为上拉电阻功能方式

	P1DIR &= ~BIT2;
	P1DIR &= ~BIT3;                        	// 设置P1.2,P1.3口为输入方向
	P1REN |= BIT2 + BIT3;                   // P1.2,P1.3口上拉电阻使能
	P1OUT |= BIT2 + BIT3;					// 选中P1.2,P1.3为上拉电阻功能方式
}

void ScanKey(void)
{

	{
		if(!(P1IN & BIT2))				//通过IO口值，得出按键按下信息 P1.2口按下
		{
			delay(50);						//延时去抖，再判断一次 P1.2口值
			if(!(P1IN & BIT2))			//再判断一次 P1.2口值
			{
				KeyFlag.S1=1;
			}
		}
		if(!(P1IN & BIT3))               // P1.3口按下
		{
			delay(50);
			if(!(P1IN & BIT3))
			{
				KeyFlag.S2=1;
			}
		}
		if(!(P2IN & BIT3))               //P2.3口按下
		{
			delay(50);
			if(!(P2IN & BIT3))
			{
				KeyFlag.S3=1;
			}
		}
		if(!(P2IN & BIT6))              //P2.6口按下
		{
			delay(50);
			if(!(P2IN & BIT6))
			{
				KeyFlag.S4=1;
			}
		}
	}
}
