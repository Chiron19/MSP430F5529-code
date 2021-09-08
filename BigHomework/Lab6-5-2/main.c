#include <msp430.h>
#include "ad.h"
#include "dac7571.h"
#include "dr_sdcard.h"
#include "dr_sdcard/integer.h"
#include "dr_sdcard/ff.h"
#include <string.h>
#include <stdio.h>
#include <string.h>
#include "dr_sdcard/HAL_SDCard.h"
#include "task.h"
#include "Paper_Display.h"
#include "echo_HC-SR04.h"

//typedef unsigned int    uint16_t;
UINT br;
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
	P5SEL |= BIT4+BIT5;                       					//ѡ�� XT1
    UCSCTL6 &= ~(XT1OFF);                     					//XT1 ����
    UCSCTL6 |= XCAP_3;                        					//Internal load cap
    P5SEL |= BIT2 + BIT3; 										//XT2���Ź���ѡ��
    UCSCTL6 &= ~XT2OFF; 										//����XT2
    while (SFRIFG1 & OFIFG)
    { 															//�ȴ�XT1��XT2��DCO�ȶ�
        UCSCTL7 &= ~(DCOFFG+XT1LFOFFG+XT2OFFG);
        SFRIFG1 &= ~OFIFG;
    }

    UCSCTL4 = SELA__XT1CLK + SELS__XT2CLK + SELM__XT2CLK; 		//����DCO�������ܷ�

    UCSCTL1 = DCORSEL_5; 										//6000kHz~23.7MHz
    UCSCTL2 = 20000000 / (4000000 / 8); 						//XT2Ƶ�ʽϸߣ���Ƶ����Ϊ��׼�ɻ�ø��ߵľ���
    UCSCTL3 = SELREF__XT2CLK + FLLREFDIV__8; 					//XT2����16��Ƶ����Ϊ��׼
    while (SFRIFG1 & OFIFG)
    { 															//�ȴ�XT1��XT2��DCO�ȶ�
        UCSCTL7 &= ~(DCOFFG+XT1LFOFFG+XT2OFFG);
        SFRIFG1 &= ~OFIFG;
    }
    UCSCTL5 = DIVA__1 + DIVS__1 + DIVM__1; 						//�趨����CLK�ķ�Ƶ
    UCSCTL4 = SELA__XT1CLK + SELS__DCOCLK + SELM__DCOCLK; 		//�趨����CLK��ʱ��Դ
}

#define BUFFER_SIZE 2048
#define Buf 3
#define Speed 150
#define checkNum 1
#define lightCmp 2000   //���⴫�бȽ�ֵ
uint8_t buffer[BUFFER_SIZE];
volatile unsigned int checkCnt = 0;

FRESULT rc;                                       					/* Result code */
FATFS fatfs;                                      					/* File system object */
DIRS dir;                                         					/* Directory object */
FILINFO fno;                                      					/* File information object */
FIL fil;                                          					/* File object */

#define FBUF_SIZE 256
char fbuf[FBUF_SIZE];

void strupr(char* str)
{
  char *ptr = str;
  while(!*ptr)
  {
    if(*ptr >= 'a' && *ptr <='z')
      *ptr -= 'a' - 'A';
    ptr++;
  }
}

int __low_level_init(void)
{
  WDTCTL = WDTPW + WDTHOLD ;//stop wdt
  return 1;
}



int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;												//�ؿ��Ź�

    __disable_interrupt();
    initClock();
