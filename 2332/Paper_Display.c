#include <msp430.h>
#include "Include.h"
#include "Paper_Display.h"

#define DELAY_TIME	50    // ͼƬ??ʾ??ͣ??ʱ??(??λ:??)
// ????ͼ
#define PIC_WHITE                   255  // ȫ??
#define PIC_BLACK                   254  // ȫ??
#define PIC_Orientation             253  // ????ͼ
#define PIC_LEFT_BLACK_RIGHT_WHITE  249  // ?????Ұ?
#define PIC_UP_BLACK_DOWN_WHITE     248  // ?Ϻ??°?

const unsigned char init_data[]={
		0x50,0xAA,0x55,0xAA,0x55,0xAA,0x11,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x0F,0x0F,0x0F,
		0x0F,0x0F,0x0F,0x01,0x00,0x00,0x00,0x00,0x00,

};

void MyRESET()
{
	nRST_L;
	DELAY_mS(10);//1ms
 	nRST_H;
  	DELAY_mS(10);//1ms
}
void DELAY_100nS(unsigned int delaytime)   // 30us
{
	int i,j;
	for(i=0;i<delaytime;i++)
		for(j=0;j<10;j++);
}

void DELAY_mS(unsigned int delaytime)    	// 1ms
{
	int i;
	for(i=0;i<delaytime;i++)
		__delay_cycles(200);
}
void DELAY_S(unsigned int delaytime)     //  1s
{
	int i;
	for(i=0;i<delaytime;i++)
        {
		__delay_cycles(3276);
        }
}
void DELAY_M(unsigned int delaytime)     //  1M
{
	int i;
	for(i=0;i<delaytime;i++)
		DELAY_S(60);
}
void READBUSY()
{
  while(1)
  {	 //=1 BUSY
     if((P2IN & BIT2)==0)
    	 break;
  }
  DELAY_M(2);
}
void FIRST_PICTURE()
{
	SPI4W_WRITECOM(0x21);
 	SPI4W_WRITEDATA(0x83);
	SPI4W_WRITECOM(0x22);
 	SPI4W_WRITEDATA(0xC4);
}
void INIT_SSD1673()
{
    MyRESET();
    READBUSY();
	SPI4W_WRITECOM(0x01);       // Gate Setting
	SPI4W_WRITEDATA(0xF9);    // MUX Gate lines=250-1=249(F9H)
	SPI4W_WRITEDATA(0x00);    // B[2]:GD=0[POR](G0 is the 1st gate output channel)  B[1]:SM=0[POR](left and right gate interlaced)  B[0]:TB=0[POR](scan from G0 to G319)
	SPI4W_WRITECOM(0x3A);       // number of dummy line period   set dummy line for 50Hz frame freq
	SPI4W_WRITEDATA(0x06);    // Set 50Hz   A[6:0]=06h[POR] Number of dummy line period in term of TGate
	SPI4W_WRITECOM(0x3B);       // Gate line width   set gate line for 50Hz frame freq
	SPI4W_WRITEDATA(0x0B);    // A[3:0]=1011(78us)  Line width in us   78us*(250+6)=19968us=19.968ms
	SPI4W_WRITECOM(0x3C);	      // Select border waveform for VBD
	//    SPI4W_WRITEDATA(0x30);    // GS0-->GS0
	//    SPI4W_WRITEDATA(0x31);    // GS0-->GS1
	//    SPI4W_WRITEDATA(0x32);    // GS1-->GS0
	SPI4W_WRITEDATA(0x33);    // GS1-->GS1  ??????һ??ˢ??Border?Ӱ׵???
	//    SPI4W_WRITEDATA(0x43);    // VBD-->VSS
	//    SPI4W_WRITEDATA(0x53);    // VBD-->VSH
	//    SPI4W_WRITEDATA(0x63);    // VBD-->VSL
	//    SPI4W_WRITEDATA(0x73);    // VBD-->HiZ

	SPI4W_WRITECOM(0x11);	      // Data Entry mode
	SPI4W_WRITEDATA(0x01);    // 01 ?CY decrement, X increment
	SPI4W_WRITECOM(0x44);       // set RAM x address start/end, in page 22
	SPI4W_WRITEDATA(0x00);    // RAM x address start at 00h;
	SPI4W_WRITEDATA(0x0f);    // RAM x address end at 0fh(15+1)*8->128    2D13
	SPI4W_WRITECOM(0x45);	      // set RAM y address start/end, in page 22
	SPI4W_WRITEDATA(0xF9);    // RAM y address start at FAh-1;		    2D13
	SPI4W_WRITEDATA(0x00);    // RAM y address end at 00h;		    2D13

	SPI4W_WRITECOM(0x2C);       // Vcom= *(-0.02)+0.01???
	//    SPI4W_WRITEDATA(0x82);    //-2.5V
	//    SPI4W_WRITEDATA(0x69);    //-2V
	SPI4W_WRITEDATA(0x4B);    //-1.4V
	//    SPI4W_WRITEDATA(0x50);    //-1.5V
	//    SPI4W_WRITEDATA(0x37);    //-1V
	//    SPI4W_WRITEDATA(0x1E);    //-0.5V

	WRITE_LUT();
	SPI4W_WRITECOM(0x21);       // Option for Display Update
	SPI4W_WRITEDATA(0x83);    // A[7]=1(Enable bypass)  A[4]=0ȫ??(value will be used as for bypass)
	DIS_IMG(PIC_WHITE);         // ȫ?ڵ?ȫ?????????????ɷ?ֹ???????ֻ?????????

	SPI4W_WRITECOM(0x21);       //
	SPI4W_WRITEDATA(0x03);    // ????ˢ?»ָ???????ǰ??2??ͼ?Ƚ?
	SPI4W_WRITECOM(0x3C);       // Select border waveform for VBD
	SPI4W_WRITEDATA(0x73);    // VBD-->HiZ  ????ˢ??ʱBorder???Ǹ???

}

