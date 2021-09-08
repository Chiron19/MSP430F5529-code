#include<msp430.h>
#define aaa 100
#define signalDelay 1000000
#define sampleNum 9

/*������ģ�����
 * ��Trig����Trig  --> P1.2
 * ��Echo          --> P1.4
 * ��Echo          --> P1.5
 */

unsigned int k=0, kk=0;
unsigned int i=0;
unsigned int ii=0;
unsigned int LastCCR2, LastCCR4;      //��һ�θߵ�ƽ������ʱ�䣬�;��������
unsigned int dat1[20], dat2[20];       //ͨ�����鱸�ݲɼ���������
unsigned char times_TACCR3=1, times_TACCR4=1;
unsigned char modeSel[5] = {0,0,0,0,0};
unsigned int modeCnt = 0;

void initLED(void)
{
    P8DIR |= BIT1;
    P3DIR |= BIT7;
    P7DIR |= BIT4;
    P6DIR |= BIT3;
    P6DIR |= BIT4;
    P3DIR |= BIT5;
}
/*�����ȫ��Ϩ��*/
void resetLED(void)
{
    P8OUT &= ~BIT1;
    P3OUT &= ~BIT7;
    P7OUT &= ~BIT4;
    P6OUT &= ~BIT3;
    P6OUT &= ~BIT4;
    P3OUT &= ~BIT5;
}
/*�����ȫ������*/
void igniteLED(void)
{
    P8OUT |= BIT1;
    P3OUT |= BIT7;
    P7OUT |= BIT4;
    P6OUT |= BIT3;
    P6OUT |= BIT4;
    P3OUT |= BIT5;
}
void igniteLED0(void)
{
    P8OUT |= BIT1;
    P3OUT |= BIT7;
    P7OUT |= BIT4;
}
void igniteLED1(void)
{
    P6OUT |= BIT3;
    P6OUT |= BIT4;
    P3OUT |= BIT5;
}
void igniteLED_single(int x)
{
    switch(x){
        case 1: P8OUT |= BIT1; break;
        case 2: P3OUT |= BIT7; break;
        case 3: P7OUT |= BIT4; break;
        case 4: P6OUT |= BIT3; break;
        case 5: P6OUT |= BIT4; break;
        case 6: P3OUT |= BIT5; break;
        default: break;
    }
}
/*�����������*/
void runLED0(int g)
{
    int p;
    P8OUT |= BIT1;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P8OUT &= ~BIT1;
    P3OUT |= BIT7;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P3OUT &= ~BIT7;
    P7OUT |= BIT4;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P7OUT &= ~BIT4;
    P6OUT |= BIT3;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P6OUT &= ~BIT3;
    P6OUT |= BIT4;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P6OUT &= ~BIT4;
    P3OUT |= BIT5;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P3OUT &= ~BIT5;
}
/*���ҵ��������*/
void runLED1(int g)
{
    int p;
    P3OUT |= BIT5;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P3OUT &= ~BIT5;
    P6OUT |= BIT4;  for (p=0; p<g; ++p) __delay_cycles(aaa);  P6OUT &= ~BIT4;
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
    P1DIR &= ~BIT4; //P1.4ѡ��input����
    P1SEL |= BIT4;  //P1.4ѡ�����裬��ӦTA0CCR3

    P1DIR &= ~BIT5; //P1.5ѡ��input����
    P1SEL |= BIT5;  //P1.5ѡ�����裬��ӦTA0CCR4

    TA0CTL = TASSEL__SMCLK + ID_0 + MC_2 + TACLR + TAIE;        //ʱ��ΪSMCLK��Continuous upģʽ����ʼʱ���������
    TA0CCTL3 = CM_1 + SCS + CAP + CCIE + CCIS_0;               //����ģʽ�������أ������ж�ʹ�ܣ��������룺CCI0A
    TA0CCTL4 = CM_1 + SCS + CAP + CCIE + CCIS_0;               //����ģʽ�������أ������ж�ʹ�ܣ��������룺CCI0A
}
void resetBuf(void)
{
    k = kk = 0;
    i = ii = 0;
    int p;
    for (p=0; p<sampleNum; ++p) dat1[p] = dat2[p] = 0;
    times_TACCR3 = times_TACCR4 = 1;
}
/*�����ж�*/
void signalJudge(void)
{
    HC_SR04Start();
    if ((kk > 100 && k > 100) && (kk < 1000 && k < 1000)) {
        __disable_interrupt();
//        modeSel[modeCnt] = 1;
        resetLED();
        __delay_cycles(signalDelay);
        igniteLED();
        __delay_cycles(signalDelay);
        resetLED();
        resetBuf();
//        modeCnt++;
        __enable_interrupt();
        return;
    }
    if ((k > 100 && kk <= 100) && (kk < 1500 && k < 1500)) {
        __disable_interrupt();
//        modeSel[modeCnt] = 2;
        resetLED();
        runLED0(k);
        __delay_cycles(signalDelay);
        resetLED();
        resetBuf();
//        modeCnt++;
        __enable_interrupt();
        return;
    }
    if ((kk > 100 && k <= 100) && (kk < 1500 && k < 1500)) {
        __disable_interrupt();
//        modeSel[modeCnt] = 3;
        resetLED();
        runLED1(kk);
        __delay_cycles(signalDelay);
        resetLED();
        resetBuf();
//        modeCnt++;
        __enable_interrupt();
        return;
    }
//    __disable_interrupt();
//    resetBuf();
//    __enable_interrupt();
}
void signalRes(void)
{
    if (modeCnt < 2) return;
    __disable_interrupt();
    int a = modeSel[0];
    int b = modeSel[1];
    modeSel[0] = modeSel[1] = modeCnt = 0;
    if (a == 1 && b == 1) {
        resetLED();
        igniteLED_single(1);
        __delay_cycles(signalDelay);
        resetLED();
        __enable_interrupt();
        return;
    }
    if (a == 2 && b == 2) {
        resetLED();
        igniteLED_single(2);
        __delay_cycles(signalDelay);
        resetLED();
        __enable_interrupt();
        return;
    }
    if (a == 2 && b == 3) {
        resetLED();
        runLED0(1000);
        resetLED();
        __enable_interrupt();
        return;
    }
    if (a == 3 && b == 3) {
        resetLED();
        igniteLED_single(1);
        __delay_cycles(signalDelay);
        resetLED();
        __enable_interrupt();
        return;
    }
    if (a == 3 && b == 2) {
        resetLED();
        runLED1(1000);
        resetLED();
        __enable_interrupt();
        return;
    }
    resetLED();
    runLED1(500);
    runLED0(1000);
    runLED1(500);
    resetLED();
    __enable_interrupt();
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
        signalJudge();
//        signalRes();
        LPM0;               //�͹���ģʽ
    }
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void)
{
    __disable_interrupt();                        //���ж�
    static unsigned int L=sampleNum/3;
    static unsigned int R=sampleNum*2/3;
    unsigned int j;
    unsigned int t;

    if (~(TA0CCTL3 & CCIFG)) {
        if(times_TACCR3 == 1)
        {
            LastCCR2 = TA0CCR3;       //��¼���ϴ�CCR3��ֵ
            TA0CCTL3 &= ~CM_1;        //�������ز���
            TA0CCTL3 |= CM_2;         //��Ϊ�½��ز���
            times_TACCR3++;
        }
        if(times_TACCR3 == 0)
        {
            if(i < sampleNum)                //�Ѳɼ�����10�ε�ֵ���ŵ�������
            {
                dat1[i] = TA0CCR3 - LastCCR2;
                i++;
            }
            if(i >= sampleNum)               //Ϊ�˷�ֹ������Ѳɼ�������ͷ��β��ֵȥ����ȡ�м��ƽ��ֵ
            {
                for(i=0;i<sampleNum;i++)
                {
                    for(j=i+1;j<sampleNum;j++)
                    {
                        if(dat1[i]>dat1[j])
                        {
                            t=dat1[j];
                            dat1[j]=dat1[i];
                            dat1[i]=t;
                        }
                    }
                }
                t = 0;
                for(i=L;i<R;i++)  t += dat1[i];
                k = t/(R-L);
                i = 0;
            }
            TA0CCTL3 &= ~CM_2;      //����½��ش���
            TA0CCTL3 |= CM_1;       //��Ϊ�����ش���
            times_TACCR3++;                //�ı�times_TACCR3��ֵ
        }
        times_TACCR3 &= 0x01;                //times_TACCR3>1ʱ��0
        TA0CCTL3 &= ~CCIFG;         //����ж���Ӧ��־
    }
    if (~(TA0CCTL4 & CCIFG)) {
        if(times_TACCR4 == 1)
        {
            LastCCR4 = TA0CCR4;       //��¼���ϴ�CCR4��ֵ
            TA0CCTL4 &= ~CM_1;        //�������ز���
            TA0CCTL4 |= CM_2;         //��Ϊ�½��ز���
            times_TACCR4++;
        }
        if(times_TACCR4 == 0)
        {
            if(ii < sampleNum)                //�Ѳɼ�����20�ε�ֵ���ŵ�������
            {
                dat2[ii] = TA0CCR4 - LastCCR4;
                ii++;
            }
            if(ii >= sampleNum)               //Ϊ�˷�ֹ������Ѳɼ�������ͷ��β��ֵȥ����ȡ�м��ƽ��ֵ
            {
                for(ii=0;ii<sampleNum;ii++)
                {
                    for(j=ii+1;j<sampleNum;j++)
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
                for(ii=L;ii<R;ii++)  t += dat2[ii];
                kk = t/(R-L);
                ii = 0;
            }
            TA0CCTL4 &= ~CM_2;      //����½��ش���
            TA0CCTL4 |= CM_1;       //��Ϊ�����ش���
            times_TACCR4++;         //�ı�times_TACCR4��ֵ
        }
        times_TACCR4 &= 0x01;       //times_TACCR4>1ʱ��0
        TA0CCTL4 &= ~CCIFG;         //����ж���Ӧ��־
    }
    LPM0_EXIT;                  //�˳��͹���ģʽ
    __enable_interrupt();       //���ж�
}
