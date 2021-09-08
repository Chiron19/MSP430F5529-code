#include <msp430.h>

int main(void)
{
// stop watchdog timer
     WDTCTL = WDTPW | WDTHOLD;

     TA0CTL= TASSEL__ACLK +  TACLR + MC_1;      // ACLK, 清除 TAR, Timer_A 增计数模式

     TA0CCR0 = 32768;     // PWM周期
     TA0CCTL1 = OUTMOD_7; // 输出模式7
     TA0CCR1 = 29491;  //占空比90%
     TA0CCTL2 = OUTMOD_7; // 输出模式7;
     TA0CCR2 = 3277;  //占空比10%

     P1DIR |= BIT2;    // P1.2 方向为输出
     P1SEL |= BIT2;    // P1.2端口为外设
//     P1OUT &= ~ BIT2;
                             // 定时器TA0.1
     P1DIR |= BIT3;    // P1.3 方向为输出
     P1SEL |= BIT3;    // P1.3端口为外设
//     P1OUT &= ~ BIT3;
                             // 定时器TA0.2

      for (;;)
      {
            _NOP();
      }

       return 0;
}
