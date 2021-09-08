#include <msp430.h> 

# define SET(Reg,n)    ( Reg |= n  )
# define CLR(Reg,n)    ( Reg &= ~n )

#define  LED1ON   SET(P8OUT,BIT1)
#define  LED1OFF  CLR(P8OUT,BIT1)

#define  LED2ON   SET(P3OUT,BIT7)
#define  LED2OFF  CLR(P3OUT,BIT7)

#define  LED3ON   SET(P7OUT,BIT4)
#define  LED3OFF  CLR(P7OUT,BIT4)

#define  LED4ON   SET(P6OUT,BIT3)
#define  LED4OFF  CLR(P6OUT,BIT3)

#define  LED5ON   SET(P6OUT,BIT4)
#define  LED5OFF  CLR(P6OUT,BIT4)

#define  LED6ON   SET(P3OUT,BIT5)
#define  LED6OFF  CLR(P3OUT,BIT5)

void ioinit()
{//LED1~LED6�ĳ�ʼ��
    P8DIR|=BIT1;
    P8OUT&=~BIT1;
    P3DIR|=BIT7;
    P3OUT&=~BIT7;
    P7DIR|=BIT4;
    P7OUT&=~BIT4;
    P6DIR|=BIT3;
    P6OUT&=~BIT3;
    P6DIR|=BIT4;
    P6OUT&=~BIT4;
    P3DIR|=BIT5;
    P3OUT&=~BIT5;

 //���̵�λ����ӦIO��ʼ��
    P6SEL |= BIT1;
}
void AD_Init()
{// ��ADC12ENC==0ʱ��Ĭ�ϣ�,��ʼ�����Ĵ��������ADCת��ʹ�ܣ�ADC12ENC==1��
    //��·����ת�����״���ҪSHI�ź������ش���������ʱ�����Զ�ѭ������ת��
    ADC12CTL0 |= ADC12MSC;

    //�������򿪣�ADC12ģ��
    ADC12CTL0 |= ADC12ON;

    //ѡ��ͨ����β���ת��ģʽ
    ADC12CTL1 |= ADC12CONSEQ_2;

    //��������ģʽѡ��ѡ������źţ�SAMPCON������Դ�ǲ�����ʱ��
    ADC12CTL1 |= ADC12SHP;

    //�洢�Ĵ���0ѡ��ͨ��A5,P6.5���Ӳ����λ����GPIOĬ�Ϸ��������뷽��
    ADC12MCTL1 |= ADC12INCH_1;

    //�޸���ʼ��ַ ������ADCת��ʹ��ǰ�棬��Ϊת��ʹ���øߺ������üĴ���
    ADC12CTL1 |= ADC12CSTARTADD_1;

    //ADCת��ʹ��
    ADC12CTL0 |= ADC12ENC;


}

void initClock()
{    //����Ч����MCLK:16MHZ,SMCLK:8MHZ,ACLK:32KHZ
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

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    initClock();
    ioinit();
    AD_Init();

    ADC12CTL0 |= ADC12SC;                         //��ʼ����ת��

    volatile unsigned int value = 0;                           //�����жϱ���
    while(1)
    {
        value = ADC12MEM1;                           //�ѽ����������

        //  �����LED1~LED6�Ƶ������жϣ�value����Ч��Χ�ǣ���
        if(value<702){LED1ON;}

        if(value>=702&&value<1380){LED1ON;LED2ON;}

        if(value>=1380&&value<2056){LED1ON;LED2ON;LED3ON;}

        if(value>=2056&&value<2728){LED1ON;LED2ON;LED3ON;LED4ON;}

        if(value>=2728&&value<3210){LED1ON;LED2ON;LED3ON;LED4ON;LED5ON;}

        if(value>=3210) {LED1ON;LED2ON;LED3ON;LED4ON;LED5ON;LED6ON;}
        LED1OFF;LED2OFF;LED3OFF;LED4OFF;LED5OFF;LED6OFF;

    }
}
