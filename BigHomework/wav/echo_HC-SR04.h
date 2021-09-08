/*
 * echo_HC-SR04.h
 *
 *  Created on: 2021��8��25��
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

/*������ģ�����
 * ��Trig����Trig  --> P1.2
 * ��Echo          --> P1.4
 * ��Echo          --> P1.5
 */

extern unsigned int valueCCR2, valueCCR4;
extern unsigned int i2;
extern unsigned int i4;
extern unsigned int LastCCR2, LastCCR4;      //��һ�θߵ�ƽ������ʱ�䣬�;��������
extern unsigned int dat1[20], dat2[20];       //ͨ�����鱸�ݲɼ���������
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
