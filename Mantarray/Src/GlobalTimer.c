#include "GlobalTimer.h"
#include "main.h"
#include "system.h"
#include <stdio.h>
extern TIM_HandleTypeDef htim21;

extern System my_sys;


void global_timer_create(GlobalTimer_t *thisGlobalTimer, TIM_HandleTypeDef timer_id)
{
	//Start global timer and initialize struct
	thisGlobalTimer->h_timer = timer_id;
	HAL_TIM_Base_Start_IT(&htim21);
	thisGlobalTimer->overflow_counter = 0;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim21)
	{
		my_sys.ph_global_timer->overflow_counter++;
	}
}

uint64_t get_global_timer(GlobalTimer_t *thisGlobalTimer)
{
	uint8_t overflow_count_begin;
	uint64_t tot_value;
	do
	{
		overflow_count_begin = thisGlobalTimer->overflow_counter;
		tot_value = thisGlobalTimer->h_timer.Instance->CNT + thisGlobalTimer->h_timer.Instance->ARR * (thisGlobalTimer->overflow_counter);

	} while (overflow_count_begin != thisGlobalTimer->overflow_counter);

	return tot_value;	//TODO there is a lot of potential timestamp synchronization issues here, need to be addressed
}

void set_global_timer(GlobalTimer_t *thisGlobalTimer, uint64_t new_value)
{
	uint32_t new_value_minus = new_value % thisGlobalTimer->h_timer.Instance->ARR;
	thisGlobalTimer->h_timer.Instance->CNT = new_value_minus;
}
