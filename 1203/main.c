#include <msp430.h> 

/**
 * main.c
 */
int main(void)
{

    WDTCTL = WDT_ADLY_1000;  // ��ʱ����Ϊ32ms
      SFRIE1 |= WDTIE;                // ʹ��WDT�ж�
      P1DIR |= BIT0;                     // P1.0���
      P1REN|=BIT2;
      P1OUT|=BIT2;
      __enable_interrupt();           // ϵͳ�ж�����
      for (;;)
      {
               // ���� LPM0
        __bis_SR_register(LPM0_bits);
        __no_operation();
      }
    return 0;
}

// ���Ź��жϷ����ӳ���
#pragma vector= WDT_VECTOR
__interrupt void watchdog_timer (void)
{
           P1OUT ^= 0x01;
        // P1.0ȡ��
}
while(1)
   {
       if(!(P1IN&BIT2))
       {
           WDTCTL = WDTPW | WDTHOLD
       }
       else{
           }
       }
