#include <msp430.h> 


/**
 * main.c
 */

    void delay_ms(unsigned long ms)
    {
            int i;
            for (i = 0; i < ms; i++)
            {
                    __delay_cycles(1000);
            }
    }
int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;
        //初始化开关S1
         P1DIR &= ~BIT2;//设置P1.2为输入方向
         P1REN |= BIT2;//使能P1.2上拉/下拉电阻
         P1OUT |= BIT2;//设置P1.2上拉电阻
         P1IE |= BIT2;//使能P1.2中断
         P1IES |= BIT2;//P1.2下降沿中断
         //初始化开关S2
         P1DIR &= ~BIT3;//设置P1.2为输入方向
         P1REN |= BIT3;//使能P1.2上拉/下拉电阻
         P1OUT |= BIT3;//设置P1.2上拉电阻
         P1IE |= BIT3;//使能P1.2中断
         P1IES |= BIT3;//P1.2下降沿中断
       //灯
         P8OUT &= ~BIT1; //初始化P8.1为低电平
         P8DIR |= BIT1;//设置P8.1为输出方向
         //蜂鸣器
           P3OUT&=~BIT6;
       P3DIR|=BIT6;
       TA0CTL = TASSEL1 + MC0;
      TA0CCR0 = 50000;//计数值3000
      _EINT();//开启全局中断


    return 0;
}
    #pragma vector = TIMER0_A0_VECTOR
    __interrupt void Timer_A0 (void)     //定时器A0中断服务程序
    {

        P8OUT ^= BIT1;
        P3OUT ^= BIT6;
        TA0CCTL0 &= ~CCIFG;
    }
    #pragma vector=PORT1_VECTOR//设置中断向量
    __interrupt void PORT1_key()
    {

    if (P1IFG&BIT2)
        TA0CCTL0 |= CCIE;//允许中断
    else if (P1IFG&BIT3)
    {
        TA0CCTL0 &= ~CCIE;//禁止中断
        P8OUT &= ~BIT1;
    }
        delay_ms(200);
        P1IFG = 0x00;//清除中断标志位
    }
