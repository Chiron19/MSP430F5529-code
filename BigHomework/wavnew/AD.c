/*
 * AD.c
 *
 *  Created on: 2014-7-15
 *      Author: dean
 */

#include "ad.h"

void AD_Init(void)
{
	ADC12CTL0 |= ADC12MSC;									//�Զ�ѭ������ת��
	ADC12CTL0 |= ADC12ON;									//����ADC12ģ��
	ADC12CTL1 |= ADC12CONSEQ_3;								//ѡ��ͨ��ѭ������ת��//ѡ������ͨ�����ѭ������ת��
	ADC12CTL1 |= ADC12CSTARTADD_0;                          //��ʼ��ַʹ�üĴ���0
	ADC12CTL1 |= ADC12SHP;									//��������ģʽ
	ADC12MCTL0 |= ADC12INCH_5; 								////�洢�Ĵ���0ѡ��ͨ��P6.5�����Ӳ����λ��
	                                                        //�洢�Ĵ���1ѡ��ͨ��P6.0���⴫��������
    ADC12MCTL1 |= ADC12INCH_0 + ADC12EOS;//End of sequence�ص��Ĵ���0
	ADC12CTL0 |= ADC12ENC;                                  //ADCת��ʹ��
	ADC12CTL0 |= ADC12SC;
}
