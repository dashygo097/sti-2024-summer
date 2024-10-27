/*
 * judge.c
 *
 *  Created on: Jul 24, 2024
 *      Author: dashygo
 */


#include "judge.h"
#include "usart.h"


int Get_State(void)
{
	int state = No_Error;


	return state;
}




void State_Out(int state)
{

	switch(state)
	{
	case R1_To0 :
		HAL_UART_Transmit(&huart1 ,(uint8_t *)"R1短路", 6 , HAL_MAX_DELAY);
		break;
	case R2_To0 :
		HAL_UART_Transmit(&huart1 ,(uint8_t *)"R2短路", 6 , HAL_MAX_DELAY);
		break;
	case R3_To0 :
		HAL_UART_Transmit(&huart1 ,(uint8_t *)"R3短路", 6 , HAL_MAX_DELAY);
		break;
	case R4_To0 :
		HAL_UART_Transmit(&huart1 ,(uint8_t *)"R4短路", 6 , HAL_MAX_DELAY);
		break;
	case R1_ToInf :
		HAL_UART_Transmit(&huart1 ,(uint8_t *)"R1开路", 6 , HAL_MAX_DELAY);
		break;
	case R2_ToInf :
		HAL_UART_Transmit(&huart1 ,(uint8_t *)"R2开路", 6 , HAL_MAX_DELAY);
		break;
	case R3_ToInf :
		HAL_UART_Transmit(&huart1 ,(uint8_t *)"R3开路", 6 , HAL_MAX_DELAY);
		break;
	case R4_ToInf :
		HAL_UART_Transmit(&huart1 ,(uint8_t *)"R4开路", 6 , HAL_MAX_DELAY);
		break;
	case C1_ToInf :
		HAL_UART_Transmit(&huart1 ,(uint8_t *)"C1开路", 6 , HAL_MAX_DELAY);
		break;
	case C2_ToInf :
		HAL_UART_Transmit(&huart1 ,(uint8_t *)"C2开路", 6 , HAL_MAX_DELAY);
		break;
	case C3_ToInf :
		HAL_UART_Transmit(&huart1 ,(uint8_t *)"C3开路", 6 , HAL_MAX_DELAY);
		break;
	case C1_Mul2 :
		HAL_UART_Transmit(&huart1 ,(uint8_t *)"C1乘2", 6 , HAL_MAX_DELAY);
		break;
	case C2_Mul2 :
		HAL_UART_Transmit(&huart1 ,(uint8_t *)"C2乘2", 6 , HAL_MAX_DELAY);
		break;
	case C3_Mul2 :
		HAL_UART_Transmit(&huart1 ,(uint8_t *)"C3乘2", 6 , HAL_MAX_DELAY);
		break;
	default :
		HAL_UART_Transmit(&huart1 ,(uint8_t *)"正常", 6 , HAL_MAX_DELAY);
	}

}



