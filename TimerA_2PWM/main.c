int main(void)
{
// stop watchdog timer
     WDTCTL = WDTPW | WDTHOLD;

     TA0CTL= TASSEL__ACLK + TACLR + MC_2;      // ACLK, ��� TAR
     TA0CCR0 =  10000;     // PWM����
     TA0CCTL1 = OUTMOD_7; // ���ģʽ7
     TA0CCR1 = 9000;  //ռ�ձ�90%
     TA0CCTL2 = OUTMOD_7; // ���ģʽ7;
     TA0CCR2 =      ;  //ռ�ձ�10%
                        ;    // P1.2 ����Ϊ���
                        ;    // P1.2�˿�Ϊ����
                             // ��ʱ��TA0.1
                        ;    // P1.3 ����Ϊ���
                        ;    // P1.3�˿�Ϊ����
                             // ��ʱ��TA0.2
      TA0CTL |= MC0;  // Timer_A ������ģʽ
      for (;;)
      {
                 _NOP();
      }

       return 0;
}
