#include "system.h"
#include "main.h"
#include "GlobalTimer.h"
#include "lis3mdl.h"
#include "UART_Comm.h"

extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim21;
extern UART_HandleTypeDef huart2;

void module_system_init(System *thisSystem)
{
	//Start lis3mdl and initialize struct
	thisSystem->sensorA.CS_GPIO_Bus = GPIOA;
	thisSystem->sensorA.CS_GPIO_Pin = GPIO_PIN_6;
	thisSystem->sensorB.CS_GPIO_Bus = GPIOA;
	thisSystem->sensorB.CS_GPIO_Pin = GPIO_PIN_7;
	thisSystem->sensorC.CS_GPIO_Bus = GPIOA;
	thisSystem->sensorC.CS_GPIO_Pin = GPIO_PIN_8;
	init_LIS3MDL_struct(&thisSystem->sensorA);
	init_LIS3MDL_struct(&thisSystem->sensorB);
	init_LIS3MDL_struct(&thisSystem->sensorC);

	//Start global timer and initialize struct
	HAL_TIM_Base_Start_IT(&htim2);
	init_Global_Timer_struct(&thisSystem->GlobalTimerTest);	//TODO test whether initialization after timer start is a good idea
	//There is an issue that on timer startup, the interrupt gets triggered once, therefore starting at timer measurements at 71 minutes
	//If this order does not work, try setting overflowCounter in init_Global_Timer_struct to max (255) so that on timer startup, it will wrap around to 0

	thisSystem->new_lis3mdl_data = 0;
	return;
}

void state_machine(System *thisSystem)
{
	while(1)
	{
		switch(thisSystem->state)
		{
			case MODULE_SYSTEM_STATUS_START:
				thisSystem->state = MODULE_SYSTEM_STATUS_INITIATION;
				module_system_init(thisSystem);
			break;
			//-----------
			case MODULE_SYSTEM_STATUS_FIRST_TIME:
			break;
			//-----------
			case MODULE_SYSTEM_STATUS_INITIATION:
				thisSystem->ID = 0;			//TODO module type will be assigned by master micro and we need to reload that from eeprom/flash memory
				thisSystem->status = 0;
				thisSystem->type = 0; 			//TODO module type will be assigned by master micro and we need to reload that from eeprom/flash memory
				thisSystem->state = MODULE_SYSTEM_STATUS_IDLE;
			break;
			//-----------
			case MODULE_SYSTEM_STATUS_IDLE:
				if (thisSystem->new_lis3mdl_data)
				{
					if (thisSystem->new_lis3mdl_data & LIS3MDL_IDA)
					{
						readLIS3MDL_XYZTemp(&thisSystem->sensorA);
						thisSystem->new_lis3mdl_data ^= LIS3MDL_IDA;
					}
					if (thisSystem->new_lis3mdl_data & LIS3MDL_IDB)
					{
						readLIS3MDL_XYZTemp(&thisSystem->sensorB);
						thisSystem->new_lis3mdl_data ^= LIS3MDL_IDB;
					}
					if (thisSystem->new_lis3mdl_data & LIS3MDL_IDC)
					{
						readLIS3MDL_XYZTemp(&thisSystem->sensorC);
						thisSystem->new_lis3mdl_data ^= LIS3MDL_IDC;
					}
				}
			break;
			//-----------
			case MODULE_SYSTEM_STATUS_CALIBRATION:
			break;
			//-----------
			case MODULE_SYSTEM_STATUS_FIRMWARE_UPDATE:
			break;
			//-----------
			case MODULE_SYSTEM_STATUS_FAULTY:
			break;
		}
	}
}
