#include <msp430.h> 
#include "key.h"

unsigned int i=0;

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    IO_Init();

  TA0CTL |= TASSEL__SMCLK + TACLR + MC_1 + TAIE;   //ʱ��ΪSMCLK,������ģʽ����ʼʱ���������
  TA0CCTL0 =  CCIE;                               //�Ƚ����ж�ʹ��
  TA0CCR0  = 500;                                //�Ƚ�ֵ��Ϊ100000���൱��100ms��ʱ����

  while(1)
  {
        ScanKey();                          //ɨ�谴������
        if (KeyFlag.S1==1)
        {
             KeyFlag.S1=0;
             __enable_interrupt(); // ��ȫ���ж�ʹ��
        }
        if (KeyFlag.S2==1)
         {
           KeyFlag.S2=0;
           P8OUT &= ~BIT1;
           __disable_interrupt(); // �ر�ȫ���ж�ʹ��
         }
  }
}
/************************��ʱ���жϺ���********************************/
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A (void)

{
      i++;
      if(i >= 100)
      {
         P8OUT ^= BIT1;          //�γ�����Ч��
         P3OUT ^= BIT6;          //�γ�����Ч��
         i = 0;
      }
      TA0CTL &= ~ TAIFG;
}
