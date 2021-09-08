#include <msp430.h> 


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    P3DIR |= BIT7;                       // ����P3.7��Ϊ���ģʽ
    P3OUT &= ~BIT7;                       // ����P3.7�ڳ�ʼ״̬
    P1REN |= BIT3;                       // ʹ��P1.2���������蹦��
    P1OUT |= BIT3;                        // ��P1.2Ϊ�������跽ʽ

    while (1)
    {        //�жϰ���״̬�����S1����������P1.2=0��̧����P1.2=1
            if (!(P1IN & BIT3))
                  P3OUT |= BIT7;     // P3.7����ߣ�LED1������
            else
                  P3OUT &= ~BIT7;     // P3.7����ͣ�LED1Ϩ��
    }
    return 0;
}
