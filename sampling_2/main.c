#include <msp430.h> 

void ioinit()
{//LED1~LED6的初始化
    P8DIR |= BIT1;  //p8.1 output
    P8OUT &= ~BIT1;

    P3DIR |= BIT7;  //p3.7 output
    P3OUT &= ~BIT7;

    P7DIR |= BIT4;  //p7.4 output
    P7OUT &= ~BIT4;

    P6DIR |= BIT3;  //p7.4 output
    P6OUT &= ~BIT3;

    P6DIR |= BIT4;  //p7.4 output
    P6OUT &= ~BIT4;

    P3DIR |= BIT5;  //p7.4 output
    P3OUT &= ~BIT5;

 //拨盘电位器对应IO初始化
    P6DIR |= BIT5;
    P6SEL |= BIT5;
}
void AD_Init()
{// 在ADC12ENC==0时（默认）,初始化各寄存器，后打开ADC转换使能（ADC12ENC==1）
    //多路采样转换（首次需要SHI信号上升沿触发采样定时器）自动循环采样转换
    ADC12CTL0 |= ADC12MSC;

    //启动（打开）ADC12模块
    ADC12CTL0 |= ADC12ON;

    //选择单通道多次采样转换模式
    ADC12CTL1 |= ADC12CONSEQ_2;

    //采样保持模式选择，选择采样信号（SAMPCON）的来源是采样定时器
    ADC12CTL1 |= ADC12SHP;

    //存储寄存器0选择通道？？,P6.5连接拨码电位器（GPIO默认方向是输入方向）
    ADC12MCTL0 |= ADC12INCH_5;

    //ADC转换使能
    ADC12CTL1 |= ADC12ENC;
}

void initClock()
{//最终效果：MCLK:16MHZ,SMCLK:8MHZ,ACLK:32KHZ
     UCSCTL6 &= ~XT1OFF;          //启动XT1
     P5SEL |= BIT2 + BIT3;        //XT2引脚功能选择
     UCSCTL6 &= ~XT2OFF;          //打开XT2

     //设置系统时钟生成器1,FLL control loop关闭SCG0=1,关闭锁频环，用户自定义UCSCTL0~1工作模式
     __bis_SR_register(SCG0);

     //手动选择DCO频率阶梯（8种阶梯），确定DCO频率大致范围。
     UCSCTL0 = DCO0+DCO1+DCO2+DCO3+DCO4;
     //DCO频率范围在28.2MHz以下，DCO频率范围选择（三个bit位，改变直流发生器电压，进而改变DCO输出频率）
     UCSCTL1 = DCORSEL_4;
     //fDCOCLK/32，锁频环分频器
     UCSCTL2 = FLLD_5;

     //n=8,FLLREFCLK时钟源为XT2CLK
     //DCOCLK=D*(N+1)*(FLLREFCLK/n)
     //DCOCLKDIV=(N+1)*(FLLREFCLK/n)
     UCSCTL3 = SELREF_5 + FLLREFDIV_3;
     //ACLK的时钟源为DCOCLKDIV,MCLK\SMCLK的时钟源为DCOCLK
     UCSCTL4 = SELA_4 + SELS_3 +SELM_3;
     //ACLK由DCOCLKDIV的32分频得到，SMCLK由DCOCLK的2分频得到
     UCSCTL5 = DIVA_5 +DIVS_1;
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    initClock();
    ioinit();
    AD_Init();

    ADC12CTL0 |= ADC12SC;                         //开始采样转换

    volatile unsigned int value = 0;                                 //设置判断变量
    for(;;)
    {

        value = ADC12MEM0;                           //把结果赋给变量

        //  请完成LED1~LED6灯的亮灭判断，value的有效范围是
        if (value >= 600)  {P8OUT |= BIT1;} else {P8OUT &= ~BIT1;}
        if (value >= 1200) {P3OUT |= BIT7;} else {P3OUT &= ~BIT7;}
        if (value >= 1800) {P7OUT |= BIT4;} else {P7OUT &= ~BIT4;}
        if (value >= 2400) {P6OUT |= BIT3;} else {P6OUT &= ~BIT3;}
        if (value >= 3000) {P6OUT |= BIT4;} else {P6OUT &= ~BIT4;}
        if (value >= 3600) {P3OUT |= BIT5;} else {P3OUT &= ~BIT5;}

    }
}
