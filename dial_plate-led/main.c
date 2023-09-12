#include <msp430.h>
#define N 12
#include<stdio.h>
void ioinit()
{//LED1~LED6�ĳ�ʼ��
   P8DIR|=BIT1 ;     //LED1
   P8OUT&=~BIT1;
   P3DIR|=BIT7;      //LED2
   P3OUT&=~BIT7;
   P7DIR|=BIT4;      //LED3
   P7OUT&=~BIT4;
   P6DIR|=BIT3;      //LED4
   P6OUT&=~BIT3;
   P6DIR|=BIT4;      //LED5
   P6OUT&=~BIT4;
   P3DIR|=BIT5;      //LED6
   P3OUT&=~BIT5;
   P6SEL|=BIT2;     //��˷��ӦIO��ʼ��
   P6DIR|=BIT2;
}
void AD_Init()
{// ��ADC12ENC==0ʱ��Ĭ�ϣ�,��ʼ�����Ĵ��������ADCת��ʹ�ܣ�ADC12ENC==1��
    //��·����ת�����״���ҪSHI�ź������ش���������ʱ�����Զ�ѭ������ת��
    ADC12CTL0 |= ADC12MSC;

    //�������򿪣�ADC12ģ��
    ADC12CTL0 |=ADC12ON ;

    //ѡ��ͨ����β���ת��ģʽ
    ADC12CTL1 |=ADC12CONSEQ_1  ;

    //��������ģʽѡ��ѡ������źţ�SAMPCON������Դ�ǲ�����ʱ��
    ADC12CTL1 |= ADC12SHP;

    //�洢�Ĵ���0ѡ��ͨ������,P6.5���Ӳ����λ����GPIOĬ�Ϸ��������뷽��
    ADC12MCTL0 |= ADC12INCH_2;

    //ADCת��ʹ��
    ADC12CTL0 |= ADC12ENC;

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

int Filter()
{
int count,i,j,temp;
int value_buf[N];
int sum=0;
for(count=0;count<N;count++)
{
  value_buf[count]=ADC12MEM0;
  _delay_cycles(500);
}
for(j=0;j<N-1;j++)
{
  for(i=0;i<N-j-1;i++)
    {
      if(value_buf[i]>value_buf[i+1])
      {
        temp=value_buf[i];
        value_buf[i]=value_buf[i+1];
        value_buf[i+1]=temp;
      }
    }
}
for(count=1;count<N-1;count++)
    sum+=value_buf[count];
return(int)(sum/(N-2));
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    initClock();
    ioinit();
    AD_Init();

    ADC12CTL0 |=ADC12SC ;                         //��ʼ����ת��

    volatile unsigned int value = 0;                                 //�����жϱ���
    while(1)
    {

        value = Filter();                           //�ѽ����������

        //  �����LED1~LED6�Ƶ������жϣ�value����Ч��Χ�ǣ���
        if(value>1500)
            P8OUT|=BIT1;
        else P8OUT&=~BIT1;
        if(value>2000��
            P3OUT|=BIT7;
        else P3OUT&=~BIT7;
        if(value>2500��
            P7OUT|=BIT4;
        else P7OUT&=~BIT4;
        if(value>3000)
            P6OUT|=BIT3;
        else P6OUT&=~BIT3;
        if(value>3500)
            P6OUT|=BIT4;
        else P6OUT&=~BIT4;
        if(value>4000)
            P3OUT|=BIT5;
        else P3OUT&=~BIT5;
    }
}
