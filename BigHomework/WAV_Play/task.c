                                                                                                                            /*
 * task.c
 *
 *  Created on: 2014-7-16
 *      Author: Administrator
 */
#include "task.h"
#include "echo_HC-SR04.h"
#include <msp430.h>
StrKeyFlag KeyFlag;						//�������±�־

void delay1(UINT t)
{
	UINT i,j;

	for(j=t;j>0;j--)
		for(i=1;i>0;i--)
			__delay_cycles(1);
}

void TimeInit()
{
	TA0CTL |= MC_1 + TASSEL__SMCLK + TACLR + ID_2;     	//ʱ��ΪSMCLK,�Ƚ�ģʽ����ʼʱ���������
	TA0CCTL0 = CCIE;						//�Ƚ����ж�ʹ��
	TA0CCR0  = 10000;//32768;				//�Ƚ�ֵ��Ϊ50000���൱��50ms��ʱ����
}
/*��ʼ������ģʽ����ȡʱ��*/
void TimeGetInit()
{
    P1DIR &= ~BIT4; //P1.4ѡ��input����
    P1SEL |= BIT4;  //P1.4ѡ�����裬��ӦTA0CCR3

    P1DIR &= ~BIT5; //P1.5ѡ��input����
    P1SEL |= BIT5;  //P1.5ѡ�����裬��ӦTA0CCR4

//    TA0CTL =  + ID_0 + MC_2 + TACLR;        //ʱ��ΪSMCLK��Continuous upģʽ����ʼʱ���������
    TA0CCTL3 = CM_1 + SCS + CAP + CCIE;               //����ģʽ�������أ������ж�ʹ�ܣ��������룺CCI0A
    TA0CCTL4 = CM_1 + SCS + CAP + CCIE;               //����ģʽ�������أ������ж�ʹ�ܣ��������룺CCI0A
}

void IOInit()
{
	P3OUT |= BIT0+BIT1;
	P3DIR |= BIT0+BIT1;
	P3REN |= BIT0+BIT1;                            // Assign I2C pins to USCI_B0
	P8OUT |= BIT2;
	P8DIR |= BIT2;
	P8REN |= BIT2;
	P8OUT &= ~BIT2;
	P1DIR |= BIT4;              			// ����P8.1��Ϊ���ģʽ
	P1OUT |= BIT4;
	P2DIR |= BIT0+BIT1+BIT7;              			// ����P8.1��Ϊ���ģʽ
	P2OUT |= BIT0+BIT1+BIT7;							// ѡ��P8.1Ϊ�����ʽ
	P2REN |= BIT1+BIT7;							// P8.1ʹ��
	P2DIR |= BIT1;                        // ����P8.1��Ϊ���ģʽ
	P2OUT |= BIT1;                            // ѡ��P8.1Ϊ�����ʽ
	P2REN |= BIT1;                         // P8.1ʹ��
	P3DIR |= BIT2+BIT3+BIT4;              			// ����P3.6��Ϊ���ģʽ
	P3OUT |= BIT2+BIT3+BIT4;						// ѡ��P3.6Ϊ�����ʽ
	P3REN |= BIT2+BIT3+BIT4;					// P3.6ʹ��
    //���������ӦIO��ʼ��
    P6DIR &= ~BIT0;                             //Input
    P6SEL &= ~BIT0;                             //GPIO��ʽ
#ifdef NewBoard
	P2DIR &= ~(BIT3+BIT6);              			// ����P8.1��Ϊ���ģʽ
	P2OUT |= BIT3+BIT6;							// ѡ��P8.1Ϊ�����ʽ
	P2REN |= BIT3+BIT6;							// P8.1ʹ��
	P1DIR &=~( BIT3+BIT2);              			// ����P8.1��Ϊ���ģʽ
	P1OUT |= BIT3+BIT2;							// ѡ��P8.1Ϊ�����ʽ
	P1REN = BIT3+BIT2;							// P8.1ʹ��
	#else
	P1DIR &=~( BIT3+BIT2+BIT4+BIT5);              			// ����P8.1��Ϊ���ģʽ
	P1OUT |= BIT3+BIT2+BIT4+BIT5;							// ѡ��P8.1Ϊ�����ʽ
	P1REN = BIT3+BIT2+BIT4+BIT5;							// P8.1ʹ��
	#endif
}
void ScanKey(void)
{
	#ifndef NewBoard
	if((P1IN&0x3c)!=0x3c)				//ͨ��IO��ֵ�ó�����������Ϣ
	{
		delay1(50);						//��ʱȥ��
		if((P1IN&0x3c)!=0x3c)			//ͨ��IO��ֵ�ó�����������Ϣ
		{
			switch(P1IN&0x3c)
			{
				case 0x38 : KeyFlag.S1+=1; ;break;//S1��������
				case 0x1c : KeyFlag.S2=1; ;break;//S1��������
				case 0x2c : KeyFlag.S3=1; ;break;//S1��������
				case 0x34 : KeyFlag.S4=1 ;break;//S4��������
				default : break;
			}
		}
	}
	#else
	{
		if((P1IN&0x04)!=0x04)				//ͨ��IO��ֵ�ó�����������Ϣ//P1.2
		{
			delay1(5);						//��ʱȥ��
			if((P1IN&0x04)!=0x04)			//ͨ��IO��ֵ�ó�����������Ϣ
			{
				KeyFlag.S1^=1;
			}
		}
		if((P1IN&0x08)!=0x08)				//ͨ��IO��ֵ�ó�����������Ϣ//P1.3
		{
			delay1(5);						//��ʱȥ��
			if((P1IN&0x08)!=0x08)			//ͨ��IO��ֵ�ó�����������Ϣ
			{
				KeyFlag.S2=1;
			}
		}
		if((P2IN&0x08)!=0x08)				//ͨ��IO��ֵ�ó�����������Ϣ//P2.3
		{
			delay1(5);						//��ʱȥ��
			if((P2IN&0x08)!=0x08)			//ͨ��IO��ֵ�ó�����������Ϣ
			{
				KeyFlag.S3=1;
			}
		}
		if((P2IN&0x40)!=0x40)				//ͨ��IO��ֵ�ó�����������Ϣ//P2.6
		{
			delay1(5);						//��ʱȥ��
			if((P2IN&0x40)!=0x40)			//ͨ��IO��ֵ�ó�����������Ϣ
			{
				KeyFlag.S4=1;
			}
		}
	}
	#endif
}
unsigned int valueCCR3=0, valueCCR4=0;
unsigned int i3=0;
unsigned int i4=0;
unsigned int LastCCR2, LastCCR4;      //��һ�θߵ�ƽ������ʱ�䣬�;��������
unsigned int dat1[20], dat2[20];       //ͨ�����鱸�ݲɼ���������
unsigned char times_TACCR3=1, times_TACCR4=1;
unsigned char modeSel[20];
unsigned int modeCnt = 0;

