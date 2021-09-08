/*
 * key.c
 *
 *  Created on: 2018-4-12
 *
 */
#include "key.h"
#include <msp430.h>

StrKeyFlag KeyFlag;

void delay(uint16 t)
{
	uint16 i,j,k;

	for(j=t;j>0;j--)
		for(i=250;i>0;i--)
			k=0;
}

/************************IO�ڳ�ʼ��********************************/

void IO_Init(void)
{
	P8DIR |= BIT1;              			// ����P8.1��Ϊ���ģʽ  ����LED��
	P8OUT &= ~BIT1;

	P3DIR |= BIT6;              			// ����P3.6��Ϊ���ģʽ  ���Ʒ�����
	P3OUT &= ~BIT6;

	P2DIR &= ~BIT6;
	P2DIR &= ~BIT3;                         // ����P2.3,P2.6��Ϊ���뷽��
	P2REN |= BIT3 + BIT6;                   // P2.3,P2.6����������ʹ��
	P2OUT |= BIT3 + BIT6;                   // ѡ��P2.3,P2.6Ϊ�������蹦�ܷ�ʽ

	P1DIR &= ~BIT2;
	P1DIR &= ~BIT3;                        	// ����P1.2,P1.3��Ϊ���뷽��
	P1REN |= BIT2 + BIT3;                   // P1.2,P1.3����������ʹ��
	P1OUT |= BIT2 + BIT3;					// ѡ��P1.2,P1.3Ϊ�������蹦�ܷ�ʽ
}

void ScanKey(void)
{

	{
		if(!(P1IN & BIT2))				//ͨ��IO��ֵ���ó�����������Ϣ P1.2�ڰ���
		{
			delay(50);						//��ʱȥ�������ж�һ�� P1.2��ֵ
			if(!(P1IN & BIT2))			//���ж�һ�� P1.2��ֵ
			{
				KeyFlag.S1=1;
			}
		}
		if(!(P1IN & BIT3))               // P1.3�ڰ���
		{
			delay(50);
			if(!(P1IN & BIT3))
			{
				KeyFlag.S2=1;
			}
		}
		if(!(P2IN & BIT3))               //P2.3�ڰ���
		{
			delay(50);
			if(!(P2IN & BIT3))
			{
				KeyFlag.S3=1;
			}
		}
		if(!(P2IN & BIT6))              //P2.6�ڰ���
		{
			delay(50);
			if(!(P2IN & BIT6))
			{
				KeyFlag.S4=1;
			}
		}
	}
}
