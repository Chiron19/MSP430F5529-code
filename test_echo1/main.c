#include<msp430.h>
#include <stdio.h>
unsigned int k=0;
#define aaa 100
unsigned int LastCCR1;      //��һ�θߵ�ƽ������ʱ�䣬�;��������
unsigned int dat[20];       //ͨ�����鱸�ݲɼ���������
unsigned char times=1;

void initLED(void)
{
    P8DIR |= BIT1;
    P3DIR |= BIT7;
    P7DIR |= BIT4;
    P6DIR |= BIT3;
}
void resetLED(void)
{
    P8OUT &= ~BIT1;
    P3OUT &= ~BIT7;
    P7OUT &= ~BIT4;
    P6OUT &= ~BIT3;
}
void igniteLED(void)
{
    P8OUT |= BIT1;
    P3OUT |= BIT7;
    P7OUT |= BIT4;
    P6OUT |= BIT3;
}
void runLED(void)
{
    int p;
    P8OUT |= BIT1;  for (p=0; p<k; ++p) __delay_cycles(aaa);  P8OUT &= ~BIT1;
    P3OUT |= BIT7;  for (p=0; p<k; ++p) __delay_cycles(aaa);  P3OUT &= ~BIT7;
    P7OUT |= BIT4;  for (p=0; p<k; ++p) __delay_cycles(aaa);  P7OUT &= ~BIT4;
    P6OUT |= BIT3;  for (p=0; p<k; ++p) __delay_cycles(aaa);  P6OUT &= ~BIT3;
}
/*��ʼ��HC_SR04Init()ģ��*/
void HC_SR04Init()
{
    P1DIR |= BIT2;
}
/*��ʼ��������*/
void HC_SR04Start()
{
    P1OUT |= BIT2;
    __delay_cycles(12);      //���ٳ���10us�ĸߵ�ƽ
    P1OUT &= ~BIT2;
//    __delay_cycles(100000);
}

/*��ʼ������ģʽ����ȡʱ��*/
void TimeGetInit()
{
    P1DIR &= ~BIT4; //P1.4ѡ��input����
    P1SEL |= BIT4;  //P1.4ѡ�����裬��ӦTACCR3

    TA0CTL = TASSEL__SMCLK + ID_0 + MC_2 + TACLR + TAIE;        //ʱ��ΪSMCLK��Continuous upģʽ����ʼʱ���������
    TA0CCTL3 = CM_1 + SCS + CAP + CCIE + CCIS_0;               //����ģʽ�������أ������ж�ʹ�ܣ��������룺CCI0A
}

void main()
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    HC_SR04Init();          //��ʼ������ģ��HC_SR04
    TimeGetInit();          //��ʼ�������������ţ�ģʽ��
    initLED();
    resetLED();
    __enable_interrupt();
    while(1)
    {
        HC_SR04Start();
        if (k > 100) {
            __disable_interrupt();
            resetLED();
            runLED();
            __enable_interrupt();
        }
        LPM0;               //�͹���ģʽ
    }
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void)
{
    __disable_interrupt();                        //���ж�
    static unsigned int i=0;
    unsigned int j;
    unsigned int t;

    if(times == 1)
    {
        LastCCR1 = TA0CCR3;       //��¼���ϴ�CCR3��ֵ
        TA0CCTL3 &= ~CM_1;        //�������ز���
        TA0CCTL3 |= CM_2;         //��Ϊ�½��ز���
        times++;
    }
    if(times == 0)
    {
        if(i < 20)                //�Ѳɼ�����20�ε�ֵ���ŵ�������
        {
            dat[i] = TA0CCR3 - LastCCR1;
            i++;
        }
        if(i >= 20)               //Ϊ�˷�ֹ������Ѳɼ�������ͷ��β��ֵȥ����ȡ�м��ƽ��ֵ
        {
            for(i=0;i<20;i++)
            {
                for(j=i+1;j<20;j++)
                {
                    if(dat[i]>dat[j])
                    {
                        t=dat[j];
                        dat[j]=dat[i];
                        dat[i]=t;
                    }
                }
            }
            t = 0;
            for(i=7;i<15;i++)  t += dat[i];
            k = t/8;
            i = 0;
        }
        TA0CCTL3 &= ~CM_2;      //����½��ش���
        TA0CCTL3 |= CM_1;       //��Ϊ�����ش���
        times++;                //�ı�times��ֵ
    }
    times &= 0x01;                //times>1ʱ��0
    LPM0_EXIT;                  //�˳��͹���ģʽ
    TA0CCTL3 &= ~CCIFG;         //����ж���Ӧ��־
    __enable_interrupt();                    //���ж�
}
