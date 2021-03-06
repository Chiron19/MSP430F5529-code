#include <msp430.h>
#define XT2_FREQ    4000000
#define MCLK_FREQ   16000000
#define SMCLK_FREQ  4000000
#define ACLK_FREQ   32000
#define musicNum 5
/**
 *
 *共阳接法；按列扫描
 */
//测试代码
//2.0 2.2 /7.4/ 3.0 3.1 2.6 /2.3/ /8.1/  2.5 2.4 4.3
//1.3 1.2 2.3 2.6  8.7 3.7 7.4 6.3 6.4 3.5 6.2 3.6 1.4 1.5 1.6 6.0
//8.7 3.7 7.4 6.3 6.4 3.5
//行设置低电平 ,row  阴极低电压导通
#define r11 P6DIR|=BIT5;P6OUT&=~BIT5;
#define r21 P3DIR|=BIT4;P3OUT&=~BIT4;
#define r31 P3DIR|=BIT3;P3OUT&=~BIT3;
#define r41 P1DIR|=BIT6;P1OUT&=~BIT6;
#define r51 P6DIR|=BIT6;P6OUT&=~BIT6;
#define r61 P3DIR|=BIT2;P3OUT&=~BIT2;
#define r71 P2DIR|=BIT7;P2OUT&=~BIT7;
#define r81 P4DIR|=BIT2;P4OUT&=~BIT2;
//行设置高电平,row   阴极高低电压不导通
#define r10 P6DIR|=BIT5;P6OUT|=BIT5;
#define r20 P3DIR|=BIT4;P3OUT|=BIT4;
#define r30 P3DIR|=BIT3;P3OUT|=BIT3;
#define r40 P1DIR|=BIT6;P1OUT|=BIT6;
#define r50 P6DIR|=BIT6;P6OUT|=BIT6;
#define r60 P3DIR|=BIT2;P3OUT|=BIT2;
#define r70 P2DIR|=BIT7;P2OUT|=BIT7;
#define r80 P4DIR|=BIT2;P4OUT|=BIT2;
//列设置高电平,line
#define l8 P2DIR|=BIT5;P2OUT|=BIT5;P2OUT&=~BIT4;P1OUT&=~BIT5;P1OUT&=~BIT4;P1OUT&=~BIT3;P1OUT&=~BIT2;P4OUT&=~BIT3;P4OUT&=~BIT0;
#define l7 P2DIR|=BIT4;P2OUT|=BIT4;P2OUT&=~BIT5;P1OUT&=~BIT5;P1OUT&=~BIT4;P1OUT&=~BIT3;P1OUT&=~BIT2;P4OUT&=~BIT3;P4OUT&=~BIT0;
#define l6 P1DIR|=BIT5;P1OUT|=BIT5;P2OUT&=~BIT5;P2OUT&=~BIT4;P1OUT&=~BIT4;P1OUT&=~BIT3;P1OUT&=~BIT2;P4OUT&=~BIT3;P4OUT&=~BIT0;
#define l5 P1DIR|=BIT4;P1OUT|=BIT4;P2OUT&=~BIT5;P2OUT&=~BIT4;P1OUT&=~BIT5;P1OUT&=~BIT3;P1OUT&=~BIT2;P4OUT&=~BIT3;P4OUT&=~BIT0;
#define l4 P1DIR|=BIT3;P1OUT|=BIT3;P2OUT&=~BIT5;P2OUT&=~BIT4;P1OUT&=~BIT5;P1OUT&=~BIT4;P1OUT&=~BIT2;P4OUT&=~BIT3;P4OUT&=~BIT0;
#define l3 P1DIR|=BIT2;P1OUT|=BIT2;P2OUT&=~BIT5;P2OUT&=~BIT4;P1OUT&=~BIT5;P1OUT&=~BIT4;P1OUT&=~BIT3;P4OUT&=~BIT3;P4OUT&=~BIT0;
#define l2 P4DIR|=BIT3;P4OUT|=BIT3;P2OUT&=~BIT5;P2OUT&=~BIT4;P1OUT&=~BIT5;P1OUT&=~BIT4;P1OUT&=~BIT3;P1OUT&=~BIT2;P4OUT&=~BIT0;
#define l1 P4DIR|=BIT0;P4OUT|=BIT0;P2OUT&=~BIT5;P2OUT&=~BIT4;P1OUT&=~BIT5;P1OUT&=~BIT4;P1OUT&=~BIT3;P1OUT&=~BIT2;P4OUT&=~BIT3;

//时钟部分
int timer=0;
int pause=0;
int playNum=0;
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

