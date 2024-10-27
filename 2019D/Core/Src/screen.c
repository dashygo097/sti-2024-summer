/*
 * screen.c
 *
 *  Created on: Jul 24, 2024
 *      Author: dashygo
 */


#include "usart.h"
#include "main.h"
#include "screen.h"


void itostr(int num,char str[] )
{
	int sign = num,i = 0,j = 0;
	char temp[33];
	if(sign<0)
	{
		num = -num;
	}
	for(;num>0;)
	{
        temp[i] = num%10+'0';
		num/=10;
		i++;
	};


	if(sign<0)
	{
		temp[i++] = '-';
	}
    temp[i++]='0';
	temp[i] = '\0';
	i--;
	while(i>=0)
	{
	str[j] = temp[i];
	j++;
	i--;
	}

	str[j] = '\0';
}


void Screen_Show(double Rin , double Rout , double Av , double wave[])
{
	//int num = 250;
//	char *str1;
//    char *str2;
//    char *str3;
//    char *str4;

    USART1PutString("x0.val=");
    itostr(Rin*100,str_2);
    USART1PutString(str_2);
    USART1PutString("\xff\xff\xff");

    USART1PutString("x1.val=");
    itostr(Rout*100,str_2);
    USART1PutString(str_2);
    USART1PutString("\xff\xff\xff");

    USART1PutString("x2.val=");
    itostr(Av*100,str_2);
    USART1PutString(str_2);
    USART1PutString("\xff\xff\xff");
//    for(int i=0;i<num;i++)
//   {
//       USART1PutString("add 8,0,");
//       itostr(wave[i]*100,str_2);
//       USART1PutString(str_2);
//       USART1PutString("\xff\xff\xff");
//   }
}

