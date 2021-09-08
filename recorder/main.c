/*main.c*/
#include <msp430.h> 
#define N 12

int Filter()//��ֵ�˲���
{
    int i, j, t;
    int value_buf[N];
    int sum = 0;
    for (i = 0; i < N; i++)
    {
        value_buf[i] = ADC12MEM0;
        _delay_cycles(1000);
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
void light_off()//LED��ȫ��
{
    P8OUT &= ~BIT1;
    P3OUT &= ~BIT7;
    P7OUT &= ~BIT4;
    P6OUT &= ~BIT3;
    P6OUT &= ~BIT4;
    P3OUT &= ~BIT5;
}
void ioinit()
{//LED1~LED6�ĳ�ʼ��
    P8DIR |= BIT1;  //p8.1 output
    P3DIR |= BIT7;  //p3.7 output
    P7DIR |= BIT4;  //p7.4 output
    P6DIR |= BIT3;  //p6.3 output
    P6DIR |= BIT4;  //p6.4 output
    P3DIR |= BIT5;  //p3.5 output
    light_off();
 //���̵�λ����ӦIO��ʼ��
    P6DIR |= BIT5;
    P6SEL |= BIT5;
//��˷��ӦIO��ʼ��
    P6DIR |= BIT2;
    P6SEL |= BIT2;

    P1DIR &= ~BIT2;
    P1REN |= BIT2;              // ʹ��P1.2���������蹦��
    P1OUT |= BIT2;              // ��P1.2Ϊ�������跽ʽ
    P1IE  |= BIT2;              // ʹ��P1.2�ж�
    P1IES |= BIT2;              // �ߵ͵�ƽת��
}
void AD_Init()
{// ��ADC12ENC==0ʱ��Ĭ�ϣ�,��ʼ�����Ĵ��������ADCת��ʹ�ܣ�ADC12ENC==1��
    //��·����ת�����״���ҪSHI�ź������ش���������ʱ�����Զ�ѭ������ת��
    ADC12CTL0 |= ADC12MSC;

    //�������򿪣�ADC12ģ��
    ADC12CTL0 |= ADC12ON;

    //���ò�����ʼ��ַ
    ADC12CTL1 |= ADC12CSTARTADD_0;

    //ѡ������ͨ�����ѭ������ת��
    ADC12CTL1 |= ADC12CONSEQ_3;

    //��������ģʽѡ��ѡ������źţ�SAMPCON������Դ�ǲ�����ʱ��
    ADC12CTL1 |= ADC12SHP;

    //�洢�Ĵ���0ѡ��ͨ��P6.2��˷�,P6.5���Ӳ����λ����GPIOĬ�Ϸ��������뷽��
    ADC12MCTL0 |= ADC12INCH_2;
    ADC12MCTL1 |= ADC12INCH_5;

    //ADCת��ʹ��
    ADC12CTL0 |= ADC12ENC;
    ADC12CTL1 |= ADC12ENC;
}

void initClock()
{//����Ч����MCLK:16MHZ,SMCLK:8MHZ,ACLK:32KHZ
     UCSCTL6 &= ~XT1OFF;          //����XT1
     P5SEL |= BIT2 + BIT3;        //XT2���Ź���ѡ��
     UCSCTL6 &= ~XT2OFF;          //��XT2

     //����ϵͳʱ��������1,FLL control loop�ر�SCG0=1,�ر���Ƶ�����û��Զ���UCSCTL0~1����ģʽ
     __bis_SR_register(SCG0);

     //�ֶ�ѡ��DCOƵ�ʽ��ݣ�8�ֽ��ݣ���ȷ��DCOƵ�ʴ��·�Χ��
     UCSCTL0 = DCO0+DCO1+DCO2+DCO3+DCO4;
     //DCOƵ�ʷ�Χ��28.2MHz���£�DCOƵ�ʷ�Χѡ������bitλ���ı�ֱ����������ѹ�������ı�DCO���Ƶ�ʣ�
     UCSCTL1 = DCORSEL_4;
     //fDCOCLK/32����Ƶ����Ƶ��
     UCSCTL2 = FLLD_5;

     //n=8,FLLREFCLKʱ��ԴΪXT2CLK
     //DCOCLK=D*(N+1)*(FLLREFCLK/n)
     //DCOCLKDIV=(N+1)*(FLLREFCLK/n)
     UCSCTL3 = SELREF_5 + FLLREFDIV_3;
     //ACLK��ʱ��ԴΪDCOCLKDIV,MCLK\SMCLK��ʱ��ԴΪDCOCLK
     UCSCTL4 = SELA_4 + SELS_3 +SELM_3;
     //ACLK��DCOCLKDIV��32��Ƶ�õ���SMCLK��DCOCLK��2��Ƶ�õ�
     UCSCTL5 = DIVA_5 +DIVS_1;
}
static volatile unsigned int ADon = 0;
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    initClock();
    ioinit();
    AD_Init();

    _enable_interrupt();

    ADC12CTL0 |= ADC12SC;                         //��ʼ����ת��

    volatile unsigned int value[2] = {0,0}, g[7]={0,2005,2100,2130,2180,2220,2250};
    //g[1]~g[6]Ϊ��������ֵ���ݣ���Ϊ����
    for(;;)
    {
        if (ADon == 0) {                            //�����ر��Ϊ�أ��򲻽������ģ��
            _delay_cycles(50000);
            continue;
        }
        value[0] = Filter(), value[1] = ADC12MEM1;  //value[0]��¼�����˲�����,value[1]��¼�����λ������
        _delay_cycles(20000);                       //ʱ�ӿ���
        //���LED1~LED6�Ƶ������жϣ�����value����Ч��Χ
        if ((value[0] >= g[1] || value[1] > 1500) && ADon)  {P8OUT |= BIT1;} else {P8OUT &= ~BIT1;}
        if (value[0] >= g[2] && ADon) {P3OUT |= BIT7;} else {P3OUT &= ~BIT7;}
        if (value[0] >= g[3] && ADon) {P7OUT |= BIT4;} else {P7OUT &= ~BIT4;}
        if (value[0] >= g[4] && ADon) {P6OUT |= BIT3;} else {P6OUT &= ~BIT3;}
        if (value[0] >= g[5] && ADon) {P6OUT |= BIT4;} else {P6OUT &= ~BIT4;}
        if (value[0] >= g[6] && ADon) {P3OUT |= BIT5;} else {P3OUT &= ~BIT5;}

    }
}

#pragma vector=PORT1_VECTOR //�ж�����
__interrupt void Port_1(void) //�жϺ���
{
    if (P1IFG & BIT2) {
        _delay_cycles(50);
        if (P1IFG & BIT2) {
            ADon ^= 1;
            light_off();
        }
    }
    P1IFG &= ~BIT2; //����жϱ��
}


