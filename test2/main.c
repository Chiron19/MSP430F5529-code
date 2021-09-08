#include <msp430.h> 


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    P3DIR |= BIT7;                       // 设置P3.7口为输出模式
    P3OUT &= ~BIT7;                       // 设置P3.7口初始状态
    P1REN |= BIT3;                       // 使能P1.2上下拉电阻功能
    P1OUT |= BIT3;                        // 置P1.2为上拉电阻方式

    while (1)
    {        //判断按键状态，如果S1按键按下则P1.2=0，抬起则P1.2=1
            if (!(P1IN & BIT3))
                  P3OUT |= BIT7;     // P3.7输出高（LED1点亮）
            else
                  P3OUT &= ~BIT7;     // P3.7输出低（LED1熄灭）
    }
    return 0;
}
