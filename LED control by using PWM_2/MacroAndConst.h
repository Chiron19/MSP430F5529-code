/*
 * Macro.h
 *
 *  Created on: 2018��1��24��
 *      Author: peter.liu
*     [ʹ��˵��]                                              *
*           1���ÿ��ļ����ڻ������ļ���                    *
 */
/********************
*   ϵ ͳ �� �� ��  *
********************/



#ifndef MACROANDCONST_H_
#define MACROANDCONST_H_



# define xtal  8
/*------------------*
*   �� �� �� �� ��  *
*------------------*/
# define True    0x01
# define TRUE    0x01
# define False   0x00
# define FALSE   0x00
# define High    0x01
# define HIGH    0x01
# define Low     0x00
# define LOW     0x00




/*------------------*
*   �� �� �� �� ��  *
*------------------*/



#ifndef NULL
    # define NULL    0
#endif




#define UBOUND(a) (sizeof(a)/sizeof(a[0]))

# define SAFE_CODE_PERFORMANCE(Code)     CLI();Code;SEI();


//
//# define SET(Reg,n)    Reg |= BIT(n);
//# define CLR(Reg,n)    Reg &= ~BIT(n);
//# define set(Reg,n)    Reg |= BIT(n);
//# define clr(Reg,n)    Reg &= ~BIT(n);
//# define NOT(Reg,n)    Reg ^= BIT(n);
# define SET(Reg,n)    Reg |= n
# define CLR(Reg,n)    Reg &= ~n;
# define set(Reg,n)    Reg |= n;
# define clr(Reg,n)    Reg &= ~n;
# define NOT(Reg,n)    Reg ^= n;


#endif /* MACROANDCONST_H_ */
