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
    P1OUT |= BIT5;					//����P1.5Ϊ���ģʽ

    P8DIR |= BIT1;  //p8.1 output
    P8OUT &= ~BIT1;					//����P8.1Ϊ���ģʽ

	P1DIR &= ~BIT2;  //p1.2 input,pull up
	P1REN |= BIT2				//ʹ��S1��S2������������

	P1DIR &= ~BIT3;  //p1.2 input,pull up
	P1REN |= BIT3;

	P1OUT |= BIT2 + BIT3; //ѡ��S1��S2Ϊ��������

	P2DIR |= BIT4 + BIT5;
	P2OUT |= BIT4 + BIT5;        //����P2.4��P2.5Ϊ���ģʽ
}
void DCmotor(int p)
{
  switch(p
  {
  case 0:					//ͣת  0 0 0
    {
        P1OUT &= ~BIT5;//  P1.5;
        P2OUT &= ~BIT4;//  P2.4;
        P2OUT &= ~BIT5;//  P2.5;
      break;
    }
  case 1:					//��ת  1 0  1
    {
        P1OUT |= BIT5;//  P1.5;
        P2OUT &= ~BIT4;//  P2.4;
        P2OUT |= BIT5;//  P2.5;

      break;
    }
  case 2:					//��ת 1 1 0
    {
        P1OUT |= BIT5;//  P1.5;
        P2OUT |= BIT4;//  P2.4;
        P2OUT &= ~BIT5;//  P2.5;

      break;
    }
  default:				//ͣת  0 0 0
    {
        P1OUT &= ~BIT5;//  P1.5;
        P2OUT &= ~BIT4;//  P2.4;
        P2OUT &= ~BIT5;//  P2.5;
      break;
    }
  }
}

