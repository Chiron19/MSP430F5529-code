#include<msp430.h>
#define aaa 100
#define signalDelay 1000000

/*������ģ�����
 * ��Trig����Trig  --> P1.2
 * ��Echo          --> P1.4
 * ��Echo          --> P1.5
 */


unsigned int kk=0;
unsigned int LastCCR4;      //��һ�θߵ�ƽ������ʱ�䣬�;��������
unsigned int dat2[20];       //ͨ�����鱸�ݲɼ���������
unsigned char times_TACCR4=1;

void initLED(void)
{
    P8DIR |= BIT1;
    P3DIR |= BIT7;
    P7DIR |= BIT4;
    P6DIR |= BIT3;
}
/*�����ȫ��Ϩ��*/
void resetLED(void)
{
    P8OUT &= ~BIT1;
    P3OUT &= ~BIT7;
    P7OUT &= ~BIT4;
    P6OUT &= ~BIT3;
}
/*�����ȫ������*/
void igniteLED(void)
{
    P8OUT |= BIT1;
    P3OUT |= BIT7;
    P7OUT |= BIT4;
    P6OUT |= BIT3;
}
/*�����������*/
void runLED0(int g)
{
    int p;
    P8OUT |= BIT1;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P8OUT &= ~BIT1;
    P3OUT |= BIT7;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P3OUT &= ~BIT7;
    P7OUT |= BIT4;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P7OUT &= ~BIT4;
    P6OUT |= BIT3;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P6OUT &= ~BIT3;
}
/*���ҵ��������*/
void runLED1(int g)
{
    int p;
    P6OUT |= BIT3;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P6OUT &= ~BIT3;
    P7OUT |= BIT4;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P7OUT &= ~BIT4;
    P3OUT |= BIT7;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P3OUT &= ~BIT7;
    P8OUT |= BIT1;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P8OUT &= ~BIT1;
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
}

/*��ʼ������ģʽ����ȡʱ��*/
void TimeGetInit()
{
    P1DIR &= ~BIT5; //P1.5ѡ��input����
    P1SEL |= BIT5;  //P1.5ѡ�����裬��ӦTA0CCR4

    TA0CTL = TASSEL__SMCLK + ID_0 + MC_2 + TACLR + TAIE + TAIFG;        //ʱ��ΪSMCLK��Continuous upģʽ����ʼʱ���������
//    TA0CCTL3 = CM_1 + SCS + CAP + CCIE + CCIS_0;               //����ģʽ�������أ������ж�ʹ�ܣ��������룺CCI0A
    TA0CCTL4 = CM_1 + SCS + CAP + CCIE + CCIS_0;               //����ģʽ�������أ������ж�ʹ�ܣ��������룺CCI0A
}
/*�����ж�*/
void signalJudge(void)
{
    if (kk > 100) {
        __disable_interrupt();
        resetLED();
        runLED1(kk);
        __enable_interrupt();
    }
}

void main()
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    HC_SR04Init();          //��ʼ������ģ��HC_SR04
    TimeGetInit();          //��ʼ�������������ţ�ģʽ��
    initLED();
    resetLED();
    __enable_interrupt();
    for(;;)
    {
        HC_SR04Start();
        signalJudge();
        LPM0;               //�͹���ģʽ
    }
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void)
{
    __disable_interrupt();                        //���ж�
    static unsigned int ii=0;
    unsigned int j;
    unsigned int t;

        if(times_TACCR4 == 1)
        {
            LastCCR4 = TA0CCR4;       //��¼���ϴ�CCR4��ֵ
            TA0CCTL4 &= ~CM_1;        //�������ز���
            TA0CCTL4 |= CM_2;         //��Ϊ�½��ز���
            times_TACCR4++;
        }
        if(times_TACCR4 == 0)
        {
            if(ii < 20)                //�Ѳɼ�����20�ε�ֵ���ŵ�������
            {
                dat2[ii] = TA0CCR4 - LastCCR4;
                ii++;
            }
            if(ii >= 20)               //Ϊ�˷�ֹ������Ѳɼ�������ͷ��β��ֵȥ����ȡ�м��ƽ��ֵ
            {
                for(ii=0;ii<20;ii++)
                {
                    for(j=ii+1;j<20;j++)
                    {
                        if(dat2[ii]>dat2[j])
                        {
                            t=dat2[j];
                            dat2[j]=dat2[ii];
                            dat2[ii]=t;
                        }
                    }
                }
                t = 0;
                for(ii=7;ii<15;ii++)  t += dat2[ii];
                kk = t/8;
                ii = 0;
            }
            TA0CCTL4 &= ~CM_2;      //����½��ش���
            TA0CCTL4 |= CM_1;       //��Ϊ�����ش���
            times_TACCR4++;         //�ı�times_TACCR4��ֵ
        }
        times_TACCR4 &= 0x01;       //times_TACCR4>1ʱ��0
        TA0CCTL4 &= ~CCIFG;         //����ж���Ӧ��־

    LPM0_EXIT;                  //�˳��͹���ģʽ
    __enable_interrupt();       //���ж�
}
