/*
 * ad9833.c
 *
 *  Created on: Jul 20, 2024
 *      Author: dashygo
 */


#include "ad9833.h"


//***************************
//		Pin assign
//		STM32			AD9833
//		PF12 		---> FSYNC
//		PF1 		---> SCK
//		PG6 		---> DAT
//		PC9			---> CS
//***************************


	#define PORT_FSYNC	GPIOB
	#define PIN_FSYNC	GPIO_PIN_15  //PC7

	#define PORT_SCK	GPIOB
	#define PIN_SCK		GPIO_PIN_14  //PC8

	#define PORT_DAT	GPIOB
	#define PIN_DAT		GPIO_PIN_13  //PC9

	#define PORT_CS		GPIOB
	#define PIN_CS		GPIO_PIN_12  //PA8

//****************************************************************

	#define FSY0_0()		HAL_GPIO_WritePin(DDS_FSY0_GPIO_Port, DDS_FSY0_Pin,GPIO_PIN_RESET)
	#define FSY0_1()		HAL_GPIO_WritePin(DDS_FSY0_GPIO_Port, DDS_FSY0_Pin,GPIO_PIN_SET)

	#define SCK_0()		HAL_GPIO_WritePin(DDS_CLK_GPIO_Port, DDS_CLK_Pin,GPIO_PIN_RESET)
	#define SCK_1()		HAL_GPIO_WritePin(DDS_CLK_GPIO_Port, DDS_CLK_Pin,GPIO_PIN_SET)

	#define DAT_0()		HAL_GPIO_WritePin(DDS_DAT_GPIO_Port, DDS_DAT_Pin,GPIO_PIN_RESET)
	#define DAT_1()		HAL_GPIO_WritePin(DDS_DAT_GPIO_Port, DDS_DAT_Pin,GPIO_PIN_SET)

	#define FSY1_0()		HAL_GPIO_WritePin(DDS_FSY1_GPIO_Port, DDS_FSY1_Pin,GPIO_PIN_RESET)
	#define FSY1_1()		HAL_GPIO_WritePin(DDS_FSY1_GPIO_Port, DDS_FSY1_Pin,GPIO_PIN_SET)



//AD9833 GPIO
//
//void AD9833_Init_GPIO()
//{
//    GPIO_InitTypeDef GPIO_InitStructure;
//
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
//
//	GPIO_InitStructure.GPIO_Pin = PIN_FSYNC|PIN_SCK|PIN_DAT|PIN_CS;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_Init(PORT_SCK, &GPIO_InitStructure);
//


//}




void AD9833_Delay(void)
{
	uint16_t i;
	for (i = 0; i < 1; i++);
}



void AD9833_Write(unsigned int TxData, unsigned char channel)
{
	unsigned char i;

	SCK_1();
	//AD9833_Delay();
	if (channel) {
		FSY1_1();
		//AD9833_Delay();
		FSY1_0();
		//AD9833_Delay();
	}
	else {
		FSY0_1();
		//AD9833_Delay();
		FSY0_0();
		//AD9833_Delay();
	}
	for(i = 0; i < 16; i++)
	{
		if (TxData & 0x8000)//
			DAT_1();
		else
			DAT_0();

		AD9833_Delay();
		SCK_0();
		AD9833_Delay();
		SCK_1();

		TxData <<= 1;
	}
	FSY0_1();

}




void AD9833_WaveSetting(double Freq,unsigned int Freq_SFR,unsigned int WaveMode,unsigned int Phase, unsigned char channel )
{
		int frequence_LSB,frequence_MSB,Phs_data;
		double   frequence_mid,frequence_DATA;
		long int frequence_hex;

		frequence_mid=268435456/25;
		frequence_DATA=Freq;
		frequence_DATA=frequence_DATA/1000000;
		frequence_DATA=frequence_DATA*frequence_mid;
		frequence_hex=frequence_DATA;
		frequence_LSB=frequence_hex;
		frequence_LSB=frequence_LSB&0x3fff;
		frequence_MSB=frequence_hex>>14;
		frequence_MSB=frequence_MSB&0x3fff;

		Phs_data=Phase|0xC000;
		AD9833_Write(0x0100, channel);
		AD9833_Write(0x2100, channel);

		if(Freq_SFR==0)
		{
		 	frequence_LSB=frequence_LSB|0x4000;
		 	frequence_MSB=frequence_MSB|0x4000;

			AD9833_Write(frequence_LSB, channel);
			AD9833_Write(frequence_MSB, channel);
			AD9833_Write(Phs_data, channel);
			AD9833_Write(0x2000 , channel);
	    }
		if(Freq_SFR==1)
		{
			 frequence_LSB=frequence_LSB|0x8000;
			 frequence_MSB=frequence_MSB|0x8000;
			AD9833_Write(frequence_LSB, channel);
			AD9833_Write(frequence_MSB, channel);
			AD9833_Write(Phs_data, channel);
			AD9833_Write(0x2800 , channel);
		}

		if(WaveMode==TRI_WAVE)
		 	AD9833_Write(0x2002, channel);
		if(WaveMode==SQU_WAVE)
			AD9833_Write(0x2028, channel);
		if(WaveMode==SIN_WAVE)
			AD9833_Write(0x2000, channel);

}





void AD9833_WaveSetting_Double(double Freq,unsigned int Freq_SFR,unsigned int WaveMode,unsigned int Phase)
{

		int frequence_LSB,frequence_MSB,Phs_data;
		double   frequence_mid,frequence_DATA;
		long int frequence_hex;

		frequence_mid=268435456/25;
		frequence_DATA=Freq;
		frequence_DATA=frequence_DATA/1000000;
		frequence_DATA=frequence_DATA*frequence_mid;
		frequence_hex=frequence_DATA;
		frequence_LSB=frequence_hex;
		frequence_LSB=frequence_LSB&0x3fff;
		frequence_MSB=frequence_hex>>14;
		frequence_MSB=frequence_MSB&0x3fff;

		Phs_data=Phase|0xC000;
		AD9833_Write(0x0100, 0);
		AD9833_Write(0x2100, 0);
		AD9833_Write(0x0100, 1);
		AD9833_Write(0x2100, 1);

		if(Freq_SFR==0)
		{
		 	frequence_LSB=frequence_LSB|0x4000;
		 	frequence_MSB=frequence_MSB|0x4000;

			AD9833_Write(frequence_LSB, 0);
			AD9833_Write(frequence_MSB, 0);
			AD9833_Write(Phs_data, 1);
			AD9833_Write(frequence_LSB, 1);
			AD9833_Write(frequence_MSB, 1);
			//AD9833_Write(0x2000);
	    }
		if(Freq_SFR==1)
		{
			 frequence_LSB=frequence_LSB|0x8000;
			 frequence_MSB=frequence_MSB|0x8000;
			AD9833_Write(frequence_LSB, 0);
			AD9833_Write(frequence_MSB, 0);
			AD9833_Write(Phs_data, 1);
			AD9833_Write(frequence_LSB, 1);
			AD9833_Write(frequence_MSB, 1);
			//AD9833_Write(0x2800);
		}

		if(WaveMode==TRI_WAVE)
		{
			AD9833_Write(0x2002, 0);
			AD9833_Write(0x2002, 1);
		}
		if(WaveMode==SQU_WAVE)
		{
			AD9833_Write(0x2028, 0);
			AD9833_Write(0x2028, 1);
		}
		if(WaveMode==SIN_WAVE)
		{
			AD9833_Write(0x2000, 0);
			AD9833_Write(0x2000, 1);
		}

}

