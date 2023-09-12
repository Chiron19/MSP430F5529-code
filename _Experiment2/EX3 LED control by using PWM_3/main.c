#include <msp430.h> 
#include "MacroAndConst.h"

static volatile unsigned int systemOn = 1,  //系统开关标记
                             LEDcycle = 0,  //计数周期变量
                             LEDon = 0,     //LED开关
                             LEDPeriod = 0, //LED点亮周期
                             volume = 0,    //声音采样值
                             lightValue = 4095,     //光强采样值
                             lightNum = 0,          //排灯点亮个数
                             lightCmp[7]={0,700,600,500,400,300,200},//光强比较值
                             target = 0xFFFE;       //目标周期变量

#define XT2_FREQ   4000000
#define MCLK_FREQ 16000000
#define SMCLK_FREQ 4000000
void ClockInit()
{
	 UCSCTL6 &= ~XT1OFF;    //启动XT1
	 P5SEL |= BIT2 + BIT3;  //XT2引脚功能选择
	 UCSCTL6 &= ~XT2OFF;    //打开XT2
	 __bis_SR_register(SCG0);
	 UCSCTL0 = DCO0+DCO1+DCO2+DCO3+DCO4;
	 UCSCTL1 = DCORSEL_4;   //DCO频率范围在28.2MHZ以下
	 UCSCTL2 = FLLD_5 + 1;  //D=16，N=1
	 UCSCTL3 = SELREF_5 + FLLREFDIV_3;      //n=8,FLLREFCLK时钟源为XT2CLK；DCOCLK=D*(N+1)*(FLLREFCLK/n);DCOCLKDIV=(N+1)*(FLLREFCLK/n);
	 UCSCTL4 = SELA_4 + SELS_3 +SELM_3;     //ACLK的时钟源为DCOCLKDIV,MCLK\SMCLK的时钟源为DCOCLK
	 UCSCTL5 = DIVA_5 +DIVS_1;              //ACLK由DCOCLKDIV的32分频得到，SMCLK由DCOCLK的2分频得到
	 //最终MCLK:16MHZ,SMCLK:8MHZ,ACLK:32KHZ
//	 __bic_SR_register(SCG0);                   //Enable the FLL control loop
}
int Filter(int Reg)//中值滤波法
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
void light_on()//排灯全亮
{
    P8OUT |= BIT1;
    P3OUT |= BIT7;
    P7OUT |= BIT4;
    P6OUT |= BIT3;
    P6OUT |= BIT4;
    P3OUT |= BIT5;
}
void light_off()//排灯全灭
{
    P8OUT &= ~BIT1;
    P3OUT &= ~BIT7;
    P7OUT &= ~BIT4;
    P6OUT &= ~BIT3;
    P6OUT &= ~BIT4;
    P3OUT &= ~BIT5;
}
void light_ignite()//排灯按照光强点亮
{
    //  LED1~LED6灯的亮灭判断
    if (lightValue <= lightCmp[1] && systemOn) {P8OUT |= BIT1; lightNum = 1;} else {P8OUT &= ~BIT1;}
    if (lightValue <= lightCmp[2] && systemOn) {P3OUT |= BIT7; lightNum = 2;} else {P3OUT &= ~BIT7;}
    if (lightValue <= lightCmp[3] && systemOn) {P7OUT |= BIT4; lightNum = 3;} else {P7OUT &= ~BIT4;}
    if (lightValue <= lightCmp[4] && systemOn) {P6OUT |= BIT3; lightNum = 4;} else {P6OUT &= ~BIT3;}
    if (lightValue <= lightCmp[5] && systemOn) {P6OUT |= BIT4; lightNum = 5;} else {P6OUT &= ~BIT4;}
    if (lightValue <= lightCmp[6] && systemOn) {P3OUT |= BIT5; lightNum = 6;} else {P3OUT &= ~BIT5;}
}
void LEDstart(void)//LED开灯，渐亮
{
    P1OUT |= BIT5; //打开P1.5
    for (; (LEDPeriod < 0xFF00) && systemOn; LEDPeriod += 0x00FF) {
        TA2CCR2 = LEDPeriod;
        __delay_cycles(MCLK_FREQ*0.005);
    }
}
void LEDend(void)//LED熄灯，渐暗
{
    for (; LEDPeriod >= 0x01FF; LEDPeriod -= 0x00FF) {
        TA2CCR2 = LEDPeriod;
        __delay_cycles(MCLK_FREQ*0.01);
    }
    P1OUT &= ~BIT5; //要把休眠开关P1.5关掉，否则LED仍有光
    LEDon = LEDcycle = 0;
}
void LEDadjust()//LED亮度调节
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
   IO端口初始化
    说明： 口袋板LED灯端口输出，依次是L1-->P8.1; L2-->P3.7; L3--> P7.4; L4-->P6.3; L5-->P6.4; L6-->P3.5
                 机械按钮输入，依次是S1-->P1.2; S2-->P1.3;
