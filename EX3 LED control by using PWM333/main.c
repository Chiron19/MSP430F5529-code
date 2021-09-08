#include <msp430.h> 
#include "MacroAndConst.h"

#define XT2_FREQ   4000000
#define MCLK_FREQ 16000000
#define SMCLK_FREQ 4000000
void ClockInit()
{
     UCSCTL6 &= ~XT1OFF; //启动XT1
     P5SEL |= BIT2 + BIT3; //XT2引脚功能选择
     UCSCTL6 &= ~XT2OFF;          //打开XT2

     __bis_SR_register(SCG0);

     UCSCTL0 = DCO0+DCO1+DCO2+DCO3+DCO4;
     UCSCTL1 = DCORSEL_4;       //DCO频率范围在28.2MHZ以下
     UCSCTL2 = FLLD_5 + 1;       //D=16，N=1
     UCSCTL3 = SELREF_5 + FLLREFDIV_3;    //n=8,FLLREFCLK时钟源为XT2CLK；DCOCLK=D*(N+1)*(FLLREFCLK/n);DCOCLKDIV=(N+1)*(FLLREFCLK/n);
     UCSCTL4 = SELA_4 + SELS_3 +SELM_3;    //ACLK的时钟源为DCOCLKDIV,MCLK\SMCLK的时钟源为DCOCLK
     UCSCTL5 = DIVA_5 +DIVS_1;      //ACLK由DCOCLKDIV的32分频得到，SMCLK由DCOCLK的2分频得到
                 //最终MCLK:16MHZ,SMCLK:8MHZ,ACLK:32KHZ
//   __bic_SR_register(SCG0);                   //Enable the FLL control loop
}

/***********************************************************************************
   IO端口初始化
    说明： 口袋板LED灯端口输出，依次是L1-P8.1;  L2-->P3.7; L3--> P7.4; L4-->P6.3;L5-->P6.4;L6-->P3.5
                 机械按钮输入，依次是S1-P1.2;  S2-->P1.3;
***********************************************************************************/
int Filter()
{
    int i, j, t;
    int value_buf[N];
    int sum = 0;
    for (i = 0; i < N; i++)
    {
        value_buf[i] = ADC12MEM0;
        _delay_cycles(5000);
    }
    for (j=0; j<N-1; j++)
    {
        for (i=0; i<N-j-1; i++)
        {
            if (value_buf[i] > value_buf[i+1])
            {
                int temp = value_buf[i];
                value_buf[i] = value_buf[i+1];
                value_buf[i+1] = temp;
            }
        }
    }
    for(i=1; i<N-1; i++) sum += value_buf[i];
    t = sum/(N-2);
    return t;
}
void light_off()
{
    P8OUT &= ~BIT1;
    P3OUT &= ~BIT7;
    P7OUT &= ~BIT4;
    P6OUT &= ~BIT3;
    P6OUT &= ~BIT4;
    P3OUT &= ~BIT5;
}
void InitIO()
{
    //L1 Init
    SET(P8DIR,BIT1);
    CLR(P8OUT,BIT1);

    //S1、S2 Init
    SET(P1REN,(BIT2+BIT3));//使能S1，S2的上下拉电阻
    SET(P1OUT,(BIT2+BIT3));//设置S1，S2为上拉

    P1DIR |= BIT5;
    P1OUT |= BIT5;

    P2SEL |= BIT5;
    P2DIR |= BIT5;
//    P2OUT |= BIT5;        //设置P2.4和P2.5为输出模式
//  P2OUT |= BIT4;        //设置P2.4和P2.5为输出模式

}
void ioinit()
{//LED1~LED6的初始化
    P8DIR |= BIT1;  //p8.1 output

    P3DIR |= BIT7;  //p3.7 output

    P7DIR |= BIT4;  //p7.4 output

    P6DIR |= BIT3;  //p7.4 output

    P6DIR |= BIT4;  //p7.4 output

    P3DIR |= BIT5;  //p7.4 output

    light_off();
 //拨盘电位器对应IO初始化
    P6DIR |= BIT5;
    P6SEL |= BIT5;

    P6DIR |= BIT2;
    P6SEL |= BIT2;

    P1DIR &= ~BIT2;
    P1REN |= BIT2;                       // 使能P1.2上下拉电阻功能
    P1OUT |= BIT2;                        // 置P1.2为上拉电阻方式
    P1IE  |= BIT2; //enable P1.2 interrupt
    P1IES |= BIT2; //high-low transition
}
void AD_Init()
{// 在ADC12ENC==0时（默认）,初始化各寄存器，后打开ADC转换使能（ADC12ENC==1）
    //多路采样转换（首次需要SHI信号上升沿触发采样定时器）自动循环采样转换
    ADC12CTL0 |= ADC12MSC;

    //启动（打开）ADC12模块
    ADC12CTL0 |= ADC12ON;

    ADC12CTL1 |= ADC12CSTARTADD_0;

    //选择单通道多次采样转换模式
    ADC12CTL1 |= ADC12CONSEQ_2;

    //采样保持模式选择，选择采样信号（SAMPCON）的来源是采样定时器
    ADC12CTL1 |= ADC12SHP;

    //存储寄存器0选择通道P6.2麦克风
    ADC12MCTL0 |= ADC12INCH_2;

    //ADC转换使能
    ADC12CTL0 |= ADC12ENC;
}

/***********************************************************************************
   TimerA初始化
    说明： xxxxxxx
***********************************************************************************/
void TimerInit(void)
{
    TA2CTL = TASSEL__SMCLK + TACLR + MC_1;
    TA2CCR0 = 0xFFFF;
    TA2CCTL2 = OUTMOD_7;
    
    TA0CTL |= TASSEL__ACLK + TACLR + MC_1 + TAIE;   //时钟为SMCLK,增计数模式，开始时清零计数器
    TA0CCTL0 =  CCIE;                               //比较器中断使能
    TA0CCR0  = 0xFFFF;                                //比较值设为100000，相当于100ms的时间间隔

    __enable_interrupt();
}

void LEDstart(void)
{
    volatile unsigned int i = 0;
    for (i=0; i<0xFFFF; i+=0x00FF) {
        TA2CCR2 = i;
        __delay_cycles(MCLK_FREQ*0.005);
    }
}
void LEDend(void)
{
    volatile unsigned int i = 0;
    for (i=0xFFFF; i>=0x00FF; i-=0x00FF) {
        TA2CCR2 = i;
        __delay_cycles(MCLK_FREQ*0.005);
    }
}
static volatile unsigned int ADon = 0, cnt_LEDcycle = 0, LEDon = 0;
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    ClockInit();
    ioinit();
    AD_Init();
    InitIO();//初始化
    TimerInit();

    ADC12CTL0 |= ADC12SC;                         //开始采样转换



    volatile unsigned int value = 0, w = 0;
                                    //设置判断变量
    for(;;)
    {
        value = Filter();

        if (value >= 2200) {
            if (!LEDon) LEDstart();
            LEDon = 1;
            cnt_LEDcycle = 0;
        }

    }
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    if (P1IFG & BIT2) {
        _delay_cycles(50);
        if (P1IN & BIT2) {
            ADon ^= 1;
            light_off();
        }
    }
    P1IFG &= ~BIT2;
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
      if (LEDon) {
          cnt_LEDcycle++;
          if(cnt_LEDcycle >= 5) {
//              printf("%d\n", cnt_LEDcycle);
              LEDend();
              cnt_LEDcycle = LEDon = 0;
          }
      }
      TA0CTL &= ~TAIFG;
}
