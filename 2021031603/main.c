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

void InitIO()
{
    //L1 Init
    SET(P8DIR,BIT1);
    CLR(P8OUT,BIT1);

    //S1、S2 Init
    SET(P1REN,(BIT2+BIT3));//使能S1，S2的上下拉电阻
    SET(P1OUT,(BIT2+BIT3));//设置S1，S2为上拉


}

/***********************************************************************************
   TimerA初始化
    说明： xxxxxxx
***********************************************************************************/
void TimerInit(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    TA2CTL |= TASSEL__SMCLK;
    TA2CTL |= TACLR;
    TA2CCR0= 1000;        //初始化
    TA2CCTL1 |= OUTMOD_7    ; // 输出模式7   _BIS_SR(LPM3_bits );
    TA2CCR1 =500   ;  //占空比90%      _NOP();
     //占空比10 %
    P2DIR|=BIT5;    // P1.2 方向为输出
    P2SEL|=BIT5;    // P1.2端口为外设    }
    // 定时器TA0.1


    TA0CTL |= MC0;  // Timer_A 增计数模式


}


int main(void)
{   InitIO();
    ClockInit();
TimerInit();
    P1DIR|=BIT5;
    P2DIR|=BIT4;
    P2DIR|=BIT5;
    P1OUT|=BIT5 ;//  P1.5;
                  P2OUT&=~BIT4;//  P2.4;
                  P2OUT|=BIT5;//2.5;

                   __delay_cycles(MCLK_FREQ*2);
      __no_operation();                         // For debugger

    while(1)
    {int i,j;
    for(i=0;i<20;i++)
    {
        TA2CCR1 = TA2CCR1+20;
    __delay_cycles(MCLK_FREQ*0.5);
    }   //调整占空比
    for(j=0;j<20;j++)
    {TA2CCR1 = TA2CCR1-20;
     __delay_cycles(MCLK_FREQ*0.5);
        }

    }


}



