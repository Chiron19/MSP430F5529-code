#include <msp430.h> 
#define X 200000

/**
 * main.c
 */
void initLED(void)
{
    P8DIR |= BIT1;                       // 设置P8.1口为输出模式
    P3DIR |= BIT7;                       // 设置P8.1口为输出模式
    P7DIR |= BIT4;                       // 设置P8.1口为输出模式
    P6DIR |= BIT3;                       // 设置P8.1口为输出模式
}
void resetLED(void)
{
    P8OUT &= ~BIT1;                       // 设置P8.1口初始状态
    P3OUT &= ~BIT7;                       // 设置P8.1口初始状态
    P7OUT &= ~BIT4;                       // 设置P8.1口初始状态
    P6OUT &= ~BIT3;                       // 设置P8.1口初始状态
}
void runLED(void)
{
    do {
    P8OUT |= BIT1;                       // 设置P8.1口为输出模式
    if ((P1IN & BIT2)) return;
    _delay_cycles(X);
    P8OUT &= ~BIT1;                       // 设置P8.1口初始状态

    P3OUT |= BIT7;                       // 设置P8.1口为输出模式
    if ((P1IN & BIT2)) return;
    _delay_cycles(X);
    P3OUT &= ~BIT7;                       // 设置P8.1口初始状态

    P7OUT |= BIT4;                       // 设置P8.1口为输出模式
    if ((P1IN & BIT2)) return;
    _delay_cycles(X);
    P7OUT &= ~BIT4;                       // 设置P8.1口初始状态

    P6OUT |= BIT3;                       // 设置P8.1口为输出模式
    if ((P1IN & BIT2)) return;
    _delay_cycles(X);
    P6OUT &= ~BIT3;                       // 设置P8.1口初始状态
    } while (1);
}
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	initLED();
	resetLED();
    P1REN |= BIT2;                       // 使能P1.2上下拉电阻功能
    P1OUT |= BIT2;                        // 置P1.2为上拉电阻方式

    while (1)
    {        //判断按键状态，如果S2按键按下则P1.2=0，抬起则P1.2=1
            if (!(P1IN & BIT2)) {
                runLED();
                resetLED();
            }
    }
	return 0;
}
