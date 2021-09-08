#include <msp430.h> 
#include "MacroAndConst.h"


#define XT2_FREQ   4000000
#define MCLK_FREQ 16000000
#define SMCLK_FREQ 4000000
void ClockInit()
{
     UCSCTL6 &= ~XT1OFF; //����XT1
     P5SEL |= BIT2 + BIT3; //XT2���Ź���ѡ��
     UCSCTL6 &= ~XT2OFF;          //��XT2
     __bis_SR_register(SCG0);
     UCSCTL0 = DCO0+DCO1+DCO2+DCO3+DCO4;
     UCSCTL1 = DCORSEL_4;       //DCOƵ�ʷ�Χ��28.2MHZ����
     UCSCTL2 = FLLD_5 + 1;       //D=16��N=1
     UCSCTL3 = SELREF_5 + FLLREFDIV_3;    //n=8,FLLREFCLKʱ��ԴΪXT2CLK��DCOCLK=D*(N+1)*(FLLREFCLK/n);DCOCLKDIV=(N+1)*(FLLREFCLK/n);
     UCSCTL4 = SELA_4 + SELS_3 +SELM_3;    //ACLK��ʱ��ԴΪDCOCLKDIV,MCLK\SMCLK��ʱ��ԴΪDCOCLK
     UCSCTL5 = DIVA_5 +DIVS_1;      //ACLK��DCOCLKDIV��32��Ƶ�õ���SMCLK��DCOCLK��2��Ƶ�õ�
                 //����MCLK:16MHZ,SMCLK:8MHZ,ACLK:32KHZ
//   __bic_SR_register(SCG0);                   //Enable the FLL control loop
}

/***********************************************************************************
   IO�˿ڳ�ʼ��
    ˵���� �ڴ���LED�ƶ˿������������L1-P8.1;  L2-->P3.7; L3--> P7.4; L4-->P6.3;L5-->P6.4;L6-->P3.5
                 ��е��ť���룬������S1-P1.2;  S2-->P1.3;
***********************************************************************************/

void InitIO()
{
    //L1 Init
    SET(P8DIR,BIT1);
    CLR(P8OUT,BIT1);

    //S1��S2 Init
    SET(P1REN,(BIT2+BIT3));//ʹ��S1��S2������������
    SET(P1OUT,(BIT2+BIT3));//����S1��S2Ϊ����


}

/***********************************************************************************
   TimerA��ʼ��
    ˵���� xxxxxxx
***********************************************************************************/
void TimerInit(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    TA2CTL |= TASSEL__SMCLK;
    TA2CTL |= TACLR;
    TA2CCR0= 1000;        //��ʼ��
    TA2CCTL1 |= OUTMOD_7    ; // ���ģʽ7   _BIS_SR(LPM3_bits );
    TA2CCR1 =500   ;  //ռ�ձ�90%      _NOP();
     //ռ�ձ�10 %
    P2DIR|=BIT5;    // P1.2 ����Ϊ���
    P2SEL|=BIT5;    // P1.2�˿�Ϊ����    }
    // ��ʱ��TA0.1


    TA0CTL |= MC0;  // Timer_A ������ģʽ


}


int main(void)
{   InitIO();
    ClockInit();
TimerInit();
    P1DIR|=BIT5;
    P2DIR|=BIT4;
    P2DIR|=BIT5;
    P1OUT|=BIT5 ;//  P1.5;
                  P2OUT&=~BIT4;//  P2.4;
                  P2OUT|=BIT5;//2.5;

                   __delay_cycles(MCLK_FREQ*2);
      __no_operation();                         // For debugger

    while(1)
    {int i,j;
    for(i=0;i<20;i++)
    {
        TA2CCR1 = TA2CCR1+20;
    __delay_cycles(MCLK_FREQ*0.5);
    }   //����ռ�ձ�
    for(j=0;j<20;j++)
    {TA2CCR1 = TA2CCR1-20;
     __delay_cycles(MCLK_FREQ*0.5);
        }

    }


}


