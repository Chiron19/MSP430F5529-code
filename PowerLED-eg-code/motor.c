/*
 * motor.c
 *
 *  Created on: 2014-7-15
 *      Author: dean
 */

#include <msp430.h>
#include "motor.h"
void ioinit()
{
    P1DIR |= BIT5;
    P1OUT |= BIT5;					//设置P1.5为输出模式

    P8DIR |= BIT1;  //p8.1 output
    P8OUT &= ~BIT1;					//设置P8.1为输出模式

	P1DIR &= ~BIT2;  //p1.2 input,pull up
	P1REN |= BIT2				//使能S1，S2的上下拉电阻

	P1DIR &= ~BIT3;  //p1.2 input,pull up
	P1REN |= BIT3;

	P1OUT |= BIT2 + BIT3; //选择S1，S2为上拉电阻

	P2DIR |= BIT4 + BIT5;
	P2OUT |= BIT4 + BIT5;        //设置P2.4和P2.5为输出模式
}
void DCmotor(int p)
{
  switch(p
  {
  case 0:					//停转  0 0 0
    {
        P1OUT &= ~BIT5;//  P1.5;
        P2OUT &= ~BIT4;//  P2.4;
        P2OUT &= ~BIT5;//  P2.5;
      break;
    }
  case 1:					//正转  1 0  1
    {
        P1OUT |= BIT5;//  P1.5;
        P2OUT &= ~BIT4;//  P2.4;
        P2OUT |= BIT5;//  P2.5;

      break;
    }
  case 2:					//反转 1 1 0
    {
        P1OUT |= BIT5;//  P1.5;
        P2OUT |= BIT4;//  P2.4;
        P2OUT &= ~BIT5;//  P2.5;

      break;
    }
  default:				//停转  0 0 0
    {
        P1OUT &= ~BIT5;//  P1.5;
        P2OUT &= ~BIT4;//  P2.4;
        P2OUT &= ~BIT5;//  P2.5;
      break;
    }
  }
}

