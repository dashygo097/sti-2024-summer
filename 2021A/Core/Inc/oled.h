/*
 * oled.h
 *
 *  Created on: May 23, 2024
 *      Author: dashygo
 */

#ifndef INC_OLED_H_
#define INC_OLED_H_

#include "i2c.h"
#include "font.h"
#include "main.h"

#define OLED_ADDRESS 0x78


void Oled_Init();
void Oled_Write_Data(uint8_t data);
void Oled_Write_Cmd(uint8_t cmd);
void Oled_RstFrame(void );
void Oled_ShowFrame(void );
void Oled_Screen_Clear(void);
void Oled_WriteChar(uint8_t data);
void Oled_WriteAsciiChar(uint8_t data);
void Oled_WriteAsciiString(char * str);
void Oled_WriteString(char * str);

#endif /* INC_OLED_H_ */
