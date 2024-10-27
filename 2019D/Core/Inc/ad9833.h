#ifndef _AD9833_H
#define _AD9833_H

#include "gpio.h"
#define TRI_WAVE 	0
#define SIN_WAVE 	1
#define SQU_WAVE 	2

#define DDS_FSY0_GPIO_Port GPIOD
#define DDS_CLK_GPIO_Port GPIOD
#define DDS_DAT_GPIO_Port GPIOD
#define DDS_FSY1_GPIO_Port GPIOD

#define DDS_FSY0_Pin GPIO_PIN_4
#define DDS_CLK_Pin GPIO_PIN_3
#define DDS_DAT_Pin GPIO_PIN_2
#define DDS_FSY1_Pin GPIO_PIN_1



//void AD9833_WaveSeting(double frequence,unsigned int frequence_SFR,unsigned int WaveMode,unsigned int Phase );
//void AD9833_Init_GPIO(void);
//void AD9833_AmpSet(unsigned char amp);

void AD9833_Delay(void);
void AD9833_Write(unsigned int TxData, unsigned char channel);
void AD9833_WaveSetting(double frequence,unsigned int frequence_SFR,unsigned int WaveMode,unsigned int Phase, unsigned char channel );
void AD9833_WaveSetting_Double(double Freq,unsigned int Freq_SFR,unsigned int WaveMode,unsigned int Phase);

#endif
;

