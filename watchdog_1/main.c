#include <msp430.h> 
int cnt=0; // 定义变量 i，volatile to prevent optimization
/**
 * main.c
 */
int main(void)
{
//    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
      WDTCTL = WDT_ADLY_250;  // 定时周期为0.5ms

      P1DIR &= ~BIT2;  //p1.2 input,pull up
      P1REN |= BIT2;
      P1OUT |= BIT2;

      SFRIE1 |= WDTIE;                // 使能WDT中断
      P1DIR |= BIT0;                  // P1.0输出

      __enable_interrupt();           // 系统中断允许
      for (;;)
      {
               // 进入 LPM0
//        __bis_SR_register(LPM0_bits);
        __no_operation();
        if (!(P1IN & BIT2)) {
            WDTCTL =  WDT_ADLY_250;
            cnt = 0;
        }
      }
    return 0;
}

// 看门狗中断服务子程序
#pragma vector= WDT_VECTOR
__interrupt void watchdog_timer (void)
{
       if (cnt >= 1) {
           cnt = 0;
           P1OUT ^= 0x01;          // P1.0取反
       }
       else cnt ++;
}
