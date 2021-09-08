#include<msp430.h>
#include <stdio.h>
unsigned int k=0;
#define aaa 100
unsigned int LastCCR1;      //上一次高电平持续的时间，和距离正相关
unsigned int dat[20];       //通过数组备份采集到的数据
unsigned char times=1;

void initLED(void)
{
    P8DIR |= BIT1;
    P3DIR |= BIT7;
    P7DIR |= BIT4;
    P6DIR |= BIT3;
}
void resetLED(void)
{
    P8OUT &= ~BIT1;
    P3OUT &= ~BIT7;
    P7OUT &= ~BIT4;
    P6OUT &= ~BIT3;
}
void igniteLED(void)
{
    P8OUT |= BIT1;
    P3OUT |= BIT7;
    P7OUT |= BIT4;
    P6OUT |= BIT3;
}
void runLED(void)
{
    int p;
    P8OUT |= BIT1;  for (p=0; p<k; ++p) __delay_cycles(aaa);  P8OUT &= ~BIT1;
    P3OUT |= BIT7;  for (p=0; p<k; ++p) __delay_cycles(aaa);  P3OUT &= ~BIT7;
    P7OUT |= BIT4;  for (p=0; p<k; ++p) __delay_cycles(aaa);  P7OUT &= ~BIT4;
    P6OUT |= BIT3;  for (p=0; p<k; ++p) __delay_cycles(aaa);  P6OUT &= ~BIT3;
}
/*初始化HC_SR04Init()模块*/
void HC_SR04Init()
{
    P1DIR |= BIT2;
}
/*开始超声发送*/
void HC_SR04Start()
{
    P1OUT |= BIT2;
    __delay_cycles(12);      //至少持续10us的高电平
    P1OUT &= ~BIT2;
//    __delay_cycles(100000);
}

/*初始化捕获模式，获取时间*/
void TimeGetInit()
{
    P1DIR &= ~BIT4; //P1.4选择input方向
    P1SEL |= BIT4;  //P1.4选择外设，对应TACCR3

    TA0CTL = TASSEL__SMCLK + ID_0 + MC_2 + TACLR + TAIE;        //时钟为SMCLK，Continuous up模式，开始时清零计数器
    TA0CCTL3 = CM_1 + SCS + CAP + CCIE + CCIS_0;               //捕获模式：上升沿，捕获中断使能，捕获输入：CCI0A
}

void main()
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    HC_SR04Init();          //初始化超声模块HC_SR04
    TimeGetInit();          //初始化计数捕获引脚，模式等
    initLED();
    resetLED();
    __enable_interrupt();
    while(1)
    {
        HC_SR04Start();
        if (k > 100) {
            __disable_interrupt();
            resetLED();
            runLED();
            __enable_interrupt();
        }
        LPM0;               //低功耗模式
    }
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void)
{
    __disable_interrupt();                        //关中断
    static unsigned int i=0;
    unsigned int j;
    unsigned int t;

    if(times == 1)
    {
        LastCCR1 = TA0CCR3;       //记录下上次CCR3的值
        TA0CCTL3 &= ~CM_1;        //清上升沿捕获
        TA0CCTL3 |= CM_2;         //改为下降沿捕获
        times++;
    }
    if(times == 0)
    {
        if(i < 20)                //把采集到的20次的值都放到数组中
        {
            dat[i] = TA0CCR3 - LastCCR1;
            i++;
        }
        if(i >= 20)               //为了防止误差，排序把采集到的两头两尾的值去掉，取中间的平均值
        {
            for(i=0;i<20;i++)
            {
                for(j=i+1;j<20;j++)
                {
                    if(dat[i]>dat[j])
                    {
                        t=dat[j];
                        dat[j]=dat[i];
                        dat[i]=t;
                    }
                }
            }
            t = 0;
            for(i=7;i<15;i++)  t += dat[i];
            k = t/8;
            i = 0;
        }
        TA0CCTL3 &= ~CM_2;      //清除下降沿触发
        TA0CCTL3 |= CM_1;       //变为上升沿触发
        times++;                //改变times的值
    }
    times &= 0x01;                //times>1时清0
    LPM0_EXIT;                  //退出低功耗模式
    TA0CCTL3 &= ~CCIFG;         //清除中断响应标志
    __enable_interrupt();                    //开中断
}
