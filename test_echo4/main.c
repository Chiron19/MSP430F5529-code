#include<msp430.h>
#define aaa 100
#define signalDelay 1000000

/*超声波模块接线
 * 左Trig、右Trig  --> P1.2
 * 左Echo          --> P1.4
 * 右Echo          --> P1.5
 */


unsigned int kk=0;
unsigned int LastCCR4;      //上一次高电平持续的时间，和距离正相关
unsigned int dat2[20];       //通过数组备份采集到的数据
unsigned char times_TACCR4=1;

void initLED(void)
{
    P8DIR |= BIT1;
    P3DIR |= BIT7;
    P7DIR |= BIT4;
    P6DIR |= BIT3;
}
/*跑马灯全部熄灭*/
void resetLED(void)
{
    P8OUT &= ~BIT1;
    P3OUT &= ~BIT7;
    P7OUT &= ~BIT4;
    P6OUT &= ~BIT3;
}
/*跑马灯全部点亮*/
void igniteLED(void)
{
    P8OUT |= BIT1;
    P3OUT |= BIT7;
    P7OUT |= BIT4;
    P6OUT |= BIT3;
}
/*从左到右跑马灯*/
void runLED0(int g)
{
    int p;
    P8OUT |= BIT1;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P8OUT &= ~BIT1;
    P3OUT |= BIT7;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P3OUT &= ~BIT7;
    P7OUT |= BIT4;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P7OUT &= ~BIT4;
    P6OUT |= BIT3;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P6OUT &= ~BIT3;
}
/*从右到左跑马灯*/
void runLED1(int g)
{
    int p;
    P6OUT |= BIT3;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P6OUT &= ~BIT3;
    P7OUT |= BIT4;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P7OUT &= ~BIT4;
    P3OUT |= BIT7;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P3OUT &= ~BIT7;
    P8OUT |= BIT1;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P8OUT &= ~BIT1;
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
}

/*初始化捕获模式，获取时间*/
void TimeGetInit()
{
    P1DIR &= ~BIT5; //P1.5选择input方向
    P1SEL |= BIT5;  //P1.5选择外设，对应TA0CCR4

    TA0CTL = TASSEL__SMCLK + ID_0 + MC_2 + TACLR + TAIE + TAIFG;        //时钟为SMCLK，Continuous up模式，开始时清零计数器
//    TA0CCTL3 = CM_1 + SCS + CAP + CCIE + CCIS_0;               //捕获模式：上升沿，捕获中断使能，捕获输入：CCI0A
    TA0CCTL4 = CM_1 + SCS + CAP + CCIE + CCIS_0;               //捕获模式：上升沿，捕获中断使能，捕获输入：CCI0A
}
/*手势判断*/
void signalJudge(void)
{
    if (kk > 100) {
        __disable_interrupt();
        resetLED();
        runLED1(kk);
        __enable_interrupt();
    }
}

void main()
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    HC_SR04Init();          //初始化超声模块HC_SR04
    TimeGetInit();          //初始化计数捕获引脚，模式等
    initLED();
    resetLED();
    __enable_interrupt();
    for(;;)
    {
        HC_SR04Start();
        signalJudge();
        LPM0;               //低功耗模式
    }
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void)
{
    __disable_interrupt();                        //关中断
    static unsigned int ii=0;
    unsigned int j;
    unsigned int t;

        if(times_TACCR4 == 1)
        {
            LastCCR4 = TA0CCR4;       //记录下上次CCR4的值
            TA0CCTL4 &= ~CM_1;        //清上升沿捕获
            TA0CCTL4 |= CM_2;         //改为下降沿捕获
            times_TACCR4++;
        }
        if(times_TACCR4 == 0)
        {
            if(ii < 20)                //把采集到的20次的值都放到数组中
            {
                dat2[ii] = TA0CCR4 - LastCCR4;
                ii++;
            }
            if(ii >= 20)               //为了防止误差，排序把采集到的两头两尾的值去掉，取中间的平均值
            {
                for(ii=0;ii<20;ii++)
                {
                    for(j=ii+1;j<20;j++)
                    {
                        if(dat2[ii]>dat2[j])
                        {
                            t=dat2[j];
                            dat2[j]=dat2[ii];
                            dat2[ii]=t;
                        }
                    }
                }
                t = 0;
                for(ii=7;ii<15;ii++)  t += dat2[ii];
                kk = t/8;
                ii = 0;
            }
            TA0CCTL4 &= ~CM_2;      //清除下降沿触发
            TA0CCTL4 |= CM_1;       //变为上升沿触发
            times_TACCR4++;         //改变times_TACCR4的值
        }
        times_TACCR4 &= 0x01;       //times_TACCR4>1时清0
        TA0CCTL4 &= ~CCIFG;         //清除中断响应标志

    LPM0_EXIT;                  //退出低功耗模式
    __enable_interrupt();       //开中断
}
