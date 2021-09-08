#include<msp430.h>
#define aaa 100
#define signalDelay 1000000
#define sampleNum 9

/*超声波模块接线
 * 左Trig、右Trig  --> P1.2
 * 左Echo          --> P1.4
 * 右Echo          --> P1.5
 */

unsigned int k=0, kk=0;
unsigned int i=0;
unsigned int ii=0;
unsigned int LastCCR2, LastCCR4;      //上一次高电平持续的时间，和距离正相关
unsigned int dat1[20], dat2[20];       //通过数组备份采集到的数据
unsigned char times_TACCR3=1, times_TACCR4=1;
unsigned char modeSel[5] = {0,0,0,0,0};
unsigned int modeCnt = 0;

void initLED(void)
{
    P8DIR |= BIT1;
    P3DIR |= BIT7;
    P7DIR |= BIT4;
    P6DIR |= BIT3;
    P6DIR |= BIT4;
    P3DIR |= BIT5;
}
/*跑马灯全部熄灭*/
void resetLED(void)
{
    P8OUT &= ~BIT1;
    P3OUT &= ~BIT7;
    P7OUT &= ~BIT4;
    P6OUT &= ~BIT3;
    P6OUT &= ~BIT4;
    P3OUT &= ~BIT5;
}
/*跑马灯全部点亮*/
void igniteLED(void)
{
    P8OUT |= BIT1;
    P3OUT |= BIT7;
    P7OUT |= BIT4;
    P6OUT |= BIT3;
    P6OUT |= BIT4;
    P3OUT |= BIT5;
}
void igniteLED0(void)
{
    P8OUT |= BIT1;
    P3OUT |= BIT7;
    P7OUT |= BIT4;
}
void igniteLED1(void)
{
    P6OUT |= BIT3;
    P6OUT |= BIT4;
    P3OUT |= BIT5;
}
void igniteLED_single(int x)
{
    switch(x){
        case 1: P8OUT |= BIT1; break;
        case 2: P3OUT |= BIT7; break;
        case 3: P7OUT |= BIT4; break;
        case 4: P6OUT |= BIT3; break;
        case 5: P6OUT |= BIT4; break;
        case 6: P3OUT |= BIT5; break;
        default: break;
    }
}
/*从左到右跑马灯*/
void runLED0(int g)
{
    int p;
    P8OUT |= BIT1;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P8OUT &= ~BIT1;
    P3OUT |= BIT7;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P3OUT &= ~BIT7;
    P7OUT |= BIT4;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P7OUT &= ~BIT4;
    P6OUT |= BIT3;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P6OUT &= ~BIT3;
    P6OUT |= BIT4;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P6OUT &= ~BIT4;
    P3OUT |= BIT5;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P3OUT &= ~BIT5;
}
/*从右到左跑马灯*/
void runLED1(int g)
{
    int p;
    P3OUT |= BIT5;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P3OUT &= ~BIT5;
    P6OUT |= BIT4;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P6OUT &= ~BIT4;
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
    P1DIR &= ~BIT4; //P1.4选择input方向
    P1SEL |= BIT4;  //P1.4选择外设，对应TA0CCR3

    P1DIR &= ~BIT5; //P1.5选择input方向
    P1SEL |= BIT5;  //P1.5选择外设，对应TA0CCR4

    TA0CTL = TASSEL__SMCLK + ID_0 + MC_2 + TACLR + TAIE;        //时钟为SMCLK，Continuous up模式，开始时清零计数器
    TA0CCTL3 = CM_1 + SCS + CAP + CCIE + CCIS_0;               //捕获模式：上升沿，捕获中断使能，捕获输入：CCI0A
    TA0CCTL4 = CM_1 + SCS + CAP + CCIE + CCIS_0;               //捕获模式：上升沿，捕获中断使能，捕获输入：CCI0A
}
void resetBuf(void)
{
    k = kk = 0;
    i = ii = 0;
    int p;
    for (p=0; p<sampleNum; ++p) dat1[p] = dat2[p] = 0;
    times_TACCR3 = times_TACCR4 = 1;
}
/*手势判断*/
void signalJudge(void)
{
    HC_SR04Start();
    if ((kk > 100 && k > 100) && (kk < 1000 && k < 1000)) {
        __disable_interrupt();
//        modeSel[modeCnt] = 1;
        resetLED();
        __delay_cycles(signalDelay);
        igniteLED();
        __delay_cycles(signalDelay);
        resetLED();
        resetBuf();
//        modeCnt++;
        __enable_interrupt();
        return;
    }
    if ((k > 100 && kk <= 100) && (kk < 1500 && k < 1500)) {
        __disable_interrupt();
//        modeSel[modeCnt] = 2;
        resetLED();
        runLED0(k);
        __delay_cycles(signalDelay);
        resetLED();
        resetBuf();
//        modeCnt++;
        __enable_interrupt();
        return;
    }
    if ((kk > 100 && k <= 100) && (kk < 1500 && k < 1500)) {
        __disable_interrupt();
//        modeSel[modeCnt] = 3;
        resetLED();
        runLED1(kk);
        __delay_cycles(signalDelay);
        resetLED();
        resetBuf();
//        modeCnt++;
        __enable_interrupt();
        return;
    }
//    __disable_interrupt();
//    resetBuf();
//    __enable_interrupt();
}
void signalRes(void)
{
    if (modeCnt < 2) return;
    __disable_interrupt();
    int a = modeSel[0];
    int b = modeSel[1];
    modeSel[0] = modeSel[1] = modeCnt = 0;
    if (a == 1 && b == 1) {
        resetLED();
        igniteLED_single(1);
        __delay_cycles(signalDelay);
        resetLED();
        __enable_interrupt();
        return;
    }
    if (a == 2 && b == 2) {
        resetLED();
        igniteLED_single(2);
        __delay_cycles(signalDelay);
        resetLED();
        __enable_interrupt();
        return;
    }
    if (a == 2 && b == 3) {
        resetLED();
        runLED0(1000);
        resetLED();
        __enable_interrupt();
        return;
    }
    if (a == 3 && b == 3) {
        resetLED();
        igniteLED_single(1);
        __delay_cycles(signalDelay);
        resetLED();
        __enable_interrupt();
        return;
    }
    if (a == 3 && b == 2) {
        resetLED();
        runLED1(1000);
        resetLED();
        __enable_interrupt();
        return;
    }
    resetLED();
    runLED1(500);
    runLED0(1000);
    runLED1(500);
    resetLED();
    __enable_interrupt();
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
        signalJudge();
//        signalRes();
        LPM0;               //低功耗模式
    }
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void)
{
    __disable_interrupt();                        //关中断
    static unsigned int L=sampleNum/3;
    static unsigned int R=sampleNum*2/3;
    unsigned int j;
    unsigned int t;

    if (~(TA0CCTL3 & CCIFG)) {
        if(times_TACCR3 == 1)
        {
            LastCCR2 = TA0CCR3;       //记录下上次CCR3的值
            TA0CCTL3 &= ~CM_1;        //清上升沿捕获
            TA0CCTL3 |= CM_2;         //改为下降沿捕获
            times_TACCR3++;
        }
        if(times_TACCR3 == 0)
        {
            if(i < sampleNum)                //把采集到的10次的值都放到数组中
            {
                dat1[i] = TA0CCR3 - LastCCR2;
                i++;
            }
            if(i >= sampleNum)               //为了防止误差，排序把采集到的两头两尾的值去掉，取中间的平均值
            {
                for(i=0;i<sampleNum;i++)
                {
                    for(j=i+1;j<sampleNum;j++)
                    {
                        if(dat1[i]>dat1[j])
                        {
                            t=dat1[j];
                            dat1[j]=dat1[i];
                            dat1[i]=t;
                        }
                    }
                }
                t = 0;
                for(i=L;i<R;i++)  t += dat1[i];
                k = t/(R-L);
                i = 0;
            }
            TA0CCTL3 &= ~CM_2;      //清除下降沿触发
            TA0CCTL3 |= CM_1;       //变为上升沿触发
            times_TACCR3++;                //改变times_TACCR3的值
        }
        times_TACCR3 &= 0x01;                //times_TACCR3>1时清0
        TA0CCTL3 &= ~CCIFG;         //清除中断响应标志
    }
    if (~(TA0CCTL4 & CCIFG)) {
        if(times_TACCR4 == 1)
        {
            LastCCR4 = TA0CCR4;       //记录下上次CCR4的值
            TA0CCTL4 &= ~CM_1;        //清上升沿捕获
            TA0CCTL4 |= CM_2;         //改为下降沿捕获
            times_TACCR4++;
        }
        if(times_TACCR4 == 0)
        {
            if(ii < sampleNum)                //把采集到的20次的值都放到数组中
            {
                dat2[ii] = TA0CCR4 - LastCCR4;
                ii++;
            }
            if(ii >= sampleNum)               //为了防止误差，排序把采集到的两头两尾的值去掉，取中间的平均值
            {
                for(ii=0;ii<sampleNum;ii++)
                {
                    for(j=ii+1;j<sampleNum;j++)
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
                for(ii=L;ii<R;ii++)  t += dat2[ii];
                kk = t/(R-L);
                ii = 0;
            }
            TA0CCTL4 &= ~CM_2;      //清除下降沿触发
            TA0CCTL4 |= CM_1;       //变为上升沿触发
            times_TACCR4++;         //改变times_TACCR4的值
        }
        times_TACCR4 &= 0x01;       //times_TACCR4>1时清0
        TA0CCTL4 &= ~CCIFG;         //清除中断响应标志
    }
    LPM0_EXIT;                  //退出低功耗模式
    __enable_interrupt();       //开中断
}
