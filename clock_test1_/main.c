#include <msp430.h>

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // �رտ��Ź�

    P1DIR |= BIT0; // ACLK  ͨ�� P1.0��� ���
//    P1SEL |= BIT0;
    //  ѡ��IO �˿ڹ���Ϊ XT1
    P5SEL |= BIT4+BIT5;
    P2DIR |= BIT2; // ACLK  ͨ�� P1.0��� ���
    P2SEL |= BIT2; //  ѡ��IO �˿ڹ���Ϊ XT1
    P5SEL |= BIT2+BIT3;

    UCSCTL6 &= ~(XT1OFF); //  ʹ�� XT1
    UCSCTL6 &= ~(XT2OFF); //  ʹ�� XT1
    UCSCTL6 |= XCAP_3; //  �����ڽӵ���ֵ��ѡ����͵���ֵXCAP_3

    do
    { //  ����XT1 ��DCO���ϱ�־λ ���ϱ�־λ
        UCSCTL7 &= ~(XT2OFFG);
        UCSCTL7 &= ~(XT1LFOFFG + DCOFFG);
        SFRIFG1 &= ~OFIFG; //  ����SFR�еĹ��ϱ�־λ �еĹ��ϱ�־λ
    } while (SFRIFG1 & OFIFG); // ����������ϱ�־λ

    UCSCTL4 |=  SELA_0 + SELS__XT2CLK + SELM__XT2CLK; //  ѡ�� ACLK��ʱ��Դ ��ʱ��Դ
    P1DIR |= BIT0;
    P1OUT &= ~BIT0;

    for(;;)
    {
        P1OUT ^= BIT0;
        _delay_cycles(1000000); //  ѭ���ȴ�,�ղ��� �ղ���
    }
}