void TimerInit(void)
{
    TA0CTL |= TASSEL_1 + ID_0 + MC_1;         //选择ACLK，无分频，增计数模式
    TA0CCR0 = 8000;
    _EINT();   //开启全局中断
    TA0CCTL0 |= CCIE;//允许中断
}

void IOinit(void)
{
    P8DIR|=BIT1;
    P8OUT|=BIT1; //L1闪烁显示中断周期
    P3DIR|=BIT7;
    P7DIR|=BIT4;
    P6DIR|=BIT3;

    P2DIR &=~BIT0; //Input
    P2REN |= BIT0;
    P2OUT |= BIT0;
    P2IE  |= BIT0; //enable P2.0 interrupt
    P2IES |= BIT0; //high-low transition

    P2DIR &=~BIT2;
    P2REN |= BIT2;
    P2OUT |= BIT2;
    P2IE  |= BIT2; //enable P2.0 interrupt
    P2IES |= BIT2; //high-low transition

    P2DIR &=~BIT6;
    P2REN |= BIT6;
    P2OUT |= BIT6;
    P2IE  |= BIT6; //enable P2.0 interrupt
    P2IES |= BIT6; //high-low transition
}
void set(unsigned int m){
    if(m&0b10000000){r11;}else{r10;}
    if(m&0b1000000){r21;}else{r10;}
    if(m&0b100000){r31;}else{r30;}
    if(m&0b10000){r41;}else{r40;}
    if(m&0b1000){r51;}else{r50;}
    if(m&0b100){r61;}else{r60;}
    if(m&0b10){r71;}else{r70;}
    if(m&1){r81;}else{r80;}
}
void reset(){
    r10;r20;r30;r40;r50;r60;r70;r80;
}
//延时函数
void delay()
{
    int one=1000,two=1000;
    for(;two>-1;two--)
        for(   ;one>-1;one--);
}
/*数组第一位表示有效数字
 * 第二位表示模式   1为滚动显示，2为正放逐帧显示，3为倒放逐帧显示
 */
unsigned int dat197[199]={197,1,0x00,0x82,0x82,0xFE,0x82,0x82,0x00,0x00,0x1C,0x3E,0x7E,0xFC,0xFC,0x7E,0x3E,0x1C,
 0x00,0x00,0xFE,0x10,0x10,0x10,0xFE,0x00,0x00,0x82,0x82,0xFE,0x82,0x82,0x00,0x00,
 0x02,0x02,0xFE,0x02,0x02,0x00,0x00,0x8C,0x92,0x92,0x92,0x62,0x00,0x00,0xC2,0xA2,
 0x92,0x8A,0x86,0x00,0x00,0x00,0x00,0x00,0xA8,0x7E,0xA8,0x00,0xDE,0x32,0xDE,0x00,//I love hitsz
 0x00,0x14,0x0C,0xFF,0x14,0x24,0xFB,0xAA,0xE6,0x00,0x00,0x80,0x7B,0x0D,0x0F,0x09,
 0x0F,0x0D,0x0B,0x00,0x00,0x14,0x0C,0xFF,0x14,0x04,0x24,0xFB,0xAA,0xE6,0x00,0x00,
 0x44,0x64,0x3C,0x94,0x48,0x3F,0x08,0xF8,0x00,0x00,0x80,0x90,0x54,0x3F,0x54,0x90,
 0x80,0x00,0x00,0x89,0xAD,0xFB,0xA9,0x8D,0x38,0x00,0xFF,0x00,0x00,0x06,0x02,0xAA,
 0x5B,0xBB,0xEA,0x52,0x46,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                     //规格严格 功夫到家
                                                             0x3C,0x24,0x3C,0x08,//哈工大之song
 0xEC,0xAB,0xEC,0x08,0x00,0x00,0x80,0x82,0x82,0xFE,0x82,0x82,0x80,0x00,0x00,0x88,
 0x88,0x48,0x3E,0x48,0x88,0x88,0x00,0x00,0x44,0x44,0x65,0x96,0x8C,0x84,0x80,0x00,
 0x00,0x0C,0x92,0x92,0x92,0x60,0x00,0x7C,0x82,0x82,0x7C,0x00,0xFE,0x08,0x10,0xFE,
 0x00,0x7C,0x82,0x92,0x70};