void WRITE_LUT()
{
	unsigned char i;
	SPI4W_WRITECOM(0x32);//write LUT register
	for(i=0;i<29;i++)
        SPI4W_WRITEDATA(init_data[i]);//write LUT register
}

void Init_buff(void)
{
	int i;
	for(i=0;i<4000;i++)
	{
		DisBuffer[i]=gImage_logo[i];
	}

}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//xx   ͼƬ??ʾ????    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
void DIS_IMG(unsigned char num)
{
  unsigned int row, col;
  unsigned int pcnt;


  SPI4W_WRITECOM(0x4E);
  SPI4W_WRITEDATA(0x00);  // set RAM x address count to 0;
  SPI4W_WRITECOM(0x4F);
  SPI4W_WRITEDATA(0xF9);  // set RAM y address count to 250-1;	2D13
  DELAY_S(5);
  SPI4W_WRITECOM(0x24);
  DELAY_S(5);
  pcnt = 0;                 // ??λ?򱣴???ʾ?ֽ?????
  for(col=0; col<250; col++)   // ?ܹ?250 GATE??  // send 128x250bits ram 2D13
  {
    for(row=0; row<16; row++)  // ?ܹ?128 SOURCE?У?ÿ??????1bit,?? 128/8=16 ?ֽ?
    {
      switch (num)
      {
		case 1:
          SPI4W_WRITEDATA(DisBuffer[pcnt]);
          break;
		case 2:
          SPI4W_WRITEDATA(gImage_logo[pcnt]);
          break;
		case PIC_WHITE:
          SPI4W_WRITEDATA(0xff);
          break;
          default:
          break;
      }
      pcnt++;
    }
  }
  SPI4W_WRITECOM(0x22);
  SPI4W_WRITEDATA(0xC7);    // (Enable Clock Signal, Enable CP) (Display update,Disable CP,Disable Clock Signal)
  //  SPI4W_WRITEDATA(0xF7);    // (Enable Clock Signal, Enable CP, Load Temperature value, Load LUT) (Display update,Disable CP,Disable Clock Signal)
  SPI4W_WRITECOM(0x20);
  DELAY_mS(1);
  READBUSY();
  DELAY_S(DELAY_TIME);
}

void SetpointXY(unsigned int xs,unsigned int xe,unsigned int ys,unsigned int ye)
{
	SPI4W_WRITECOM(0x44);//set RAM x address start/end, in page 36
	SPI4W_WRITEDATA(xs);//RAM x address start at 00h;
	SPI4W_WRITEDATA(xe);//RAM x address end at 11h(17)->72: [because 1F(31)->128 and 12(18)->76]

	SPI4W_WRITECOM(0x45);//set RAM y address start/end, in page 37
    SPI4W_WRITEDATA(ys);//RAM y address start at 00h;
    SPI4W_WRITEDATA(ye);//RAM y address start at ABh(171)->172: [because B3(179)->180]

	SPI4W_WRITECOM(0x4E);//set RAM x address count to 0;
	SPI4W_WRITEDATA(xs);
	SPI4W_WRITECOM(0x4F);//set RAM y address count to 0;
	SPI4W_WRITEDATA(ye);

	SPI4W_WRITECOM(0x24);
}

void enterdeepsleep()
{
  SPI4W_WRITECOM(0x10);
  SPI4W_WRITEDATA(0x01);
}

