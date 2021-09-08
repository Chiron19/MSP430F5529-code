/*
 * LEDboard.h
 *
 *  Created on: 2021年8月30日
 *      Author: dssy2
 */

#ifndef LEDBOARD_H_
#define LEDBOARD_H_


#include <msp430.h>
#define XT2_FREQ    4000000
#define MCLK_FREQ   16000000
#define SMCLK_FREQ  4000000
#define ACLK_FREQ   32000
/**
 *
 *共阳接法；按列扫描
 */
//测试代码
//2.0 2.2 /7.4/ 3.0 3.1 2.6 /2.3/ /8.1/  2.5 2.4 4.3
//1.3 1.2 2.3 2.6  8.7 3.7 7.4 6.3 6.4 3.5 6.2 3.6 1.4 1.5 1.6 6.0
//8.7 3.7 7.4 6.3 6.4 3.5
//行设置低电平 ,row  阴极低电压导通 //两节带
#define r81 P4DIR|=BIT0; P4SEL&=~BIT0; P4OUT&=~BIT0; //灰4.0 紫4.3 蓝6.6 绿4.2 黄4.1 橙6.4 红3.5 棕7.0
#define r71 P4DIR|=BIT3; P4SEL&=~BIT3; P4OUT&=~BIT3;
#define r61 P6DIR|=BIT6; P6SEL&=~BIT6; P6OUT&=~BIT6;
#define r51 P4DIR|=BIT2; P4SEL&=~BIT2; P4OUT&=~BIT2;
#define r41 P4DIR|=BIT1; P4SEL&=~BIT1; P4OUT&=~BIT1;
#define r31 P6DIR|=BIT4; P6SEL&=~BIT4; P6OUT&=~BIT4;
#define r21 P3DIR|=BIT5; P3SEL&=~BIT5; P3OUT&=~BIT5;
#define r11 P7DIR|=BIT0; P7SEL&=~BIT0; P7OUT&=~BIT0;
//行设置高电平,row   阴极高低电压不导通
#define r80 P4DIR|=BIT0; P4SEL&=~BIT0; P4OUT|=BIT0;
#define r70 P4DIR|=BIT3; P4SEL&=~BIT3; P4OUT|=BIT3;
#define r60 P6DIR|=BIT6; P6SEL&=~BIT6; P6OUT|=BIT6;
#define r50 P4DIR|=BIT2; P4SEL&=~BIT2; P4OUT|=BIT2;
#define r40 P2DIR|=BIT7; P2SEL&=~BIT7; P2OUT|=BIT7;
#define r30 P6DIR|=BIT4; P6SEL&=~BIT4; P6OUT|=BIT4;
#define r20 P3DIR|=BIT5; P3SEL&=~BIT5; P3OUT|=BIT5;
#define r10 P7DIR|=BIT0; P7SEL&=~BIT0; P7OUT|=BIT0;
//列设置高电平,line   //         棕2.4 红8.1 橙3.7 黄2.5 绿7.4 蓝6.3 紫2.2 灰3.1
#define l8 P2DIR|=BIT4; P2OUT|=BIT4; P8OUT&=~BIT2; P3OUT&=~BIT7; P2OUT&=~BIT5; P7OUT&=~BIT4; P6OUT&=~BIT3; P2OUT&=~BIT2; P3OUT&=~BIT1; //
#define l7 P8DIR|=BIT1; P8OUT|=BIT1; P2OUT&=~BIT4; P3OUT&=~BIT7; P2OUT&=~BIT5; P7OUT&=~BIT4; P6OUT&=~BIT3; P2OUT&=~BIT2; P3OUT&=~BIT1;
#define l6 P3DIR|=BIT7; P3OUT|=BIT7; P2OUT&=~BIT4; P8OUT&=~BIT1; P2OUT&=~BIT5; P7OUT&=~BIT4; P6OUT&=~BIT3; P2OUT&=~BIT2; P3OUT&=~BIT1;
#define l5 P2DIR|=BIT5; P2OUT|=BIT5; P2OUT&=~BIT4; P8OUT&=~BIT1; P3OUT&=~BIT7; P7OUT&=~BIT4; P6OUT&=~BIT3; P2OUT&=~BIT2; P3OUT&=~BIT1;
#define l4 P7DIR|=BIT4; P7OUT|=BIT4; P2OUT&=~BIT4; P8OUT&=~BIT1; P3OUT&=~BIT7; P2OUT&=~BIT5; P6OUT&=~BIT3; P2OUT&=~BIT2; P3OUT&=~BIT1;
#define l3 P6DIR|=BIT3; P6OUT|=BIT3; P2OUT&=~BIT4; P8OUT&=~BIT1; P3OUT&=~BIT7; P2OUT&=~BIT5; P7OUT&=~BIT4; P2OUT&=~BIT2; P3OUT&=~BIT1;
#define l2 P2DIR|=BIT2; P2OUT|=BIT2; P2OUT&=~BIT4; P8OUT&=~BIT1; P3OUT&=~BIT7; P2OUT&=~BIT5; P7OUT&=~BIT4; P6OUT&=~BIT3; P3OUT&=~BIT1;
#define l1 P3DIR|=BIT1; P3OUT|=BIT1; P2OUT&=~BIT4; P8OUT&=~BIT1; P3OUT&=~BIT7; P2OUT&=~BIT5; P7OUT&=~BIT4; P6OUT&=~BIT3; P2OUT&=~BIT2;
#define SE P2SEL&=~BIT4; P8SEL&=~BIT2; P3SEL&=~BIT7; P2SEL&=~BIT5; P7SEL&=~BIT4; P6SEL&=~BIT3; P2SEL&=~BIT2; P3SEL&=~BIT1;
//时钟部分
extern int timerFlg;
extern int CLK;

/* 图案数组第一位表示有效数字
 * 第二位表示模式   1为滚动显示，2为正放逐帧显示，3为倒放逐帧显示
 */
extern unsigned int dat197[199];
extern unsigned int dat112[114];
extern unsigned int dat272[274];
extern unsigned int dat320[322];
extern unsigned int dat248[250];

#endif /* LEDBOARD_H_ */