***********************************************************************************/
void InitIO()
{
                    //LED1~LED6的初始化
    P8DIR |= BIT1;  //p8.1 output
    P3DIR |= BIT7;  //p3.7 output
    P7DIR |= BIT4;  //p7.4 output
    P6DIR |= BIT3;  //p6.3 output
    P6DIR |= BIT4;  //p6.4 output
    P3DIR |= BIT5;  //p3.5 output

                    //麦克风对应IO初始化
    P6DIR |= BIT2;
    P6SEL |= BIT2;
                    //光敏电阻对应IO初始化
    P6DIR |= BIT1;
    P6SEL |= BIT1;

    P1DIR &= ~BIT2;
    P1REN |= BIT2;  // 使能P1.2上下拉电阻功能
    P1OUT |= BIT2;  // 置P1.2为上拉电阻方式
    P1IE  |= BIT2;  //enable P1.2 interrupt
    P1IES |= BIT2;  //high-low transition

                    //LED灯控制
	P1DIR |= BIT5;
	P1OUT |= BIT5;

	P2SEL |= BIT5; //设置P2.5由TA2.2控制
	P2DIR |= BIT5; //设置P1.5和P2.5为输出模式

	                //点亮熄灭测试LED
	systemOn = 1;
	light_on();
    LEDstart();
    light_off();
    LEDend();
    systemOn = 0;
}
void AD_Init()
{// 在ADC12ENC==0时（默认）,初始化各寄存器，后打开ADC转换使能（ADC12ENC==1）
    ADC12CTL0 |= ADC12MSC;//多路采样转换（首次需要SHI信号上升沿触发采样定时器）自动循环采样转换
    ADC12CTL0 |= ADC12ON; //启动（打开）ADC12模块

    ADC12CTL1 |= ADC12CONSEQ_3;//选择序列通道多次循环采样转换
    ADC12CTL1 |= ADC12CSTARTADD_0; //开始地址使用寄存器0
    ADC12CTL1 |= ADC12SHP;//采样保持模式选择，选择采样信号（SAMPCON）的来源是采样定时器

    //存储寄存器0选择通道P6.2麦克风
    ADC12MCTL0 |= ADC12INCH_2;
    //存储寄存器1选择通道P6.1光敏电阻
    ADC12MCTL1 |= ADC12INCH_0 + ADC12EOS;//End of sequence回到寄存器0

    //ADC转换使能
    ADC12CTL0 |= ADC12ENC;
}
void TimerInit(void)//TimerA初始化
{
    TA2CTL = TASSEL__SMCLK + TACLR + MC_1;
    TA2CCR0 = 0xFFFF;
    TA2CCTL2 = OUTMOD_7;                            //复位-翻转
    
    TA0CTL |= TASSEL__ACLK + TACLR + MC_1 + TAIE;   //时钟为ACLK(32768Hz),增计数模式，开始时清零计数器
    TA0CCTL0 =  CCIE;                               //比较器中断使能
    TA0CCR0  = 32768;                               //周期1秒进一次中断

    __enable_interrupt();
}


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
                            //初始化
    ClockInit();
    TimerInit();
    AD_Init();
    InitIO();

    __no_operation();       // For debugger
    __delay_cycles(MCLK_FREQ*2);

    ADC12CTL0 |= ADC12SC;   //开始采样

    for(;;)
    {
       if (systemOn == 0) {
           __delay_cycles(10000);
           continue;
       }

       volume = Filter(0);      //音量，经过滤波
       lightValue = Filter(1);  //光强，经过滤波
       if (volume >= 3000 && systemOn) {
           if (!LEDon) LEDstart();
           LEDon = 1;
           LEDcycle = 0;        //重新开始计时
       }
       if (!LEDon) light_ignite();  //LED未触发时排灯根据光强点亮
       else LEDadjust();            //根据光强调整LED亮度
    }
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    if (P1IFG & BIT2) {
        __delay_cycles(100); //时延防止敏感
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
            P1IFG &= ~BIT2; //清除中断标记
        }
    }
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
      if (LEDon != 0) {
          LEDcycle++;
          if(LEDcycle >= 30) LEDend();//调整关灯延时的计数值
      }
      TA0CTL &= ~TAIFG; //清除中断标记
      TA0CTL |= TACLR;  //清除计数器
}
