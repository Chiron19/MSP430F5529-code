#include <msp430.h> 
#include "MacroAndConst.h"

static volatile unsigned int systemOn = 1,  //ϵͳ���ر��
                             LEDcycle = 0,  //�������ڱ���
                             LEDon = 0,     //LED����
                             LEDPeriod = 0, //LED��������
                             volume = 0,    //��������ֵ
                             lightValue = 4095,     //��ǿ����ֵ
                             lightNum = 0,          //�ŵƵ�������
                             lightCmp[7]={0,700,600,500,400,300,200},//��ǿ�Ƚ�ֵ
                             target = 0xFFFE;       //Ŀ�����ڱ���

#define XT2_FREQ   4000000
#define MCLK_FREQ 16000000
#define SMCLK_FREQ 4000000
void ClockInit()
{
	 UCSCTL6 &= ~XT1OFF;    //����XT1
	 P5SEL |= BIT2 + BIT3;  //XT2���Ź���ѡ��
	 UCSCTL6 &= ~XT2OFF;    //��XT2
	 __bis_SR_register(SCG0);
	 UCSCTL0 = DCO0+DCO1+DCO2+DCO3+DCO4;
	 UCSCTL1 = DCORSEL_4;   //DCOƵ�ʷ�Χ��28.2MHZ����
	 UCSCTL2 = FLLD_5 + 1;  //D=16��N=1
	 UCSCTL3 = SELREF_5 + FLLREFDIV_3;      //n=8,FLLREFCLKʱ��ԴΪXT2CLK��DCOCLK=D*(N+1)*(FLLREFCLK/n);DCOCLKDIV=(N+1)*(FLLREFCLK/n);
	 UCSCTL4 = SELA_4 + SELS_3 +SELM_3;     //ACLK��ʱ��ԴΪDCOCLKDIV,MCLK\SMCLK��ʱ��ԴΪDCOCLK
	 UCSCTL5 = DIVA_5 +DIVS_1;              //ACLK��DCOCLKDIV��32��Ƶ�õ���SMCLK��DCOCLK��2��Ƶ�õ�
	 //����MCLK:16MHZ,SMCLK:8MHZ,ACLK:32KHZ
//	 __bic_SR_register(SCG0);                   //Enable the FLL control loop
}
int Filter(int Reg)//��ֵ�˲���
{
    int i, j, t;
    int value_buf[N];
    int sum = 0;
    if (Reg == 0) {
        for (i = 0; i < N; i++)
        {
            value_buf[i] = ADC12MEM0;
            _delay_cycles(1000);
        }
    }
    else {
        for (i = 0; i < N; i++)
        {
            value_buf[i] = ADC12MEM1;
            _delay_cycles(50);
        }
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
void light_on()//�ŵ�ȫ��
{
    P8OUT |= BIT1;
    P3OUT |= BIT7;
    P7OUT |= BIT4;
    P6OUT |= BIT3;
    P6OUT |= BIT4;
    P3OUT |= BIT5;
}
void light_off()//�ŵ�ȫ��
{
    P8OUT &= ~BIT1;
    P3OUT &= ~BIT7;
    P7OUT &= ~BIT4;
    P6OUT &= ~BIT3;
    P6OUT &= ~BIT4;
    P3OUT &= ~BIT5;
}
void light_ignite()//�ŵư��չ�ǿ����
{
    //  LED1~LED6�Ƶ������ж�
    if (lightValue <= lightCmp[1] && systemOn) {P8OUT |= BIT1; lightNum = 1;} else {P8OUT &= ~BIT1;}
    if (lightValue <= lightCmp[2] && systemOn) {P3OUT |= BIT7; lightNum = 2;} else {P3OUT &= ~BIT7;}
    if (lightValue <= lightCmp[3] && systemOn) {P7OUT |= BIT4; lightNum = 3;} else {P7OUT &= ~BIT4;}
    if (lightValue <= lightCmp[4] && systemOn) {P6OUT |= BIT3; lightNum = 4;} else {P6OUT &= ~BIT3;}
    if (lightValue <= lightCmp[5] && systemOn) {P6OUT |= BIT4; lightNum = 5;} else {P6OUT &= ~BIT4;}
    if (lightValue <= lightCmp[6] && systemOn) {P3OUT |= BIT5; lightNum = 6;} else {P3OUT &= ~BIT5;}
}
void LEDstart(void)//LED���ƣ�����
{
    P1OUT |= BIT5; //��P1.5
    for (; (LEDPeriod < 0xFF00) && systemOn; LEDPeriod += 0x00FF) {
        TA2CCR2 = LEDPeriod;
        __delay_cycles(MCLK_FREQ*0.005);
    }
}
void LEDend(void)//LEDϨ�ƣ�����
{
    for (; LEDPeriod >= 0x01FF; LEDPeriod -= 0x00FF) {
        TA2CCR2 = LEDPeriod;
        __delay_cycles(MCLK_FREQ*0.01);
    }
    P1OUT &= ~BIT5; //Ҫ�����߿���P1.5�ص�������LED���й�
    LEDon = LEDcycle = 0;
}
void LEDadjust()//LED���ȵ���
{
    if (lightValue <= 200) target = 0x00FF;
    else if (lightValue >= 1000) target = 0xFFFE;
    else target = (lightValue-196)*80;
    if (LEDPeriod < target) {
        if (target - LEDPeriod <= 0x1FFF) return;
        for (; (LEDPeriod <= target && LEDPeriod < 0xFF00) && systemOn; LEDPeriod += 0x00FF) {
                TA2CCR2 = LEDPeriod;
                __delay_cycles(MCLK_FREQ*0.005);
         }
    }
    else {
        if (LEDPeriod - target <= 0x1FFF) return;
        for (; (LEDPeriod >= target && LEDPeriod >= 0x00FF) && systemOn; LEDPeriod -= 0x007F) {
                TA2CCR2 = LEDPeriod;
                __delay_cycles(MCLK_FREQ*0.005);
         }
    }
}
/***********************************************************************************
   IO�˿ڳ�ʼ��
    ˵���� �ڴ���LED�ƶ˿������������L1-->P8.1; L2-->P3.7; L3--> P7.4; L4-->P6.3; L5-->P6.4; L6-->P3.5
                 ��е��ť���룬������S1-->P1.2; S2-->P1.3;
***********************************************************************************/
void InitIO()
{
                    //LED1~LED6�ĳ�ʼ��
    P8DIR |= BIT1;  //p8.1 output
    P3DIR |= BIT7;  //p3.7 output
    P7DIR |= BIT4;  //p7.4 output
    P6DIR |= BIT3;  //p6.3 output
    P6DIR |= BIT4;  //p6.4 output
    P3DIR |= BIT5;  //p3.5 output

                    //��˷��ӦIO��ʼ��
    P6DIR |= BIT2;
    P6SEL |= BIT2;
                    //���������ӦIO��ʼ��
    P6DIR |= BIT1;
    P6SEL |= BIT1;

    P1DIR &= ~BIT2;
    P1REN |= BIT2;  // ʹ��P1.2���������蹦��
    P1OUT |= BIT2;  // ��P1.2Ϊ�������跽ʽ
    P1IE  |= BIT2;  //enable P1.2 interrupt
    P1IES |= BIT2;  //high-low transition

                    //LED�ƿ���
	P1DIR |= BIT5;
	P1OUT |= BIT5;

	P2SEL |= BIT5; //����P2.5��TA2.2����
	P2DIR |= BIT5; //����P1.5��P2.5Ϊ���ģʽ

	                //����Ϩ�����LED
	systemOn = 1;
	light_on();
    LEDstart();
    light_off();
    LEDend();
    systemOn = 0;
}
void AD_Init()
{// ��ADC12ENC==0ʱ��Ĭ�ϣ�,��ʼ�����Ĵ��������ADCת��ʹ�ܣ�ADC12ENC==1��
    ADC12CTL0 |= ADC12MSC;//��·����ת�����״���ҪSHI�ź������ش���������ʱ�����Զ�ѭ������ת��
    ADC12CTL0 |= ADC12ON; //�������򿪣�ADC12ģ��

    ADC12CTL1 |= ADC12CONSEQ_3;//ѡ������ͨ�����ѭ������ת��
    ADC12CTL1 |= ADC12CSTARTADD_0; //��ʼ��ַʹ�üĴ���0
    ADC12CTL1 |= ADC12SHP;//��������ģʽѡ��ѡ������źţ�SAMPCON������Դ�ǲ�����ʱ��

    //�洢�Ĵ���0ѡ��ͨ��P6.2��˷�
    ADC12MCTL0 |= ADC12INCH_2;
    //�洢�Ĵ���1ѡ��ͨ��P6.1��������
    ADC12MCTL1 |= ADC12INCH_0 + ADC12EOS;//End of sequence�ص��Ĵ���0

    //ADCת��ʹ��
    ADC12CTL0 |= ADC12ENC;
}
void TimerInit(void)//TimerA��ʼ��
{
    TA2CTL = TASSEL__SMCLK + TACLR + MC_1;
    TA2CCR0 = 0xFFFF;
    TA2CCTL2 = OUTMOD_7;                            //��λ-��ת
    
    TA0CTL |= TASSEL__ACLK + TACLR + MC_1 + TAIE;   //ʱ��ΪACLK(32768Hz),������ģʽ����ʼʱ���������
    TA0CCTL0 =  CCIE;                               //�Ƚ����ж�ʹ��
    TA0CCR0  = 32768;                               //����1���һ���ж�

    __enable_interrupt();
}


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
                            //��ʼ��
    ClockInit();
    TimerInit();
    AD_Init();
    InitIO();

    __no_operation();       // For debugger
    __delay_cycles(MCLK_FREQ*2);

    ADC12CTL0 |= ADC12SC;   //��ʼ����

    for(;;)
    {
       if (systemOn == 0) {
           __delay_cycles(10000);
           continue;
       }

       volume = Filter(0);      //�����������˲�
       lightValue = Filter(1);  //��ǿ�������˲�
       if (volume >= 3000 && systemOn) {
           if (!LEDon) LEDstart();
           LEDon = 1;
           LEDcycle = 0;        //���¿�ʼ��ʱ
       }
       if (!LEDon) light_ignite();  //LEDδ����ʱ�ŵƸ��ݹ�ǿ����
       else LEDadjust();            //���ݹ�ǿ����LED����
    }
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    if (P1IFG & BIT2) {
        __delay_cycles(100); //ʱ�ӷ�ֹ����
        if (P1IFG & BIT2)
        {
            if (systemOn) {
                systemOn = 0;
                light_off();
                LEDend();
            }
            else {
                systemOn = 1;
                LEDon = LEDcycle = 0;
            }
            P1IFG &= ~BIT2; //����жϱ��
        }
    }
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
      if (LEDon != 0) {
          LEDcycle++;
          if(LEDcycle >= 30) LEDend();//�����ص���ʱ�ļ���ֵ
      }
      TA0CTL &= ~TAIFG; //����жϱ��
      TA0CTL |= TACLR;  //���������
}
