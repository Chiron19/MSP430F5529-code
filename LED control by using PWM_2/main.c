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
int Filter()
{
    int i, j, t;
    int value_buf[N];
    int sum = 0;
    for (i = 0; i < N; i++)
    {
        value_buf[i] = ADC12MEM0;
        _delay_cycles(5000);
    }
    for (j=0; j<N-1; j++)
    {
        for (i=0; i<N-j-1; i++)
        {
            if (value_buf[i] > value_buf[i+1])
            {
                int temp = value_buf[i];
                value_buf[i] = value_buf[i+1];
                value_buf[i+1] = temp;
            }
        }
    }
    for(i=1; i<N-1; i++) sum += value_buf[i];
    t = sum/(N-2);
    return t;
}
void light_off()
{
    P8OUT &= ~BIT1;
    P3OUT &= ~BIT7;
    P7OUT &= ~BIT4;
    P6OUT &= ~BIT3;
    P6OUT &= ~BIT4;
    P3OUT &= ~BIT5;
}
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
void ioinit()
{//LED1~LED6�ĳ�ʼ��
    P8DIR |= BIT1;  //p8.1 output

    P3DIR |= BIT7;  //p3.7 output

    P7DIR |= BIT4;  //p7.4 output

    P6DIR |= BIT3;  //p7.4 output

    P6DIR |= BIT4;  //p7.4 output

    P3DIR |= BIT5;  //p7.4 output

    light_off();
 //���̵�λ����ӦIO��ʼ��
    P6DIR |= BIT5;
    P6SEL |= BIT5;

    P6DIR |= BIT2;
    P6SEL |= BIT2;

    P1DIR &= ~BIT2;
    P1REN |= BIT2;                       // ʹ��P1.2���������蹦��
    P1OUT |= BIT2;                        // ��P1.2Ϊ�������跽ʽ
    P1IE  |= BIT2; //enable P1.2 interrupt
    P1IES |= BIT2; //high-low transition
}
void AD_Init()
{// ��ADC12ENC==0ʱ��Ĭ�ϣ�,��ʼ�����Ĵ��������ADCת��ʹ�ܣ�ADC12ENC==1��
    //��·����ת�����״���ҪSHI�ź������ش���������ʱ�����Զ�ѭ������ת��
    ADC12CTL0 |= ADC12MSC;

    //�������򿪣�ADC12ģ��
    ADC12CTL0 |= ADC12ON;

    ADC12CTL1 |= ADC12CSTARTADD_0;

    //ѡ��ͨ����β���ת��ģʽ
    ADC12CTL1 |= ADC12CONSEQ_2;

    //��������ģʽѡ��ѡ������źţ�SAMPCON������Դ�ǲ�����ʱ��
    ADC12CTL1 |= ADC12SHP;

    //�洢�Ĵ���0ѡ��ͨ������,P6.5���Ӳ����λ����GPIOĬ�Ϸ��������뷽��
    ADC12MCTL0 |= ADC12INCH_2;

    //ADCת��ʹ��
    ADC12CTL0 |= ADC12ENC;
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


static volatile unsigned int ADon = 0;
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    ClockInit();
    ioinit();
    AD_Init();
    InitIO();//��ʼ��
    TimerInit();

    ADC12CTL0 |= ADC12SC;                         //��ʼ����ת��

    volatile unsigned int value[2] = {0,0}, w = 0;
    volatile int i;
                                    //�����жϱ���
    for(;;)
    {
        if (ADon == 0) {
            _delay_cycles(50000);
            continue;
        }
        value[0] = Filter(), value[1] = ADC12MEM1;                           //�ѽ����������
        w = value[0];
        _delay_cycles(200000);
        //  �����LED1~LED6�Ƶ������жϣ�value����Ч��Χ�ǣ���
        if ((w >= 2100 || value[1] > 1500) && ADon)  {P8OUT |= BIT1;} else {P8OUT &= ~BIT1;}
        if (w >= 2200 && ADon) {P3OUT |= BIT7;} else {P3OUT &= ~BIT7;}
        if (w >= 2300 && ADon) {P7OUT |= BIT4;} else {P7OUT &= ~BIT4;}
        if (w >= 2400 && ADon) {P6OUT |= BIT3;} else {P6OUT &= ~BIT3;}
        if (w >= 2500 && ADon) {P6OUT |= BIT4;} else {P6OUT &= ~BIT4;}
        if (w >= 2600 && ADon) {P3OUT |= BIT5;} else {P3OUT &= ~BIT5;}

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

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    if (P1IFG & BIT2) {
        _delay_cycles(50);
        if (P1IFG & BIT2) {
            ADon ^= 1;
            light_off();
        }
    }
    P1IFG &= ~BIT2;
}
