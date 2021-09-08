#include <msp430.h>


/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    P8DIR |= BIT1;  //p8.1 output
    P8OUT &= ~BIT1;

    P1DIR &= ~BIT2;  //p1.2 input,pull up
    P1REN |= BIT2;
    P1OUT |= BIT2;
    P1IE  |= BIT2; //enable P1.2 interrupt
    P1IES |= BIT2; //high-low transition

    P3DIR |= BIT7;  //p3.7 output
    P3OUT &= ~BIT7;

    P1DIR &= ~BIT3;  //p1.3 input,pull up
    P1REN |= BIT3;
    P1OUT |= BIT3;
    P1IE  |= BIT3; //enable P1.3 interrupt
    P1IES |= BIT3; //high-low transition

    P7DIR |= BIT4;  //p7.4 output
    P7OUT &= ~BIT4;

    P2DIR &= ~BIT3;  //p2.3 input,pull up
    P2REN |= BIT3;
    P2OUT |= BIT3;
    P2IE  |= BIT3; //enable P2.3 interrupt
    P2IES |= BIT3; //high-low transition

    _enable_interrupt();

    while(1)
    {
        _nop();
    }

    return 0;
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    if (P1IFG & BIT2) {
        P8OUT ^= BIT1;
        P1IFG &= ~BIT2;
    }
    if (P1IFG & BIT3) {
        P3OUT ^= BIT7;
        P1IFG &= ~BIT3;
    }
}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    P7OUT ^= BIT4;
    P2IFG &= ~BIT3;
}
