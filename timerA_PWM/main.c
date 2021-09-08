#include <msp430.h>

int main(void)
{
// stop watchdog timer
     WDTCTL = WDTPW | WDTHOLD;

     TA0CTL= TASSEL__ACLK +  TACLR + MC_1;      // ACLK, ��� TAR, Timer_A ������ģʽ

     TA0CCR0 = 32768;     // PWM����
     TA0CCTL1 = OUTMOD_7; // ���ģʽ7
     TA0CCR1 = 29491;  //ռ�ձ�90%
     TA0CCTL2 = OUTMOD_7; // ���ģʽ7;
     TA0CCR2 = 3277;  //ռ�ձ�10%

     P1DIR |= BIT2;    // P1.2 ����Ϊ���
     P1SEL |= BIT2;    // P1.2�˿�Ϊ����
//     P1OUT &= ~ BIT2;
                             // ��ʱ��TA0.1
     P1DIR |= BIT3;    // P1.3 ����Ϊ���
     P1SEL |= BIT3;    // P1.3�˿�Ϊ����
//     P1OUT &= ~ BIT3;
                             // ��ʱ��TA0.2

      for (;;)
      {
            _NOP();
      }

       return 0;
}
