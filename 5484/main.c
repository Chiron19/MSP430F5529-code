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
        //��ʼ������S1
         P1DIR &= ~BIT2;//����P1.2Ϊ���뷽��
         P1REN |= BIT2;//ʹ��P1.2����/��������
         P1OUT |= BIT2;//����P1.2��������
         P1IE |= BIT2;//ʹ��P1.2�ж�
         P1IES |= BIT2;//P1.2�½����ж�
         //��ʼ������S2
         P1DIR &= ~BIT3;//����P1.2Ϊ���뷽��
         P1REN |= BIT3;//ʹ��P1.2����/��������
         P1OUT |= BIT3;//����P1.2��������
         P1IE |= BIT3;//ʹ��P1.2�ж�
         P1IES |= BIT3;//P1.2�½����ж�
       //��
         P8OUT &= ~BIT1; //��ʼ��P8.1Ϊ�͵�ƽ
         P8DIR |= BIT1;//����P8.1Ϊ�������
         //������
           P3OUT&=~BIT6;
       P3DIR|=BIT6;
       TA0CTL = TASSEL1 + MC0;
      TA0CCR0 = 50000;//����ֵ3000
      _EINT();//����ȫ���ж�


    return 0;
}
    #pragma vector = TIMER0_A0_VECTOR
    __interrupt void Timer_A0 (void)     //��ʱ��A0�жϷ������
    {

        P8OUT ^= BIT1;
        P3OUT ^= BIT6;
        TA0CCTL0 &= ~CCIFG;
    }
    #pragma vector=PORT1_VECTOR//�����ж�����
    __interrupt void PORT1_key()
    {

    if (P1IFG&BIT2)
        TA0CCTL0 |= CCIE;//�����ж�
    else if (P1IFG&BIT3)
    {
        TA0CCTL0 &= ~CCIE;//��ֹ�ж�
        P8OUT &= ~BIT1;
    }
        delay_ms(200);
        P1IFG = 0x00;//����жϱ�־λ
    }