/*������ģ�����
 * ��Trig����Trig  --> P1.6
 * ��Echo          --> P1.4
 * ��Echo          --> P1.5
 */


void initLED(void)
{
    P8DIR |= BIT1;  //L1: P8.1
//    P3DIR |= BIT7;  //L2: P3.7
//    P7DIR |= BIT4;  //L3: P7.4
//    P6DIR |= BIT3;  //L4: P6.3
//    P6DIR |= BIT4;  //L5: P6.4
//    P3DIR |= BIT5;  //L6: P3.5
}
/*�����ȫ��Ϩ��*/
void resetLED(void)
{
    P8OUT &= ~BIT1;
    P3OUT &= ~BIT7;
    P7OUT &= ~BIT4;
//    P6OUT &= ~BIT3;
//    P6OUT &= ~BIT4;
//    P3OUT &= ~BIT5;
}
/*�����ȫ������*/
void igniteLED(void)
{
    P8OUT |= BIT1;
    P3OUT |= BIT7;
    P7OUT |= BIT4;
//    P6OUT |= BIT3;
//    P6OUT |= BIT4;
//    P3OUT |= BIT5;
}
void igniteLED0(void)
{
    P8OUT |= BIT1;
    P3OUT |= BIT7;
    P7OUT |= BIT4;
}
void igniteLED1(void)
{
//    P6OUT |= BIT3;
//    P6OUT |= BIT4;
//    P3OUT |= BIT5;
}
void igniteLED_single(int x)
{
    switch(x){
        case 0: resetLED(); break;
        case 1: P8OUT |= BIT1; break;
        case 2: P3OUT |= BIT7; break;
        case 3: P7OUT |= BIT4; break;
//        case 4: P6OUT |= BIT3; break;
//        case 5: P6OUT |= BIT4; break;
//        case 6: P3OUT |= BIT5; break;
        default: break;
    }
}
/*�����������*/
void runLED0(int g)
{
    int p;
    P8OUT |= BIT1;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P8OUT &= ~BIT1;
    P3OUT |= BIT7;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P3OUT &= ~BIT7;
    P7OUT |= BIT4;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P7OUT &= ~BIT4;
//    P6OUT |= BIT3;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P6OUT &= ~BIT3;
//    P6OUT |= BIT4;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P6OUT &= ~BIT4;
//    P3OUT |= BIT5;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P3OUT &= ~BIT5;
}
/*���ҵ��������*/
void runLED1(int g)
{
    int p;
//    P3OUT |= BIT5;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P3OUT &= ~BIT5;
//    P6OUT |= BIT4;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P6OUT &= ~BIT4;
//    P6OUT |= BIT3;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P6OUT &= ~BIT3;
    P7OUT |= BIT4;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P7OUT &= ~BIT4;
    P3OUT |= BIT7;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P3OUT &= ~BIT7;
    P8OUT |= BIT1;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P8OUT &= ~BIT1;
}
//void runLED2(int g)
//{
//    int p;
//    P8OUT |= BIT1;  P3OUT |= BIT5; for (p=0; p<g; ++p) __delay_cycles(aaa);  P8OUT &= ~BIT1; P3OUT &= ~BIT5;
//    P3OUT |= BIT7;  P6OUT |= BIT4; for (p=0; p<g; ++p) __delay_cycles(aaa);  P3OUT &= ~BIT7; P6OUT &= ~BIT4;
//    P7OUT |= BIT4;  P6OUT |= BIT3; for (p=0; p<g; ++p) __delay_cycles(aaa);
//    P3OUT |= BIT7;  P6OUT |= BIT4; for (p=0; p<g; ++p) __delay_cycles(aaa);
//    P8OUT |= BIT1;  P3OUT |= BIT5; for (p=0; p<g; ++p) __delay_cycles(aaa);
//    resetLED();
//}
void delayCnt(int g)
{
    int p;
    for (p=0; p<g; ++p) __delay_cycles(1);
}
/*��ʼ��HC_SR04Init()ģ��*/
void HC_SR04Init()
{
    P1DIR |= BIT6;
}
/*��ʼ��������*/
void HC_SR04Start()
{
    P1OUT |= BIT6;
    __delay_cycles(12);      //���ٳ���10us�ĸߵ�ƽ
    P1OUT &= ~BIT6;
}


