int main(void)
{
// stop watchdog timer
     WDTCTL = WDTPW | WDTHOLD;

     TA0CTL= TASSEL__ACLK + TACLR + MC_2;      // ACLK, 清除 TAR
     TA0CCR0 =  10000;     // PWM周期
     TA0CCTL1 = OUTMOD_7; // 输出模式7
     TA0CCR1 = 9000;  //占空比90%
     TA0CCTL2 = OUTMOD_7; // 输出模式7;
     TA0CCR2 =      ;  //占空比10%
                        ;    // P1.2 方向为输出
                        ;    // P1.2端口为外设
                             // 定时器TA0.1
                        ;    // P1.3 方向为输出
                        ;    // P1.3端口为外设
                             // 定时器TA0.2
      TA0CTL |= MC0;  // Timer_A 增计数模式
      for (;;)
      {
                 _NOP();
      }

       return 0;
}
