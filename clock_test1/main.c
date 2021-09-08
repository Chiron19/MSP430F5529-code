#include <msp430.h>

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // 关闭看门狗
    P1DIR |= BIT0; // ACLK  通过 P1.0输出 输出
    P1SEL |= BIT0; //  选择IO 端口功能为 XT1
    P5SEL |= BIT4+BIT5;

    UCSCTL6 &= ~(XT1OFF); //  使能 XT1
    UCSCTL6 |= XCAP_3; //  配置内接电容值，选择典型电容值XCAP_3

    do
    { //  清零XT1 、DCO故障标志位 故障标志位
        UCSCTL7 &= ~( XT1LFOFFG + DCOFFG);
        UCSCTL7 &= ~( XT2OFFG);
        SFRIFG1 &= ~OFIFG; //  清零SFR中的故障标志位 中的故障标志位
    } while (SFRIFG1 & OFIFG); // 检测振荡器故障标志位

    UCSCTL4 |= SELA_0; //  选择 ACLK的时钟源 的时钟源
    UCSCTL4 |= SELS__XT2CLK; //  选择 ACLK的时钟源 的时钟源; //  选择 ACLK的时钟源 的时钟源
    while(1)
    {
        ; //  循环等待,空操作 空操作
    }
}