void resetBuf(void)
{
    __disable_interrupt();
    valueCCR3 = valueCCR4 = 0;
    i3 = i4 = 0;
    times_TACCR3 = times_TACCR4 = 1;
    __enable_interrupt();
}
/*�����ж�*/
void signalJudge(void)
{
    __disable_interrupt();
    if ((valueCCR4 > cmpLow && valueCCR3 > cmpLow) && (valueCCR4 < cmpHigh*0.8 && valueCCR3 < cmpHigh*0.8)) {
        modeSel[modeCnt] = 1;
//        resetLED();
//        runLED2(valueCCR3+valueCCR4);
//        __delay_cycles(signalDelay);
//        resetLED();
        resetBuf();
        modeCnt++;
//        KeyFlag.S1 ^= 1;
        __enable_interrupt();
        return;
    }
    if ((valueCCR3 > cmpLow && valueCCR4 <= cmpLow) && (valueCCR4 < cmpHigh && valueCCR3 < cmpHigh)) {
        modeSel[modeCnt] = 2;
//        resetLED();
//        runLED0(valueCCR3);
//        __delay_cycles(signalDelay);
//        resetLED();
        resetBuf();
        modeCnt++;
//        KeyFlag.S2 = 1;
        __enable_interrupt();
        return;
    }
    if ((valueCCR4 > cmpLow && valueCCR3 <= cmpLow) && (valueCCR4 < cmpHigh && valueCCR3 < cmpHigh)) {
        modeSel[modeCnt] = 3;
//        resetLED();
//        runLED1(valueCCR4);
//        __delay_cycles(signalDelay);
//        resetLED();
        resetBuf();
        modeCnt++;
//        KeyFlag.S3 = 1;
        __enable_interrupt();
        return;
    }

//    modeSel[modeCnt] = 0;
//    modeCnt++;
//
//    resetBuf();
    __enable_interrupt();
}
void signalRes(void)
{
    if (modeCnt < 9) return;
    int i, a, b, c;
    __disable_interrupt();
    modeCnt = 0;
    a=0;
    b=0;
    c=0;
    for (i=0; i<20; ++i) {
        if (modeSel[i] == 1) ++a;
        if (modeSel[i] == 2) ++b;
        if (modeSel[i] == 3) ++c;
    }
    if (a > 4) {
                resetLED();
                igniteLED();
                __delay_cycles(signalDelay);
                resetLED();
                KeyFlag.S1^=1;
    }
    else if (b > 4) {
                resetLED();
                runLED0(2000);
                __delay_cycles(signalDelay);
                resetLED();
                KeyFlag.S2 = 1;
    }
    else if (c > 3) {
                resetLED();
                runLED1(2000);
                __delay_cycles(signalDelay);
                resetLED();
                KeyFlag.S3 = 1;
    }
//    if (a+b+c > 1 && a < 15 && b < 15 && c < 15)
//    {
//        resetLED();
//        runLED0(1000);
//        runLED1(1000);
//        runLED0(1000);
//        __delay_cycles(signalDelay);
//        resetLED();
//    }
    resetBuf();
    __enable_interrupt();
}

int Filter(void)//��ֵ�˲���
{
    int i, j, t;
    int value_buf[remoteBuf];
    int sum = 0;

    for (i = 0; i < remoteBuf; i++)
    {
        value_buf[i] = ADC12MEM1; //���⴫�в���
        __delay_cycles(50);
    }


    for (j=0; j<remoteBuf-1; j++)
    {
        for (i=0; i<remoteBuf-j-1; i++)
        {
            if (value_buf[i] > value_buf[i+1])
            {
                int temp = value_buf[i];
                value_buf[i] = value_buf[i+1];
                value_buf[i+1] = temp;
            }
        }
    }
    for(i=1; i<remoteBuf-1; i++) sum += value_buf[i];
    t = sum/(remoteBuf-2);
    return t;
}