//    PaperIO_Int();
//    INIT_SPD2701();
//    display(0,1);
  unsigned char res,clear;
  unsigned int count;
  unsigned int MusicData;
  FILINFO finfo;			 											/*�ļ���Ϣ*/
  DIRS dirss;
  char path[100] = {""};
  char FileName[100][13];
  char *result3;
  unsigned char PlayNum=0;
  unsigned char MusicNum;   //������Ŀ
  volatile unsigned int value = 0;										//�����жϱ���
  volatile unsigned int lightValue = 0;                                 //���⴫�в���ֵ



  IOInit();

    HC_SR04Init();          //��ʼ������ģ��HC_SR04

    initLED();
    igniteLED();


  AD_Init();															//AD��ʼ��
  SDCard_init();														//SD��ʼ��
  TimeInit();															//��ʱ����ʼ��
  TimeGetInit();          //��ʼ�������������ţ�ģʽ��

  f_mount(0, &fatfs);													//�ļ�ϵͳ����
  if (f_opendir(&dirss,path) == FR_OK)									//��Ŀ¼
  {
  	PlayNum=0;
	while (f_readdir(&dirss, &finfo) == FR_OK)    						/* ���ζ�ȡ�ļ��� */
	{
		if ( finfo.fattrib & AM_ARC )       							/* �ж��Ƿ�Ϊ�浵���ĵ� */
		{
				if( !finfo.fname[0] )	 								/* �ļ���Ϊ�ռ�������Ŀ¼��ĩβ���˳� */
				break;
				result3 = strstr( finfo.fname, ".WAV" );
				if(result3!=NULL)
				{
					memcpy(&FileName[PlayNum][0],&finfo.fname,sizeof(finfo.fname));
					PlayNum++;

				}

		}
	}
	MusicNum=PlayNum;
	PlayNum = 0;
	resetLED();
	__enable_interrupt();
	while(1)
	{
		memcpy(&finfo.fname,&FileName[PlayNum][0],13);			//��ȡ�ļ�����
		res=f_open(&fil,finfo.fname,FA_OPEN_EXISTING|FA_READ);
		br = 1;
		clear = 0;
		do {
            lightValue = Filter(); //���⴫�в���
            __delay_cycles(10000);
            if (KeyFlag.S1 == 1 || KeyFlag.S2 == 1 || KeyFlag.S3 == 1) break;
        } while (lightValue < lightCmp);
		while(1)
	    {
		  res = f_read(&fil,buffer,(sizeof(buffer)),&br);
		  if(br == 0)
			  break;
		  while(count < 2048)
		  {
            if(KeyFlag.S1 == 0)
            {
//                __disable_interrupt();
                MusicData=buffer[count];
                MusicData<<=4;
                value = ADC12MEM0;
                delay1(Speed);                                             //11052
                if(value<=10)
                {
                    DACValue(MusicData*0);
                }
                else if(value<=400)
                {
                    DACValue(MusicData>>5);
                }
                else if(value<=1000)
                {
                    DACValue(MusicData>>4);
                }
                else if(value<=1500)
                {
                    DACValue(MusicData>>3);
                }
                else if(value<=2000)
                {
                    DACValue(MusicData>>2);
                }
                else if(value<=3000)
                {
                    DACValue(MusicData>>1);
                }
                else
                {
                    DACValue(MusicData);
                }
                count++;
//                __enable_interrupt();
            }
		  }

//		  DACValue(0);
		  if(KeyFlag.S2==1)
		  {
		  	KeyFlag.S2=0;
			break;
		  }
		  if(KeyFlag.S3==1)
		  {
			break;
		  }
		  count=0;
	  	}
		f_close(&fil);
	    if(KeyFlag.S3==1)
	    {
			KeyFlag.S3=0;
			if(PlayNum>=1)
				PlayNum-=1;
			else
				PlayNum=MusicNum-1;
	    }
	    else
		PlayNum++;
		if(PlayNum>=MusicNum)
			PlayNum=0;
	}
  }
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
    ScanKey();
    HC_SR04Start();
    signalJudge();
    signalRes();
    TA0CCTL0 &= ~CCIFG;//��λ�жϱ�ʶ��
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void)
{
    __disable_interrupt();                        //���ж�
    static unsigned int L=sampleNum/3;
    static unsigned int R=sampleNum*2/3;
    unsigned int j;
    unsigned int t;

    if (~(TA0CCTL3 & CCIFG)) {
        if(times_TACCR3 == 1)
        {
            LastCCR3 = TA0CCR3;       //��¼���ϴ�CCR3��ֵ
            TA0CCTL3 &= ~CM_1;        //�������ز���
            TA0CCTL3 |= CM_2;         //��Ϊ�½��ز���
            times_TACCR3++;
        }
        if(times_TACCR3 == 0)
        {
            if(i3 < sampleNum)                //�Ѳɼ�����10�ε�ֵ���ŵ�������
            {
                dat1[i3] = TA0CCR3 - LastCCR3;
                i3++;
            }
            if(i3 >= sampleNum)               //Ϊ�˷�ֹ������Ѳɼ�������ͷ��β��ֵȥ����ȡ�м��ƽ��ֵ
            {
                for(i3=0;i3<sampleNum;i3++)
                {
                    for(j=i3+1;j<sampleNum;j++)
                    {
                        if(dat1[i3]>dat1[j])
                        {
                            t=dat1[j];
                            dat1[j]=dat1[i3];
                            dat1[i3]=t;
                        }
                    }
                }
                t = 0;
                for(i3=L;i3<R;i3++)  t += dat1[i3];
                valueCCR3 = t/(R-L);
                i3 = 0;
            }
            TA0CCTL3 &= ~CM_2;      //����½��ش���
            TA0CCTL3 |= CM_1;       //��Ϊ�����ش���
            times_TACCR3++;                //�ı�times_TACCR3��ֵ
        }
        times_TACCR3 &= 0x01;                //times_TACCR3>1ʱ��0
        TA0CCTL3 &= ~CCIFG;         //����ж���Ӧ��־
    }
    if (~(TA0CCTL4 & CCIFG)) {
        if(times_TACCR4 == 1)
        {
            LastCCR4 = TA0CCR4;       //��¼���ϴ�CCR4��ֵ
            TA0CCTL4 &= ~CM_1;        //�������ز���
            TA0CCTL4 |= CM_2;         //��Ϊ�½��ز���
            times_TACCR4++;
        }
        if(times_TACCR4 == 0)
        {
            if(i4 < sampleNum)                //�Ѳɼ�����20�ε�ֵ���ŵ�������
            {
                dat2[i4] = TA0CCR4 - LastCCR4;
                i4++;
            }
            if(i4 >= sampleNum)               //Ϊ�˷�ֹ������Ѳɼ�������ͷ��β��ֵȥ����ȡ�м��ƽ��ֵ
            {
                for(i4=0;i4<sampleNum;i4++)
                {
                    for(j=i4+1;j<sampleNum;j++)
                    {
                        if(dat2[i4]>dat2[j])
                        {
                            t=dat2[j];
                            dat2[j]=dat2[i4];
                            dat2[i4]=t;
                        }
                    }
                }
                t = 0;
                for(i4=L;i4<R;i4++)  t += dat2[i4];
                valueCCR4 = t/(R-L);
                i4 = 0;
            }
            TA0CCTL4 &= ~CM_2;      //����½��ش���
            TA0CCTL4 |= CM_1;       //��Ϊ�����ش���
            times_TACCR4++;         //�ı�times_TACCR4��ֵ
        }
        times_TACCR4 &= 0x01;       //times_TACCR4>1ʱ��0
        TA0CCTL4 &= ~CCIFG;         //����ж���Ӧ��־
    }
    LPM0_EXIT;                  //�˳��͹���ģʽ
    __enable_interrupt();       //���ж�
}

