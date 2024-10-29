/*
 * oled.c
 *
 *  Created on: May 23, 2024
 *      Author: dashygo
 */

/* ################### Manuals ###################
 *
 * This is oled_i2c_bsp for the first pairwork  for STI practice
 *
 * OK ,  to use oled ,please firstly enable I2C1
 * (i.e. stm32h7的第一个I2C,别问为什么不让这个bsp兼容各种i2cX ，问就是懒)
 *
 * then connect oled screen with the stm32h7,and we are done
 * (it is very simple,isn't it?  )
 *
 * ################### NOTICE ####################
 *
 * 请务必将oled.c和font.c加入Scr中,并将其头文件(oled.h和font.h)加入Inc中!!!
 * 这是Cmake的文件类型存放标准)
 *
 * To use OLED please type
 *
 * Oled_Init();
 *
 *firstly and then you can use oled to write ascii patterns !
 */



#include "oled.h"
#include "font.h"
#include <string.h>

int pages = 0;
uint8_t GRAM[8][128];
uint8_t *ADptr = GRAM[0];//ADDRESS ptr

void Oled_Write_Cmd(uint8_t cmd){

	HAL_I2C_Mem_Write(&hi2c1, OLED_ADDRESS, 0x00, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 0xff);
}

void Oled_Write_Data(uint8_t data)//Write Shift
{
    HAL_I2C_Mem_Write(&hi2c1, OLED_ADDRESS, 0x40, I2C_MEMADD_SIZE_8BIT,&data, 1, 0xff);
    ADptr += CharLen;
}

void Oled_Init()//Initialize Oled
{
	Oled_RstFrame();
	Oled_Write_Cmd(0xAE);//--display off
	Oled_Write_Cmd(0x00);//---set low column address
	Oled_Write_Cmd(0x10);//---set high column address
	Oled_Write_Cmd(0x40);//--set start line address
	Oled_Write_Cmd(0xB0);//--set page address
	Oled_Write_Cmd(0x81); // contract control
	Oled_Write_Cmd(0xFF);//--128
	Oled_Write_Cmd(0xA1);//set segment remap
	Oled_Write_Cmd(0xA6);//--normal / reverse
	Oled_Write_Cmd(0xA8);//--set multiplex ratio(1 to 64)
	Oled_Write_Cmd(0x3F);//--1/32 duty
	Oled_Write_Cmd(0xC8);//Com scan direction
	Oled_Write_Cmd(0xD3);//-set display offset
	Oled_Write_Cmd(0x00);//
	Oled_Write_Cmd(0xD5);//set osc division
	Oled_Write_Cmd(0x80);//
	Oled_Write_Cmd(0xD8);//set area color mode off
	Oled_Write_Cmd(0x05);//
	Oled_Write_Cmd(0xD9);//Set Pre-Charge Period
	Oled_Write_Cmd(0xF1);//
	Oled_Write_Cmd(0xDA);//set com pin configuartion
	Oled_Write_Cmd(0x12);//
	Oled_Write_Cmd(0xDB);//set Vcomh
	Oled_Write_Cmd(0x30);//
	Oled_Write_Cmd(0x8D);//set charge pump enable
	Oled_Write_Cmd(0x14);//
	Oled_Write_Cmd(0xAF);//--turn on oled panel
	Oled_ShowFrame();
	pages = 0;
}

/* 以上的三个函数并非应用级函数，使用时请使用下面的函数 :) */
/* Frame level functions
 *
 * ################## NOTICE ###################
 *
 * FRAME-LEVEL FUNCTIONS ARE STILL UNDER DEVELOPMENT.
 * SO THEY MIGHT NOT WORK
 * UNLESS I HAVE TIME  :L
 *
 */

void Oled_RstFrame(void )//Reset Frame
{

	memset(GRAM , 0 , sizeof(GRAM));
}

void Oled_ShowFrame(void )//Show Frame
{
	uint8_t Buffer[129];
	Buffer[0] = 0x40;
	for (int i = 0 ; i < 8; i++){
		memcpy(Buffer + 1 , GRAM[i] , 128);
		Oled_Write_Cmd(0xB0 + i);
		Oled_Write_Cmd(0x00);
		Oled_Write_Cmd(0x10);
		HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDRESS, Buffer , sizeof(Buffer), HAL_MAX_DELAY);
	}
}


/* 这里的Frame可以简单理解为屏幕的shadow, 0->dark 1->light */

/* I sometimes use Eng because CubeIDE does not support Chi input ......
 * while all the Chinese is to express them more clearly (wink        */

/* 以下是较实用的函数 */

/* these functions are used to Write ascii patterns with stacks beginning from bottom */

void Oled_Screen_Clear(void)//Clear the screen with Frame to zero
{
	for (int i = 0 ; i < 8 ; i++){
		Oled_Write_Cmd(0xB0 + i);
		Oled_Write_Cmd(0x00);
		Oled_Write_Cmd(0x10);
		for (int j = 0 ; j < 128 ; j++){
			Oled_Write_Data(0);

		}
	}
	ADptr = GRAM[0];
}

void Oled_WriteChar(uint8_t data)//Receive data from u(s)art or anything else to Register
{
	uint8_t *ascii = ascii_8x6[data];
	for (int i = 0 ; i < 6; i ++ ){
		Oled_Write_Data(ascii[i]);
	}
}

/* e.g Oled_WriteChar(17) Write a "1" */

void Oled_WriteString(char * str)//Write a string
{
	while (*str){
		Oled_WriteChar(*str);
		str++;
	}
}

/* better WriteFunctions below:
 * Oled_WriteAsciiChar() is used to write */

void Oled_WriteAsciiChar(uint8_t data){

	uint8_t delta = (ADptr - GRAM[0])/6;
	//int pages = delta/128 ;
	if (data == ' '){
		Oled_WriteChar(0);
	}
	else if('0' <= data && data <= '9'){
		Oled_WriteChar(data - 32);
	}
	else if ('a' <= data && data <= 'z' ){
		Oled_WriteChar(data - 32);
	}
	else if ('A' <= data && data <= 'Z'){
		Oled_WriteChar(data - 32);
	}
	else if (data == '.'){
		Oled_WriteChar(14);
	}
	else if (data == '\n'){
//		for (int i = delta; i < 128*(delta/128) + 128; i++ ){
//			Oled_Write_Cmd(0xB0 + pages);
//			Oled_Write_Cmd(0x00);
//			Oled_Write_Cmd(0x10);
//			Oled_Write_Data(0);
//		}
		pages++;
		Oled_Write_Cmd(0xB0 + pages);
		Oled_Write_Cmd(0x00);
		Oled_Write_Cmd(0x10);
	}
	else if (data == ',')
	{
		Oled_WriteChar(12);
	}
	else if (data == ':')
	{
		Oled_WriteChar('9' -31);
	}
	else if (data == '-')
	{
		Oled_WriteChar(13);
	}
	else if (data == '%%')
	{
		Oled_WriteChar(5);
	}
	else if (data == '/')
	{
		Oled_WriteChar(15);
	}
	else if (data == '=')
	{
		Oled_WriteChar(29);
	}

}

void Oled_WriteAsciiString(char * str)//Write a string
{
	while (*str){
		Oled_WriteAsciiChar(*str);
		str++;
	}
}
