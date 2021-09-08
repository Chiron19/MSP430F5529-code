/*main.c*/
#include <msp430.h> 
#define N 12

int Filter()//中值滤波法
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
void light_off()//LED灯全灭
{
    P8OUT &= ~BIT1;
    P3OUT &= ~BIT7;
    P7OUT &= ~BIT4;
    P6OUT &= ~BIT3;
    P6OUT &= ~BIT4;
    P3OUT &= ~BIT5;
}
void ioinit()
{//LED1~LED6的初始化
    P8DIR |= BIT1;  //p8.1 output
    P3DIR |= BIT7;  //p3.7 output
    P7DIR |= BIT4;  //p7.4 output
    P6DIR |= BIT3;  //p6.3 output
    P6DIR |= BIT4;  //p6.4 output
    P3DIR |= BIT5;  //p3.5 output
    light_off();
 //拨盘电位器对应IO初始化
    P6DIR |= BIT5;
    P6SEL |= BIT5;
//麦克风对应IO初始化
    P6DIR |= BIT2;
    P6SEL |= BIT2;

    P1DIR &= ~BIT2;
    P1REN |= BIT2;              // 使能P1.2上下拉电阻功能
    P1OUT |= BIT2;              // 置P1.2为上拉电阻方式
    P1IE  |= BIT2;              // 使能P1.2中断
    P1IES |= BIT2;              // 高低电平转换
}
void AD_Init()
{// 在ADC12ENC==0时（默认）,初始化各寄存器，后打开ADC转换使能（ADC12ENC==1）
    //多路采样转换（首次需要SHI信号上升沿触发采样定时器）自动循环采样转换
    ADC12CTL0 |= ADC12MSC;

    //启动（打开）ADC12模块
    ADC12CTL0 |= ADC12ON;

    //设置采样起始地址
    ADC12CTL1 |= ADC12CSTARTADD_0;

    //选择序列通道多次循环采样转换
    ADC12CTL1 |= ADC12CONSEQ_3;

    //采样保持模式选择，选择采样信号（SAMPCON）的来源是采样定时器
    ADC12CTL1 |= ADC12SHP;

    //存储寄存器0选择通道P6.2麦克风,P6.5连接拨码电位器（GPIO默认方向是输入方向）
    ADC12MCTL0 |= ADC12INCH_2;
    ADC12MCTL1 |= ADC12INCH_5;

    //ADC转换使能
    ADC12CTL0 |= ADC12ENC;
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
static volatile unsigned int ADon = 0;
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    initClock();
    ioinit();
    AD_Init();

    _enable_interrupt();

    ADC12CTL0 |= ADC12SC;                         //开始采样转换

    volatile unsigned int value[2] = {0,0}, g[7]={0,2005,2100,2130,2180,2220,2250};
    //g[1]~g[6]为声音采样值阶梯，存为数组
    for(;;)
    {
        if (ADon == 0) {                            //若开关标记为关，则不进入采样模块
            _delay_cycles(50000);
            continue;
        }
        value[0] = Filter(), value[1] = ADC12MEM1;  //value[0]记录声音滤波采样,value[1]记录拨码电位器采样
        _delay_cycles(20000);                       //时延控制
        //完成LED1~LED6灯的亮灭判断，设置value的有效范围
        if ((value[0] >= g[1] || value[1] > 1500) && ADon)  {P8OUT |= BIT1;} else {P8OUT &= ~BIT1;}
        if (value[0] >= g[2] && ADon) {P3OUT |= BIT7;} else {P3OUT &= ~BIT7;}
        if (value[0] >= g[3] && ADon) {P7OUT |= BIT4;} else {P7OUT &= ~BIT4;}
        if (value[0] >= g[4] && ADon) {P6OUT |= BIT3;} else {P6OUT &= ~BIT3;}
        if (value[0] >= g[5] && ADon) {P6OUT |= BIT4;} else {P6OUT &= ~BIT4;}
        if (value[0] >= g[6] && ADon) {P3OUT |= BIT5;} else {P3OUT &= ~BIT5;}

    }
}

#pragma vector=PORT1_VECTOR //中断向量
__interrupt void Port_1(void) //中断函数
{
    if (P1IFG & BIT2) {
        _delay_cycles(50);
        if (P1IFG & BIT2) {
            ADon ^= 1;
            light_off();
        }
    }
    P1IFG &= ~BIT2; //清除中断标记
}