void SPI4W_WRITECOM(unsigned char INIT_COM)
{
	unsigned char TEMPCOM;
	unsigned char scnt;
	TEMPCOM=INIT_COM;
	nCS_H;
	nCS_L;
	SCLK_L;
	nDC_L;
	for(scnt=0;scnt<8;scnt++)
	{
		if(TEMPCOM&0x80)
			SDA_H;
		else
			SDA_L;
		DELAY_100nS(10);
		SCLK_H;
		DELAY_100nS(10);
		SCLK_L;
		TEMPCOM=TEMPCOM<<1;
		DELAY_100nS(10);
	}
	nCS_H;
}

void SPI4W_WRITEDATA(unsigned char INIT_DATA)
{
	unsigned char TEMPCOM;
	unsigned char scnt;
	TEMPCOM=INIT_DATA;
	nCS_H;
	nCS_L;
	SCLK_L;
	nDC_H;
	for(scnt=0;scnt<8;scnt++)
	{
		if(TEMPCOM&0x80)
			SDA_H;
		else
			SDA_L;
		DELAY_100nS(10);
		SCLK_H;
		DELAY_100nS(10);
		SCLK_L;
		TEMPCOM=TEMPCOM<<1;
		DELAY_100nS(10);
	}
	nCS_H;
}

unsigned char FontSize[]={16,32};
void display(unsigned char *str, //?ַ???
             unsigned int xsize,  //x????λ??
			 unsigned int ysize,  //y????λ??
			 unsigned int font,   //???? 0,1,2
			 unsigned int size,   //?ֺ? 0,1
			 unsigned int reverse,//???? 0 ??????ʾ?? 1 ????
			 unsigned int fresh   //????ˢ??
			 )
{
    unsigned char i,j,k;
	unsigned char *zimoPoint;
	unsigned char zimoW;
	unsigned char zimoH;
	unsigned char moveBytes;  //?ƶ??ֽ???
	unsigned char moveBits;   //?ƶ?λ??
	volatile unsigned char zimoBufferTemp[6];

	zimoW=FontSize[size]/2;	  //x??????
	zimoH=FontSize[size]/8;	  //y???ֽ???

	moveBits=ysize%8;
	if((moveBits)!=0) //???????ֽڴ???
	{
		moveBytes=zimoH+1;
	}
	else  moveBytes=zimoH;

	while(*str)
	{
	   if((font==TimesNewRoman) && (size==size8)) zimoPoint=(unsigned char *)&TimesNewRoman8;
	   else if((font==TimesNewRoman) && (size==size16)) zimoPoint=(unsigned char *)&TimesNewRoman16;
	   zimoPoint=zimoPoint+((*str)-0x20)*zimoW*zimoH;     //ָ???ַ???ģ?Ŀ?ʼ??ַ??
	   for(i=0;i<zimoW;i++)
	   {
			//??????ģһ??????
			for(j=0;j<6;j++) zimoBufferTemp[j]=0;
			if(reverse==0)
			{
			   	for(j=0;j<zimoH;j++)
				{
					zimoBufferTemp[j]=~(*(zimoPoint+i+j*zimoW));
				}
			}
			else
			{
			   	for(j=0;j<zimoH;j++)
				{
					zimoBufferTemp[j]=(*(zimoPoint+i+j*zimoW));
				}
			}
			//??ģһ????????λ????
			if (moveBits==0);
			else
			{
			    for(k=0;k<moveBits;k++)
				{
					for(j=moveBytes;j>0;j--)
					{
						 zimoBufferTemp[j]=zimoBufferTemp[j]>>1;
						 if ((zimoBufferTemp[j-1]&0x01)>0) zimoBufferTemp[j]=zimoBufferTemp[j]+0x80;
					}
					zimoBufferTemp[0]=zimoBufferTemp[0]>>1;
				}
			}
			//DisBuffer????
			if (moveBits==0);
			else
			{
			    k=(0xff>>moveBits);
				zimoBufferTemp[moveBytes-1]=zimoBufferTemp[moveBytes-1] | (DisBuffer[(xsize+i)*16+((ysize/8)+zimoH)] & k);
				k=~k;
				zimoBufferTemp[0]=zimoBufferTemp[0] | (DisBuffer[(xsize+i)*16+(ysize/8)] & k);

			}
			//???? DisBuffer
			for(j=0;j<moveBytes;j++)
			{
			    DisBuffer[(xsize+i)*16+((ysize/8)+j)]=zimoBufferTemp[j];
			}
	   }
	   xsize=xsize+zimoW;
	   str++;
	}

}

void PaperIO_Int(void)
{
	P1DIR |= BIT4;
	P2DIR |= BIT7;
	P2DIR &= ~BIT2;
	P3DIR |= BIT2 + BIT3 + BIT4;

}



