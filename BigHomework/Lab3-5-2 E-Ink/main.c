#include <msp430.h>
#include "Paper_Display.h"

#define TimesNewRoman 0
#define Arial 1
#define ArialBlack 2
#define size8 0
#define size16 1
#define sizeQ 0
#define fontQ 0
volatile unsigned char DisBuffer[250*16];
void initClock()
{
	 UCSCTL6 &= ~XT1OFF; //启动XT1
	 P5SEL |= BIT2 + BIT3; //XT2引脚功能选择
	 UCSCTL6 &= ~XT2OFF;          //打开XT2
	 __bis_SR_register(SCG0);
	 UCSCTL0 = DCO0+DCO1+DCO2+DCO3+DCO4;
	 UCSCTL1 = DCORSEL_4;       //DCO频率范围在28.2MHZ以下
	 UCSCTL2 = FLLD_5 + 1;       //D=16，N=1
	 UCSCTL3 = SELREF_5 + FLLREFDIV_3;    //n=8,FLLREFCLK时钟源为XT2CLK；DCOCLK=D*(N+1)*(FLLREFCLK/n);DCOCLKDIV=(N+1)*(FLLREFCLK/n);
	 UCSCTL4 = SELA_4 + SELS_3 +SELM_3;    //ACLK的时钟源为DCOCLKDIV,MCLK\SMCLK的时钟源为DCOCLK
	 UCSCTL5 = DIVA_5 +DIVS_1;      //ACLK由DCOCLKDIV的32分频得到，SMCLK由DCOCLK的2分频得到
	             //最终MCLK:16MHZ,SMCLK:8MHZ,ACLK:32KHZ

//	 __bic_SR_register(SCG0);                   //Enable the FLL control loop

}
volatile unsigned int selc = 0;
void statusChange(void){
    volatile unsigned int x=1;
    if (selc > 6) selc = 0;
    P8OUT &= ~BIT1;
    P3OUT &= ~BIT7;
    P7OUT &= ~BIT4;
    P6OUT &= ~BIT3;
    P6OUT &= ~BIT4;
    P3OUT &= ~BIT5;
    switch (selc) {
        case 1:
            display("Chapter 1   AAAAAA", 12, 10,fontQ,sizeQ,1,0);
            display("Chapter 2   BBBBBB", 12, 40,fontQ,sizeQ,0,0);
            display("Chapter 3   CCCCCC", 12, 70,fontQ,sizeQ,0,0);
            display("Chapter 4   DDDDDD", 12, 100,fontQ,sizeQ,0,0);
            P8OUT |= BIT1;
            break;
        case 2:
            display("Chapter 1   AAAAAA", 12, 10,fontQ,sizeQ,0,0);
            display("Chapter 2   BBBBBB", 12, 40,fontQ,sizeQ,1,0);
            display("Chapter 3   CCCCCC", 12, 70,fontQ,sizeQ,0,0);
            display("Chapter 4   DDDDDD", 12, 100,fontQ,sizeQ,0,0);
            P3OUT |= BIT7;
            break;
        case 3:
            display("Chapter 1   AAAAAA", 12, 10,fontQ,sizeQ,0,0);
            display("Chapter 2   BBBBBB", 12, 40,fontQ,sizeQ,0,0);
            display("Chapter 3   CCCCCC", 12, 70,fontQ,sizeQ,1,0);
            display("Chapter 4   DDDDDD", 12, 100,fontQ,sizeQ,0,0);
            P7OUT |= BIT4;
            break;
        case 4:
            display("Chapter 1   AAAAAA", 12, 10,fontQ,sizeQ,0,0);
            display("Chapter 2   BBBBBB", 12, 40,fontQ,sizeQ,0,0);
            display("Chapter 3   CCCCCC", 12, 70,fontQ,sizeQ,0,0);
            display("Chapter 4   DDDDDD", 12, 100,fontQ,sizeQ,1,0);
            P6OUT |= BIT3;
            break;
        case 5:
            x = 4;
            P6OUT |= BIT4;
//            DIS_IMG(x);
            break;
        case 6:
            x = 5;
            P3OUT |= BIT5;
//            DIS_IMG(x);
            break;
        default:
            display("Chapter 1   AAAAAA", 12, 10,fontQ,sizeQ,0,0);
            display("Chapter 2   BBBBBB", 12, 40,fontQ,sizeQ,0,0);
            display("Chapter 3   CCCCCC", 12, 70,fontQ,sizeQ,0,0);
            display("Chapter 4   DDDDDD", 12, 100,fontQ,sizeQ,0,0);
//            DIS_IMG(x);
            break;
    }
    DIS_IMG(x);
    selc ++;
}
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    P8DIR |= BIT1;
    P8OUT &=~ BIT1;

    P3DIR |= BIT7;  //p3.7 output
    P3OUT &= ~BIT7;

    P7DIR |= BIT4;  //p7.4 output
    P7OUT &= ~BIT4;

    P6DIR |= BIT3;
    P6OUT &=~ BIT3;

    P6DIR |= BIT4;
    P6OUT &=~ BIT4;

    P3DIR |= BIT5;
    P3OUT &=~ BIT5;

    P1DIR &= ~BIT2;  //p1.2 input,pull up
    P1REN |= BIT2;
    P1OUT |= BIT2;
    P1IE  |= BIT2; //enable P1.2 interrupt
    P1IES |= BIT2; //high-low transition

    initClock();
    PaperIO_Int();
    INIT_SSD1673();
    Init_buff();
    DIS_IMG(1);
    _enable_interrupt();
//    display("                        ", 42, 0,TimesNewRoman,size8,1,0);
//    display(" Huatsing Instruments   ", 42, 16,TimesNewRoman,size8,1,1);//列/行
//    display("012abcABC" ,80, 0,TimesNewRoman,size16,0,0);
//    display("012abcABC", 0,50,Arial,size8,1,0);
//    display("012abcABC",80,50,Arial,size16,1,0);
//    display("012abcABC", 0,100,ArialBlack,size8,1,0);
//    display("012abcABC",80,90,ArialBlack,size16,1,0);

//    DIS_IMG(1);

//    DIS_IMG(4);
//    DIS_IMG(3);
    while(1)
    {
        _NOP();
    }

}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    if (P1IFG & BIT2) {
        statusChange();
        P1IFG &= ~BIT2;
    }
}
