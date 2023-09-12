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
//    P2OUT |= BIT5;        //����P2.4��P2.5Ϊ���ģʽ
//  P2OUT |= BIT4;        //����P2.4��P2.5Ϊ���ģʽ

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

    //�洢�Ĵ���0ѡ��ͨ��P6.2��˷�
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
    
    TA0CTL |= TASSEL__ACLK + TACLR + MC_1 + TAIE;   //ʱ��ΪSMCLK,������ģʽ����ʼʱ���������
    TA0CCTL0 =  CCIE;                               //�Ƚ����ж�ʹ��
    TA0CCR0  = 0xFFFF;                                //�Ƚ�ֵ��Ϊ100000���൱��100ms��ʱ����

    __enable_interrupt();
}

void LEDstart(void)
{
    volatile unsigned int i = 0;
    for (i=0; i<0xFFFF; i+=0x00FF) {
        TA2CCR2 = i;
        __delay_cycles(MCLK_FREQ*0.005);
    }
}
void LEDend(void)
{
    volatile unsigned int i = 0;
    for (i=0xFFFF; i>=0x00FF; i-=0x00FF) {
        TA2CCR2 = i;
        __delay_cycles(MCLK_FREQ*0.005);
    }
}
static volatile unsigned int ADon = 0, cnt_LEDcycle = 0, LEDon = 0;
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    ClockInit();
    ioinit();
    AD_Init();
    InitIO();//��ʼ��
    TimerInit();

    ADC12CTL0 |= ADC12SC;                         //��ʼ����ת��



    volatile unsigned int value = 0, w = 0;
                                    //�����жϱ���
    for(;;)
    {
        value = Filter();

        if (value >= 2200) {
            if (!LEDon) LEDstart();
            LEDon = 1;
            cnt_LEDcycle = 0;
        }

    }
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    if (P1IFG & BIT2) {
        _delay_cycles(50);
        if (P1IN & BIT2) {
            ADon ^= 1;
            light_off();
        }
    }
    P1IFG &= ~BIT2;
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
      if (LEDon) {
          cnt_LEDcycle++;
          if(cnt_LEDcycle >= 5) {
//              printf("%d\n", cnt_LEDcycle);
              LEDend();
              cnt_LEDcycle = LEDon = 0;
          }
      }
      TA0CTL &= ~TAIFG;
}
