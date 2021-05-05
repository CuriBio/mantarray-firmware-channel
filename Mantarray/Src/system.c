#include <lis3mdl_driver.h>
#include "system.h"
#include "main.h"
#include "GlobalTimer.h"
#include "UART_Comm.h"
#include "EEPROM.h"
#include "I2C.h"

extern SPI_HandleTypeDef hspi1;
extern I2C_HandleTypeDef hi2c2;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim21;
extern TIM_HandleTypeDef htim22;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern System my_sys;

void module_system_init(System *thisSystem)
{
	BusInit(&thisSystem->Bus);

	GlobalTimerInit(&thisSystem->GlobalTimer);
	//HAL_Delay(1000);
	I2CInit(&thisSystem->I2C);

	MagnetometerInit(&thisSystem->s1);

	BusInit(&thisSystem->Bus);

	return;
}



void state_machine(System *thisSystem)
{
	uint8_t status = 0;
	uint8_t testData[23] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22};
	uint32_t temp = 0;
	Bus_t* thisBus = &thisSystem->Bus;
	while(1)
	{
		switch(thisSystem->state)
		{
			case MODULE_SYSTEM_STATUS_START:;
				//Check if system has undergone first time setup by looking for a 32-bit random number in EEPROM
				//IF A NEW FIRST TIME SETUP IS DESIRED TO BE RUN just change the value of EEPROM_FIRST_TIME_COMPLETE in <EEPROM.h>
				//uint32_t test = *(uint32_t*) FIRST_TIME_INITIATION;
				thisSystem->state = (*(uint32_t*) FIRST_TIME_INITIATION != EEPROM_FIRST_TIME_COMPLETE) ?
						MODULE_SYSTEM_STATUS_FIRST_TIME :
						MODULE_SYSTEM_STATUS_INITIATION;
			break;
			//-----------
			case MODULE_SYSTEM_STATUS_FIRST_TIME:
				EEPROMInit(thisSystem);
				thisSystem->state = MODULE_SYSTEM_STATUS_INITIATION;

			break;
			//-----------
			case MODULE_SYSTEM_STATUS_INITIATION:
				module_system_init(thisSystem);
				thisSystem->BUS_FLAG = 0;
				thisSystem->ID = 0;			//TODO module type will be assigned by master micro and we need to reload that from eeprom/flash memory
				thisSystem->status = 0;
				thisSystem->type = 0; 			//TODO module type will be assigned by master micro and we need to reload that from eeprom/flash memory
				thisSystem->state = MODULE_SYSTEM_STATUS_IDLE;
			break;
			//-----------
			case MODULE_SYSTEM_STATUS_IDLE:
				if (thisSystem->BUS_FLAG == 1)
				{
					//GPIOC->BSRR = GPIO_PIN_0;
					//GPIOC->BRR = GPIO_PIN_0;
					//Set Bus pins to output
					temp = GPIOB->MODER;
					temp &= ~BUS_BUSMASK32;
					temp |= BUS_ACK_MODER;
					GPIOB->MODER = temp;
					//Set CBus pins to output
					//temp = GPIOA->MODER;
					//temp &= ~BUS_CBUSMASK32;
					//temp |= BUS_CACK_MODER;
					//GPIOA->MODER = temp;
					//Send acknowledge
					GPIOA->BSRR = GPIO_PIN_15;
					//GPIOA->BSRR = 0x00002000;
					//Acknowledge(&thisSystem->Bus);
					//GPIOC->BSRR = GPIO_PIN_0;
					//GPIOC->BRR = GPIO_PIN_0;
					//Send dataframe
					GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[0]) | ((0x000000FF & ~testData[0]) << 16));GPIOA->BSRR = GPIO_PIN_0;
					GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[1]) | ((0x000000FF & ~testData[1]) << 16));GPIOA->BSRR = GPIO_PIN_0;
					GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[2]) | ((0x000000FF & ~testData[2]) << 16));GPIOA->BSRR = GPIO_PIN_0;
					GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[3]) | ((0x000000FF & ~testData[3]) << 16));GPIOA->BSRR = GPIO_PIN_0;
					GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[4]) | ((0x000000FF & ~testData[4]) << 16));GPIOA->BSRR = GPIO_PIN_0;
					GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[5]) | ((0x000000FF & ~testData[5]) << 16));GPIOA->BSRR = GPIO_PIN_0;
					GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[6]) | ((0x000000FF & ~testData[6]) << 16));GPIOA->BSRR = GPIO_PIN_0;
					GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[7]) | ((0x000000FF & ~testData[7]) << 16));GPIOA->BSRR = GPIO_PIN_0;
					GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[8]) | ((0x000000FF & ~testData[8]) << 16));GPIOA->BSRR = GPIO_PIN_0;
					GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[9]) | ((0x000000FF & ~testData[9]) << 16));GPIOA->BSRR = GPIO_PIN_0;
					GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[10]) | ((0x000000FF & ~testData[10]) << 16));GPIOA->BSRR = GPIO_PIN_0;
					GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[11]) | ((0x000000FF & ~testData[11]) << 16));GPIOA->BSRR = GPIO_PIN_0;
					GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[12]) | ((0x000000FF & ~testData[12]) << 16));GPIOA->BSRR = GPIO_PIN_0;
					GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[13]) | ((0x000000FF & ~testData[13]) << 16));GPIOA->BSRR = GPIO_PIN_0;
					GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[14]) | ((0x000000FF & ~testData[14]) << 16));GPIOA->BSRR = GPIO_PIN_0;
					GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[15]) | ((0x000000FF & ~testData[15]) << 16));GPIOA->BSRR = GPIO_PIN_0;
					GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[16]) | ((0x000000FF & ~testData[16]) << 16));GPIOA->BSRR = GPIO_PIN_0;
					GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[17]) | ((0x000000FF & ~testData[17]) << 16));GPIOA->BSRR = GPIO_PIN_0;
					GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[18]) | ((0x000000FF & ~testData[18]) << 16));GPIOA->BSRR = GPIO_PIN_0;
					GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[19]) | ((0x000000FF & ~testData[19]) << 16));GPIOA->BSRR = GPIO_PIN_0;
					GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[20]) | ((0x000000FF & ~testData[20]) << 16));GPIOA->BSRR = GPIO_PIN_0;
					GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[21]) | ((0x000000FF & ~testData[21]) << 16));GPIOA->BSRR = GPIO_PIN_0;
					GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[22]) | ((0x000000FF & ~testData[22]) << 16));GPIOA->BSRR = GPIO_PIN_0;
					GPIOA->BRR = GPIO_PIN_0;
					thisSystem->BUS_FLAG = 0;
					//MockData(&thisSystem->Magnetometer);
					//WriteDataFrame(&thisSystem->Magnetometer, &thisSystem->Bus);
					//GPIOC->BSRR = GPIO_PIN_0;
					//GPIOC->BRR = GPIO_PIN_0;
					//Set all bus pins to low and send complete
					thisBus->_GPIO_Bus->BRR = (uint32_t) (0x000000FF);
					GPIOA->BRR = GPIO_PIN_15;
					//GPIOA->BRR = (uint32_t) (0x00002000);
					//Set Bus pins to input
					temp = GPIOB->MODER;
					temp &= ~BUS_BUSMASK32;
					GPIOB->MODER = temp;
					//Set CBus pins to input
					//temp = GPIOA->MODER;
					//temp &= ~BUS_CBUSMASK32;
					//GPIOA->MODER = temp;
					//Complete(&thisSystem->Bus);
					//GPIOC->BSRR = GPIO_PIN_0;
					//GPIOC->BRR = GPIO_PIN_0;
					//HAL_I2C_Slave_Receive_IT(&hi2c2, thisSystem->I2C.receiveBuffer, I2C_RECEIVE_LENGTH);
				}
				//HAL_Delay(1);
				//thisSystem->BUS_FLAG = 1;
				//GPIOC->BSRR = GPIO_PIN_0;
				//GPIOC->BRR = GPIO_PIN_0;
				//GPIOC->BSRR = ((GPIOC->ODR & GPIO_PIN_0) << 16) | (~GPIOC->ODR & GPIO_PIN_0);
				/*if (__HAL_GPIO_EXTI_GET_FLAG(thisSystem->sensorA_MMC5983.INT_GPIO_Pin))
				{
					readMMC5983_XYZ(&thisSystem->sensorA_MMC5983);
					__HAL_GPIO_EXTI_CLEAR_IT(thisSystem->sensorA_MMC5983.INT_GPIO_Pin);
				}*/
				/*if (__HAL_GPIO_EXTI_GET_FLAG(thisSystem->Magnetometer.sensorB_MMC5983.INT_GPIO_Pin))
				{
					readMMC5983_XYZ(&thisSystem->Magnetometer, &thisSystem->Magnetometer.sensorB_MMC5983);
					__HAL_GPIO_EXTI_CLEAR_IT(thisSystem->Magnetometer.sensorB_MMC5983.INT_GPIO_Pin);
				}*/
				/*if (__HAL_GPIO_EXTI_GET_FLAG(thisSystem->sensorA.DRDY_GPIO_Pin))
				{
					readLIS3MDL_XYZ(&thisSystem->sensorA);
					__HAL_GPIO_EXTI_CLEAR_IT(thisSystem->sensorA.DRDY_GPIO_Pin);
				}
				if (__HAL_GPIO_EXTI_GET_FLAG(thisSystem->sensorB.DRDY_GPIO_Pin))
				{
					readLIS3MDL_XYZ(&thisSystem->sensorB);
					__HAL_GPIO_EXTI_CLEAR_IT(thisSystem->sensorB.DRDY_GPIO_Pin);
				}
				if (__HAL_GPIO_EXTI_GET_FLAG(thisSystem->sensorC.DRDY_GPIO_Pin))
				{
					readLIS3MDL_XYZ(&thisSystem->sensorC);
					__HAL_GPIO_EXTI_CLEAR_IT(thisSystem->sensorC.DRDY_GPIO_Pin);
				}*/
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