unsigned int dat112[114]={112,1,0x00,0x82,0x82,0xFE,0x82,0x82,0x00,0x00,0xF8,0x2E,0xF8,0x00,0xFE,0x08,0x30,0x08,//I AM MAKE IN CHINA
 0xFE,0x00,0x00,0xFE,0x08,0x30,0x08,0xFE,0x00,0xF8,0x2E,0xF8,0x00,0xFE,0x82,0x7C,
 0x00,0xFE,0x92,0x92,0x00,0x00,0x82,0xFE,0x82,0x00,0xFE,0x08,0x10,0xFE,0x00,0x00,
 0x7C,0x82,0x82,0x82,0x00,0xFE,0x10,0x10,0xFE,0x00,0x82,0xFE,0x82,0x00,0xFE,0x08,
 0x10,0xFE,0x00,0xF8,0x16,0xF8,0x00,0x00,0x00,0x00,0x00,0x48,0x49,0xFF,0x29,0x08,//我中国人
 0xBF,0x48,0xAA,0x1E,0x12,0x12,0xFF,0x12,0x12,0x1E,0x00,0x00,0xFF,0x81,0xD5,0xFD,
 0xD5,0xA1,0xFF,0x00,0x00,0x80,0x40,0x30,0x1F,0x30,0x40,0x80,0x80,0x00,0x00,0x00};

//流水心
unsigned int dat272[274]={272,3,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x00,0x00,
                         0x00,0x00,0x02,0x04,0x04,0x00,0x00,0x00,0x00,0x02,0x02,0x04,0x04,0x00,0x00,0x00,
                         0x04,0x02,0x02,0x04,0x04,0x00,0x00,0x00,0x0C,0x02,0x02,0x04,0x04,0x00,0x00,0x00,
                         0x1C,0x02,0x02,0x04,0x04,0x00,0x00,0x00,0x1C,0x22,0x02,0x04,0x04,0x00,0x00,0x00,
                         0x1C,0x22,0x42,0x04,0x04,0x00,0x00,0x00,0x1C,0x22,0x42,0x84,0x04,0x00,0x00,0x00,
                         0x1C,0x22,0x42,0x84,0x84,0x00,0x00,0x00,0x1C,0x22,0x42,0x84,0x84,0x40,0x00,0x00,
                         0x1C,0x22,0x42,0x84,0x84,0x40,0x20,0x00,0x1C,0x22,0x42,0x84,0x84,0x40,0x20,0x10,
                         0x1C,0x22,0x42,0x84,0x84,0x40,0x20,0x18,0x1C,0x22,0x42,0x84,0x84,0x40,0x20,0x1C,
                         0x1C,0x22,0x42,0x84,0x84,0x40,0x22,0x1C,0x1C,0x22,0x42,0x84,0x84,0x42,0x22,0x1C,
                         0x1C,0x22,0x42,0x84,0x84,0x42,0x22,0x1C,0x00,0x1C,0x3C,0x78,0x78,0x3C,0x1C,0x00,
                         0x1C,0x3E,0x7E,0xFC,0xFC,0x7E,0x3E,0x1C,0x1C,0x3E,0x7E,0xFC,0xFC,0x7E,0x3E,0x1C,
                         0x00,0x1C,0x3C,0x78,0x78,0x3C,0x1C,0x00,0x1C,0x3E,0x7E,0xFC,0xFC,0x7E,0x3E,0x1C,
                         0x00,0x1C,0x3C,0x78,0x78,0x3C,0x1C,0x00,0x1C,0x3E,0x7E,0xFC,0xFC,0x7E,0x3E,0x1C,
                         0x00,0x1C,0x3C,0x78,0x78,0x3C,0x1C,0x00,0x1C,0x3E,0x7E,0xFC,0xFC,0x7E,0x3E,0x1C,
                         0x00,0x1C,0x3C,0x78,0x78,0x3C,0x1C,0x00,0x1C,0x3E,0x7E,0xFC,0xFC,0x7E,0x3E,0x1C,
                         0x00,0x1C,0x3C,0x78,0x78,0x3C,0x1C,0x00,0x1C,0x3E,0x7E,0xFC,0xFC,0x7E,0x3E,0x1C,
                         0x00,0x1C,0x3C,0x78,0x78,0x3C,0x1C,0x00,0x1C,0x3E,0x7E,0xFC,0xFC,0x7E,0x3E,0x1C
                       };
