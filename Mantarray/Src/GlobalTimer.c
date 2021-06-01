#include "GlobalTimer.h"
#include "main.h"
#include <stdio.h>
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim21;
extern TIM_HandleTypeDef htim22;


void GlobalTimerInit(GlobalTimer_t *thisGlobalTimer)
{
	//Start global timer and initialize struct
	//HAL_TIM_Base_Start_IT(&htim6);
	//HAL_TIM_Base_Start_IT(&htim21);
	//HAL_TIM_Base_Start_IT(&htim22);
	thisGlobalTimer->overflowCounter = 0;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim6)
	{
		//TODO set up tim2's clock frequency
	///my_sys.GlobalTimer.overflowCounter++;
		//TEST CODE
		//my_sys.GlobalTimerTest.uartBufLen = sprintf(my_sys.GlobalTimerTest.uartBuffer, "Reached Max \r\n");
		//serialSend(&huart2, my_sys.GlobalTimerTest.uartBuffer, my_sys.GlobalTimerTest.uartBufLen);
	}
	if (htim == &htim21)
	{
		//readMMC5983_XYZ(&my_sys.Magnetometer, &my_sys.Magnetometer.sensorB_MMC5983);
		/*(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_14) == GPIO_PIN_SET) ?
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_RESET) :
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_SET);*/
	}
	if (htim == &htim22)
	{
		//uint8_t test = register_read_MMC5983(&my_sys.Magnetometer.sensorB_MMC5983, MMC5983_INTERNALCONTROL0);
		//test = register_read_MMC5983(&my_sys.Magnetometer.sensorB_MMC5983, MMC5983_INTERNALCONTROL1);
		//test = register_read_MMC5983(&my_sys.Magnetometer.sensorB_MMC5983, MMC5983_INTERNALCONTROL2);
		//test = 0;
		//readMMC5983_XYZ(&my_sys.sensorB_MMC5983);
	}
}

uint64_t getGlobalTimer(GlobalTimer_t *thisGlobalTimer)
{
	uint8_t overflowCountBegin;
	uint64_t totValue;
	do
	{
		overflowCountBegin = thisGlobalTimer->overflowCounter;
		totValue = htim6.Instance->CNT + htim6.Instance->ARR * (thisGlobalTimer->overflowCounter);

	} while (overflowCountBegin != thisGlobalTimer->overflowCounter);

	return totValue;	//TODO there is a lot of potential timestamp synchronization issues here, need to be addressed
}

void setGlobalTimer(GlobalTimer_t *thisGlobalTimer, uint64_t newValue)
{
	uint32_t newValueMinus = newValue % htim6.Instance->ARR;
	htim6.Instance->CNT = newValueMinus;
}
