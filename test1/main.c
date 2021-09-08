#include <msp430.h> 
#define X 200000

/**
 * main.c
 */
void initLED(void)
{
    P8DIR |= BIT1;                       // ����P8.1��Ϊ���ģʽ
    P3DIR |= BIT7;                       // ����P8.1��Ϊ���ģʽ
    P7DIR |= BIT4;                       // ����P8.1��Ϊ���ģʽ
    P6DIR |= BIT3;                       // ����P8.1��Ϊ���ģʽ
}
void resetLED(void)
{
    P8OUT &= ~BIT1;                       // ����P8.1�ڳ�ʼ״̬
    P3OUT &= ~BIT7;                       // ����P8.1�ڳ�ʼ״̬
    P7OUT &= ~BIT4;                       // ����P8.1�ڳ�ʼ״̬
    P6OUT &= ~BIT3;                       // ����P8.1�ڳ�ʼ״̬
}
void runLED(void)
{
    do {
    P8OUT |= BIT1;                       // ����P8.1��Ϊ���ģʽ
    if ((P1IN & BIT2)) return;
    _delay_cycles(X);
    P8OUT &= ~BIT1;                       // ����P8.1�ڳ�ʼ״̬

    P3OUT |= BIT7;                       // ����P8.1��Ϊ���ģʽ
    if ((P1IN & BIT2)) return;
    _delay_cycles(X);
    P3OUT &= ~BIT7;                       // ����P8.1�ڳ�ʼ״̬

    P7OUT |= BIT4;                       // ����P8.1��Ϊ���ģʽ
    if ((P1IN & BIT2)) return;
    _delay_cycles(X);
    P7OUT &= ~BIT4;                       // ����P8.1�ڳ�ʼ״̬

    P6OUT |= BIT3;                       // ����P8.1��Ϊ���ģʽ
    if ((P1IN & BIT2)) return;
    _delay_cycles(X);
    P6OUT &= ~BIT3;                       // ����P8.1�ڳ�ʼ״̬
    } while (1);
}
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	initLED();
	resetLED();
    P1REN |= BIT2;                       // ʹ��P1.2���������蹦��
    P1OUT |= BIT2;                        // ��P1.2Ϊ�������跽ʽ

    while (1)
    {        //�жϰ���״̬�����S2����������P1.2=0��̧����P1.2=1
            if (!(P1IN & BIT2)) {
                runLED();
                resetLED();
            }
    }
	return 0;
}