//倒数，
unsigned int dat320[322]={320,2,0x00,0x00,0x4C,0x92,0x92,0x92,0x7C,0x00,0x00,0x00,0x4C,0x92,0x92,0x92,0x7C,0x00,0x00,0x00,0x4C,0x92,0x92,0x92,0x7C,0x00,0x00,0x00,0x4C,0x92,0x92,0x92,0x7C,0x00,
                         0x00,0x00,0x6C,0x92,0x92,0x92,0x6C,0x00,0x00,0x00,0x6C,0x92,0x92,0x92,0x6C,0x00,0x00,0x00,0x6C,0x92,0x92,0x92,0x6C,0x00,0x00,0x00,0x6C,0x92,0x92,0x92,0x6C,0x00,
                         0x00,0x00,0x02,0x02,0x02,0xF2,0x0E,0x00,0x00,0x00,0x02,0x02,0x02,0xF2,0x0E,0x00,0x00,0x00,0x02,0x02,0x02,0xF2,0x0E,0x00,0x00,0x00,0x02,0x02,0x02,0xF2,0x0E,0x00,
                         0x00,0x00,0x7C,0x92,0x92,0x92,0x64,0x00,0x00,0x00,0x7C,0x92,0x92,0x92,0x64,0x00,0x00,0x00,0x7C,0x92,0x92,0x92,0x64,0x00,0x00,0x00,0x7C,0x92,0x92,0x92,0x64,0x00,
                         0x00,0x00,0x4E,0x8A,0x8A,0x8A,0x72,0x00,0x00,0x00,0x4E,0x8A,0x8A,0x8A,0x72,0x00,0x00,0x00,0x4E,0x8A,0x8A,0x8A,0x72,0x00,0x00,0x00,0x4E,0x8A,0x8A,0x8A,0x72,0x00,
                         0x00,0x00,0x30,0x28,0x24,0xFE,0x20,0x00,0x00,0x00,0x30,0x28,0x24,0xFE,0x20,0x00,0x00,0x00,0x30,0x28,0x24,0xFE,0x20,0x00,0x00,0x00,0x30,0x28,0x24,0xFE,0x20,0x00,
                         0x00,0x00,0x44,0x92,0x92,0x92,0x6C,0x00,0x00,0x00,0x44,0x92,0x92,0x92,0x6C,0x00,0x00,0x00,0x44,0x92,0x92,0x92,0x6C,0x00,0x00,0x00,0x44,0x92,0x92,0x92,0x6C,0x00,
                         0x00,0x00,0xE4,0xA2,0xA2,0xA2,0x9C,0x00,0x00,0x00,0xE4,0xA2,0xA2,0xA2,0x9C,0x00,0x00,0x00,0xE4,0xA2,0xA2,0xA2,0x9C,0x00,0x00,0x00,0xE4,0xA2,0xA2,0xA2,0x9C,0x00,
                         0x00,0x00,0x84,0x84,0xFE,0xFE,0x80,0x80,0x00,0x00,0x84,0x84,0xFE,0xFE,0x80,0x80,0x00,0x00,0x84,0x84,0xFE,0xFE,0x80,0x80,0x00,0x00,0x84,0x84,0xFE,0xFE,0x80,0x80,
                         0x00,0x00,0x7C,0x82,0x82,0x82,0x7C,0x00,0x00,0x00,0x7C,0x82,0x82,0x82,0x7C,0x00,0x00,0x00,0x7C,0x82,0x82,0x82,0x7C,0x00,0x00,0x00,0x7C,0x82,0x82,0x82,0x7C,0x00};

//fengche
unsigned int dat248[250]={248,3,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,
 0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,
 0x00,0x00,0x3C,0x3C,0x3C,0x3C,0x00,0x00,0x01,0x00,0x3C,0x34,0x2C,0x3C,0x00,0x80,
0x01,0x00,0x3C,0x34,0x2C,0x3C,0x00,0x80,0x00,0x00,0x3C,0x3C,0x3C,0x3C,0x00,0x00,
0x00,0x00,0x3C,0x3C,0x3C,0x3C,0x00,0x00,0x80,0x00,0x3C,0x2C,0x34,0x3C,0x00,0x01,
0x80,0x00,0x3C,0x2C,0x34,0x3C,0x00,0x01,0x00,0x00,0x3C,0x3C,0x3C,0x3C,0x00,0x00,
0x81,0x00,0x3C,0x24,0x24,0x3C,0x00,0x81,0x81,0x00,0x3C,0x24,0x24,0x3C,0x00,0x81,
0x24,0x00,0x99,0x24,0x24,0x99,0x00,0x24,0x24,0x00,0x99,0x24,0x24,0x99,0x00,0x24,
0x04,0x04,0xD8,0x24,0x24,0x1B,0x20,0x20,0x04,0x04,0xD8,0x24,0x24,0x1B,0x20,0x20,
0x04,0x04,0xF4,0x04,0x20,0x2F,0x20,0x20,0x04,0x04,0xF4,0x04,0x20,0x2F,0x20,0x20,
0x04,0x04,0xE4,0x18,0x18,0x27,0x20,0x20,0x04,0x04,0xE4,0x18,0x18,0x27,0x20,0x20,
0x08,0x04,0x64,0x98,0x19,0x26,0x20,0x10,
0x10,0x08,0x24,0x59,0x9A,0x24,0x10,0x08,
0x00,0x18,0x24,0x5A,0x5A,0x24,0x18,0x00,
0x08,0x10,0x24,0x9A,0x59,0x24,0x08,0x10,
0x10,0x20,0x26,0x19,0x98,0x64,0x04,0x08,
0x20,0x20,0x27,0x18,0x18,0xE4,0x04,0x04,
0x40,0x21,0x26,0x18,0x18,0x64,0x84,0x02,
0x81,0x42,0x24,0x18,0x18,0x24,0x42,0x81,
0x02,0xC2,0x24,0x18,0x18,0x24,0x43,0x40};//





