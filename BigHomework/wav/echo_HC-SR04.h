/*
 * echo_HC-SR04.h
 *
 *  Created on: 2021年8月25日
 *      Author: dssy2
 */
#ifndef ECHO_HC_SR04_H_
#define ECHO_HC_SR04_H_


#define aaa 800
#define signalDelay 1000000
#define sampleNum 12
#define cmpLow 100
#define cmpHigh 800
#define remoteBuf 12

/*超声波模块接线
 * 左Trig、右Trig  --> P1.2
 * 左Echo          --> P1.4
 * 右Echo          --> P1.5
 */

extern unsigned int valueCCR2, valueCCR4;
extern unsigned int i2;
extern unsigned int i4;
extern unsigned int LastCCR2, LastCCR4;      //上一次高电平持续的时间，和距离正相关
extern unsigned int dat1[20], dat2[20];       //通过数组备份采集到的数据
extern unsigned char times_TACCR2, times_TACCR4;
extern unsigned char modeSel[20];
extern unsigned int modeCnt;

void initLED(void);
void resetLED(void);
void igniteLED(void);
void igniteLED0(void);
void igniteLED1(void);
void igniteLED_single(int x);
void runLED0(int g);
void runLED1(int g);
void HC_SR04Init();
void HC_SR04Start();
void TimeGetInit();
void resetBuf(void);
void signalJudge(void);
void signalRes(void);

#endif /* ECHO_HC_SR04_H_ */
