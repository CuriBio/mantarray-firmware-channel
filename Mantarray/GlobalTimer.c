#include "GlobalTimer.h"
#include "system.h"
#include "main.h"
#include <string.h>
#include <stdio.h>

extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim21;

extern System my_sys;

void init_Global_Timer_struct(GlobalTimer* thisGlobalTimer)
{
	thisGlobalTimer->overflowCounter = 0;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim2)
	{
		//TODO set up tim2's clock frequency
		my_sys.GlobalTimerTest.overflowCounter++;
		//TEST CODE
		my_sys.GlobalTimerTest.uartBufLen = sprintf(my_sys.GlobalTimerTest.uartBuffer, "Reached Max \r\n");
		serialSend(&huart2, my_sys.GlobalTimerTest.uartBuffer, my_sys.GlobalTimerTest.uartBufLen);
	}
	if (htim == &htim21)
	{
		//TODO Implement timer reset
	}
}

uint64_t getGlobalTimer(GlobalTimer *thisGlobalTimer)
{
	uint8_t overflowCountBegin;
	uint64_t totValue;
	do
	{
		overflowCountBegin = thisGlobalTimer->overflowCounter;
		totValue = htim2.Instance->CNT + htim2.Instance->ARR * (thisGlobalTimer->overflowCounter);

	} while (overflowCountBegin != thisGlobalTimer->overflowCounter);

	return totValue;	//TODO there is a lot of potential timestamp synchronization issues here, need to be addressed
}

void setGlobalTimer(GlobalTimer *thisGlobalTimer, uint64_t newValue)
{
	uint32_t newValueMinus = newValue % htim2.Instance->ARR;
	htim2.Instance->CNT = newValueMinus;
}
