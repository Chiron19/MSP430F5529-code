/*
 * Paper_Display.h
 *
 *  Created on: 2021��9��2��
 *      Author: dssy2
 */
#ifndef PAPER_DISPLAY_H_
#define PAPER_DISPLAY_H_


#include <msp430.h>

#define DELAY_TIME  50    // ͼƬ��ʾ��ͣ��ʱ��(��λ:��)
// ����ͼ
#define PIC_BUFFER                  200 // ȫ��
#define PIC_END                     250  // ȫ��
#define PIC_WHITE                   255  // ȫ��
#define PIC_BLACK                   254  // ȫ��
#define PIC_Orientation             253  // ����ͼ
#define PIC_LEFT_BLACK_RIGHT_WHITE  249  // ����Ұ�
#define PIC_UP_BLACK_DOWN_WHITE     248  // �Ϻ��°�

#define nRST_H  P1OUT|=BIT4
#define nRST_L  P1OUT&=~BIT4
#define nCS_H   P3OUT|=BIT2
#define nCS_L   P3OUT&=~BIT2
#define SDA_H   P3OUT|=BIT3
#define SDA_L   P3OUT&=~BIT3
#define SCLK_H   P2OUT|=BIT7
#define SCLK_L   P2OUT&=~BIT7
#define nDC_H   P3OUT|=BIT4
#define nDC_L   P3OUT&=~BIT4
#define nBUSY   P2IN & BIT2

extern volatile unsigned char DisBuffer[250*16];

void SPI4W_WRITECOM(unsigned char INIT_COM);
void SPI4W_WRITEDATA(unsigned char INIT_DATA);
void SPI4W_WRITE(unsigned char INIT_COM,unsigned char INIT_DATA);
void MYRESET(void);
void WRITE_LUT(void);
void INIT_SSD1673(void);
void DIS_IMG(unsigned int num);
void FIRST_PICTURE(void);
void IMAGE_WRITE(unsigned char patt_num);
void DELAY_100nS(unsigned int delaytime);
void DELAY_mS(unsigned int delaytime);
void DELAY_S(unsigned int delaytime);
void DELAY_M(unsigned int delaytime);
void RING(void);
void READBUSY(void);
void closebump(void);
void DisplayString(const char* str, unsigned int sx, unsigned int sy);
void enterdeepsleep(void);
void Init_buff(void);
void SET_EYEONE(void);
void PaperIO_Int(void);
void display(unsigned char *str, //�ַ���
             unsigned int xsize,  //x����λ��
             unsigned int ysize,  //y����λ��
             unsigned int font,   //���� 0,1,2
             unsigned int size,   //�ֺ� 0,1
             unsigned int reverse,//���� 0 ������ʾ�� 1 ����
             unsigned int fresh   //����ˢ��
             );



#endif /* PAPER_DISPLAY_H_ */
