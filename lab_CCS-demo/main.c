#include <msp430.h> 


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	P1DIR |= BIT0;
	P1OUT &= ~BIT0;

	while(1)
	{
	    P1OUT ^= BIT0;
	    _delay_cycles(1000000);
	}

	
	return 0;
}
