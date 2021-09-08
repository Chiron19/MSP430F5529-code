#include <msp430.h> 

void Uart_Init(void)                           // 115200 8N1
{
    P4SEL |= BIT4+BIT5;                        // P4.4,5 = USA1 TXD/RXD
    UCA1CTL1 |= UCSWRST;                       // **Put state machine in reset**
    UCA1CTL1 |= UCSSEL__SMCLK ;                // SMCLK
    UCA1BR0 = 34;                              // 4MHz 115200 (see User's Guide)
                                               // UCA1BR0 = 输入的时钟源/串口通信速率 （取整）
    UCA1BR1 = 0;                               // 4MHz 115200
    UCA1MCTL = UCBRS_6;                        // 输入的时钟源/串口通信速率的余数*8
    UCA1CTL1 &= ~UCSWRST;                      // **Initialize USCI state machine**
//    __bis_SR_register(GIE);       // Enter LPM0, interrupts enabled
}

void UCS_Init(void)
{
//    P1DIR |= BIT0;                            // ACLK set out to pins
//    P1SEL |= BIT0;
    P2DIR |= BIT2;                            // SMCLK set out to pins
    P2SEL |= BIT2;
//    P7DIR |= BIT7;                            // MCLK set out to pins
//    P7SEL |= BIT7;

    P5SEL |= BIT2+BIT3;                       // Port select XT2

    UCSCTL6 &= ~XT2OFF;                       // Enable XT2
    UCSCTL3 |= SELREF_2;                      // FLLref = REFO
                                              // Since LFXT1 is not used,
                                              // sourcing FLL with LFXT1 can cause
                                              // XT1OFFG flag to set
    UCSCTL4 |= SELA_2;                        // ACLK=REFO,SMCLK=DCO,MCLK=DCO

    // Loop until XT1,XT2 & DCO stabilizes - in this case loop until XT2 settles
    do
    {
      UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
                                              // Clear XT2,XT1,DCO fault flags
      SFRIFG1 &= ~OFIFG;                      // Clear fault flags
    }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag

    UCSCTL6 &= ~XT2DRIVE0;                    // Decrease XT2 Drive according to
                                              // expected frequency
    UCSCTL4 |= SELS_5 + SELM_5;               // SMCLK=MCLK=XT2
}


#define Trig1(a)  if(a==1) P1OUT |= BIT3; else P1OUT &= ~BIT3

unsigned int cap_new = 0;           // 首次捕捉的ta0r值
unsigned int cap_old = 0;           // 二次捕捉的ta0r值
//char test_num = 10;                 // 测量次数
char cap_N = 0;                     // 溢出次数
char state = 0x00;                  // zhuangtai
long cap_data = 0;                  // 距离值
//unsigned int dat[test_num];         //通过数组存10次测量值

void Hc_sr_Init(void)           // 超声波模块初始化
{
   /*
    *  P1.2 为echo引脚             捕获模式
    *  P1.2 具有端口中断的通用数字I / O ，TA0 CCR1捕获：CCI1A输入，比较：Out1输出  BSL接收输入
    *  P1.3 为Trig          数字i/o模式
    *  P1.3 具有端口中断的通用数字I / O ， TA0 CCR2捕获：CCI2A输入，比较：Out2输出
    */
    P1OUT &= ~( BIT2 + BIT3 );
    P1DIR |=  BIT3;
    P1SEL |=  BIT2;
    /*
     * note：由于选用的是P1.2 根据端口定义可知，使用的是TA0CCR1，捕获输入引脚通过TA0CCTL1寄存器控制，
     *-捕获值存储在TA0CCR1中。
     * TASSEL：SNCLK; 分频：4MHz/8 = 500k；计数模式：0  ==> 0xFFFF;
     * TA0CCTL1：上升沿捕获 ；同步捕捉；捕获模式；CCI1A输入；
     */
    TA0CTL   = TASSEL__SMCLK + ID__8 + MC_2 + TACLR + TAIE;
    TA0CCTL1 = CM_1 + SCS + CAP + CCIE + CCIS_0;
}

void Hc_sr_Open(void)           //生成一个持续10us的高电平
{
    Trig1(1);
    __delay_cycles(40);
    Trig1(0);
}
int main(void) {

    WDT_A_hold(WDT_A_BASE);
//    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    UCS_Init();
    Uart_Init();
    Hc_sr_Init();
    __bis_SR_register(GIE);
    UARTprintf("\nInitialization is complete!");
    while(1){
        Hc_sr_Open();
        __delay_cycles(1000000);
    }
}
#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void)
{
    switch(__even_in_range(TA0IV,14))
    {
      case  0:break;                                 // No interrupt
      case  2:
          state =  TA0CCTL1 >> 14;
          TA0CCTL1 &= ~CCIFG;
          if( TA0CCTL1 & CM_1){
              cap_new = TA0CCR1;
              TA0CCTL1 &= ~CM_1;
              TA0CCTL1 |=  CM_2;
          }else if ( TA0CCTL1 & CM_2){
              cap_old = TA0CCR1;
              cap_data = ( cap_old - cap_new ) * 0.34;
              UARTprintf("\n\ncap_data: %u mm",cap_data);
              TA0CCTL1 &= ~CM_2;
              TA0CCTL1 |=  CM_1;
          }else UARTprintf("\nstate: %x",state);     // 判断CM位捕捉模式
//          UARTprintf("\n yes! ccr1 \nTA0 CCR1: %u  TA0R: %u",TA0CCR1,TA0R);
          break;                          // CCR1 not used
      case  4: break;                          // CCR2 not used
      case  6: break;                          // reserved
      case  8: break;                          // reserved
      case 10: break;                          // reserved
      case 12: break;                          // reserved
      case 14:
          TA0CTL &= ~TAIFG;
          if(cap_old   < cap_new ){
              cap_N += 1;
          }
          break;                          // overflow
      default: break;
    }
}





