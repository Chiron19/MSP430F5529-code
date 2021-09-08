#include <msp430.h> 
int cnt=0; // ������� i��volatile to prevent optimization
/**
 * main.c
 */
int main(void)
{
//    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
      WDTCTL = WDT_ADLY_250;  // ��ʱ����Ϊ0.5ms

      P1DIR &= ~BIT2;  //p1.2 input,pull up
      P1REN |= BIT2;
      P1OUT |= BIT2;

      SFRIE1 |= WDTIE;                // ʹ��WDT�ж�
      P1DIR |= BIT0;                  // P1.0���

      __enable_interrupt();           // ϵͳ�ж�����
      for (;;)
      {
               // ���� LPM0
//        __bis_SR_register(LPM0_bits);
        __no_operation();
        if (!(P1IN & BIT2)) {
            WDTCTL =  WDT_ADLY_250;
            cnt = 0;
        }
      }
    return 0;
}

// ���Ź��жϷ����ӳ���
#pragma vector= WDT_VECTOR
__interrupt void watchdog_timer (void)
{
       if (cnt >= 1) {
           cnt = 0;
           P1OUT ^= 0x01;          // P1.0ȡ��
       }
       else cnt ++;
}
