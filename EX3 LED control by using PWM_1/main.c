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
//	 __bic_SR_register(SCG0);                   //Enable the FLL control loop
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
	
	P1DIR |= BIT5;
	P1OUT |= BIT5;

	P2SEL |= BIT5;
	P2DIR |= BIT5;

//	P2OUT |= BIT4;        //����P2.4��P2.5Ϊ���ģʽ

}

/***********************************************************************************
   TimerA��ʼ��
    ˵���� xxxxxxx
***********************************************************************************/
void TimerInit(void)
{
    TA2CTL = TASSEL__SMCLK + TACLR + MC_1;
    TA2CCR0 = 0xFFFF;

    TA2CCTL2 = OUTMOD_7;
    
    __enable_interrupt();
}

volatile int i;
int main(void)
{
     WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    InitIO();//��ʼ��
    ClockInit();
    TimerInit();

	__no_operation();                         // For debugger
    __delay_cycles(MCLK_FREQ*2);


    for (;;)
    {
        for (i=0; i<=255; i+=4) {
            TA2CCR2 = i*(0x00FF);
            __delay_cycles(MCLK_FREQ*0.05);
        }

        for (i=255; i>=0; i-=8) {
            TA2CCR2 = i*(0x00FF);
            __delay_cycles(MCLK_FREQ*0.05);
        }

        __delay_cycles(MCLK_FREQ*0.5);
    }

}