//滚动显示
void play1(unsigned int dat[400]){
    int i=2,row=0;
    int n=dat[0]-6;
    while(i<n){
           timer=0;
           for(row=7;row>-1;row--)
           {
               int j=row+i;
               if(row==7) {l8;set(dat[j]);}
               if(row==6) {l7;set(dat[j]);}
               if(row==5) {l6;set(dat[j]);}
               if(row==4) {l5;set(dat[j]);}
               if(row==3) {l4;set(dat[j]);}
               if(row==2) {l3;set(dat[j]);}
               if(row==1) {l2;set(dat[j]);}
               if(row==0) {l1;set(dat[j]);
               if(timer!=1){row=8;}}
               reset();
               delay();
           }
           i++;
           reset();

       }
}
//逐帧显示，正放
void play2(unsigned int dat[400]){
    int row=7,i=2;
    int n=dat[0];
    while(i<n){
           timer=0;
           for(row=7;row>-1;row--)
           {
               int j=row+i;
               if(row==7) {l8;set(dat[j]);}
               if(row==6) {l7;set(dat[j]);}
               if(row==5) {l6;set(dat[j]);}
               if(row==4) {l5;set(dat[j]);}
               if(row==3) {l4;set(dat[j]);}
               if(row==2) {l3;set(dat[j]);}
               if(row==1) {l2;set(dat[j]);}
               if(row==0) {l1;set(dat[j]);
               if(timer!=1){row=8;}}
               reset();
               delay();
           }

           i=i+8;
           reset();
    }
}
//逐帧显示,倒放
void play3(unsigned int dat[400]){
   int n=dat[0];
   int row=7,i=n-6;
          while(i>1){
                 timer=0;
                 for(row=7;row>-1;row--)
                 {
                     int j=i+row;
                     if(row==7) {l8;set(dat[j]);}
                     if(row==6) {l7;set(dat[j]);}
                     if(row==5) {l6;set(dat[j]);}
                     if(row==4) {l5;set(dat[j]);}
                     if(row==3) {l4;set(dat[j]);}
                     if(row==2) {l3;set(dat[j]);}
                     if(row==1) {l2;set(dat[j]);}
                     if(row==0) {l1;set(dat[j]);
                     if(timer!=1){row=8;}}
                     reset();
                     delay();
                 }

                 i=i-8;
                 reset();
          }
}

void rank(unsigned int dat[400]){
    int m=dat[1];
    if(m==1){ play1(dat);}//滚动显示
    if(m==2){ play2(dat);}//逐帧显示
    if(m==3){ play2(dat);
    play3(dat);}//逐帧显示
}

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;                                           //关看门狗
    TimerInit();
    initClock();
    IOinit();
    playNum = 1;
    while(1)
    {
//        if (pause == 1) __delay_cycles(100);
//        else {
            switch(playNum) {
            case 0: rank(dat112); break;
            case 1: rank(dat197); break;
            case 2: rank(dat272); break;
            case 3: rank(dat320); break;
            case 4: rank(dat248); break;
            default: rank(dat197); break;
            }
            playNum++;
//        }
    }
}
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    if (P2IFG & BIT0) {
        P3OUT ^= BIT7;
        P2IFG &= ~BIT0;
        pause ^= 1;
    }
    if (P2IFG & BIT2) {
        P7OUT ^= BIT4;
        P2IFG &= ~BIT2;
        playNum++;
        if (playNum >= musicNum) playNum = 0;
    }
    if (P2IFG & BIT6) {
        P6OUT ^= BIT3;
        P2IFG &= ~BIT6;
        if (playNum == 0) playNum = musicNum-1;
        else playNum--;
    }
}
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A0 (void)     //定时器A0中断服务程序
{
   if(timer==0) timer=1;
   P8OUT^=BIT1;
   TA0CCTL0 &= ~CCIFG;//复位中断标识符
}
