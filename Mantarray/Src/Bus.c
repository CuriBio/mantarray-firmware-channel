#include <string.h>
#include <stdio.h>
#include "main.h"
#include "system.h"
#include "Magnetometer.h"
#include "Bus.h"

extern System my_sys;
extern UART_HandleTypeDef huart1;
extern I2C_HandleTypeDef hi2c2;

void BusInit(Bus_t *thisBus)
{
	thisBus->_C_GPIO_Bus = GPIOA;
	thisBus->_GPIO_Bus = GPIOB;

	thisBus->_C0_GPIO_Pin = GPIO_PIN_13;
	thisBus->_C1_GPIO_Pin = GPIO_PIN_14;
	thisBus->_C2_GPIO_Pin = GPIO_PIN_15;
	thisBus->_0_GPIO_Pin = GPIO_PIN_0;
	thisBus->_1_GPIO_Pin = GPIO_PIN_1;
	thisBus->_2_GPIO_Pin = GPIO_PIN_2;
	thisBus->_3_GPIO_Pin = GPIO_PIN_3;
	thisBus->_4_GPIO_Pin = GPIO_PIN_4;
	thisBus->_5_GPIO_Pin = GPIO_PIN_5;
	thisBus->_6_GPIO_Pin = GPIO_PIN_6;
	thisBus->_7_GPIO_Pin = GPIO_PIN_7;

	uint32_t temp = 0;
	//Set main bus output speed to very high
	temp = thisBus->_GPIO_Bus->OSPEEDR;
	temp &= ~BUS_BUSMASK32;
	temp |= BUS_ACK_OSPEEDR;
	thisBus->_GPIO_Bus->OSPEEDR = temp;
	//Set main bus output type to output push-pull
	temp = thisBus->_GPIO_Bus->OTYPER;
	temp &= ~BUS_BUSMASK32;
	thisBus->_GPIO_Bus->OTYPER = temp;
	//Set main bus pullup/down resistors to none
	temp = thisBus->_GPIO_Bus->PUPDR;
	temp &= ~BUS_BUSMASK32;
	thisBus->_GPIO_Bus->PUPDR = temp;
	//Set C bus output speed to very high
	temp = thisBus->_C_GPIO_Bus->OSPEEDR;
	temp &= ~BUS_CBUSMASK32;
	temp |= BUS_CACK_OSPEEDR;
	thisBus->_C_GPIO_Bus->OSPEEDR = temp;
	//Set C bus output type to output push-pull
	temp = thisBus->_C_GPIO_Bus->OTYPER;
	temp &= ~BUS_CBUSMASK32;
	thisBus->_C_GPIO_Bus->OTYPER = temp;
	//Set C bus pullup/down resistors to none
	temp = thisBus->_C_GPIO_Bus->PUPDR;
	temp &= ~BUS_CBUSMASK32;
	thisBus->_C_GPIO_Bus->PUPDR = temp;
}

void MockData(Magnetometer_t * thisMagnetometer)
{
	/*
	thisMagnetometer->XReadings[0] += 1000;
	thisMagnetometer->XReadings[1] += 100;
	thisMagnetometer->XReadings[2] += 10;
	thisMagnetometer->YReadings[0] += 2000;
	thisMagnetometer->YReadings[1] += 200;
	thisMagnetometer->YReadings[2] += 20;
	thisMagnetometer->ZReadings[0] += 3000;
	thisMagnetometer->ZReadings[1] += 300;
	thisMagnetometer->ZReadings[2] += 30;
	thisMagnetometer->tempReading += 40;
	*/
}

void WriteDataFrame(Magnetometer_t * thisMagnetometer, Bus_t *thisBus)
{

	Acknowledge(thisBus);

	//thisMagnetometer->timeStamp = getGlobalTimer(&my_sys.GlobalTimer);
	SendData(thisBus, (uint8_t*)(&thisMagnetometer->timeStamp), 4);
	for (uint8_t i = 0; i < 9; i++)
	{
		if (thisMagnetometer->sensorConfig>>i & 0x0001)
		{
			SendData(thisBus, (uint8_t*)(thisMagnetometer->Readings[X_AX] + i), 2);
		}
	}
	//SendData(thisBus, (uint8_t*)(&thisMagnetometer->tempReading), 2);
	Complete(thisBus);
}

void SendData (Bus_t *thisBus, uint8_t* data, uint8_t len)
{
	for (uint8_t i = 0; i < len; i++)
	{
		thisBus->_GPIO_Bus->BSRR = (uint32_t) ((0x000000FF & data[i]) | ((0x000000FF & ~data[i]) << 16));
		//thisBus->_GPIO_Bus->BRR = (uint32_t) (0x000000FF & ~data[i]);
		GPIOA->BSRR = GPIO_PIN_14;
		GPIOA->BRR = GPIO_PIN_14;
		//thisBus->_C_GPIO_Bus->BSRR = thisBus->_C1_GPIO_Pin;
		//thisBus->_C_GPIO_Bus->BRR = thisBus->_C1_GPIO_Pin;
	}
	//Set A2, A12, A13 (HIGH)
	//GPIOA->BRR = 0b0011000000000100 << 16; //move to upper 16 bits
	//Set A2, A12, A13 (HIGH)
	//GPIOA->BSRR = 0b0011000000000100;
	//Clear A2, A12, A13 (LOW)
	//GPIOA->BSRR = 0b0011000000000100 << 16; //move to upper 16 bits
	//Clear A2, A12, A13 (LOW)
	//GPIOA->BRR = 0b0011000000000100;

	/*HAL_GPIO_WritePin(thisBus->_GPIO_Bus, thisBus->_0_GPIO_Pin, 1);
	HAL_GPIO_WritePin(thisBus->_GPIO_Bus, thisBus->_1_GPIO_Pin, 0);
	HAL_GPIO_WritePin(thisBus->_GPIO_Bus, thisBus->_2_GPIO_Pin, 1);
	HAL_GPIO_WritePin(thisBus->_GPIO_Bus, thisBus->_3_GPIO_Pin, 0);
	HAL_GPIO_WritePin(thisBus->_GPIO_Bus, thisBus->_4_GPIO_Pin, 1);
	HAL_GPIO_WritePin(thisBus->_GPIO_Bus, thisBus->_5_GPIO_Pin, 0);
	HAL_GPIO_WritePin(thisBus->_GPIO_Bus, thisBus->_6_GPIO_Pin, 1);
	HAL_GPIO_WritePin(thisBus->_GPIO_Bus, thisBus->_7_GPIO_Pin, 0);*/
}

void Acknowledge (Bus_t *thisBus)
{
	//thisBus->_GPIO_Bus->MODER ^= BUS_ACK_MODER;
	//thisBus->_C_GPIO_Bus->MODER ^= BUS_CACK_MODER;
	//thisBus->_C_GPIO_Bus->BSRR = (uint32_t) (0x00002000);
	thisBus->GPIO_InitStruct.Pin = thisBus->_0_GPIO_Pin | thisBus->_1_GPIO_Pin |
								   thisBus->_2_GPIO_Pin | thisBus->_3_GPIO_Pin |
								   thisBus->_4_GPIO_Pin | thisBus->_5_GPIO_Pin |
								   thisBus->_6_GPIO_Pin | thisBus->_7_GPIO_Pin;
	thisBus->GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	thisBus->GPIO_InitStruct.Pull = GPIO_NOPULL;
	thisBus->GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(thisBus->_GPIO_Bus, &thisBus->GPIO_InitStruct);

	thisBus->GPIO_InitStruct.Pin = thisBus->_C0_GPIO_Pin | thisBus->_C1_GPIO_Pin |
								   thisBus->_C2_GPIO_Pin;
	thisBus->GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	thisBus->GPIO_InitStruct.Pull = GPIO_NOPULL;
	thisBus->GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	//HAL_GPIO_Init(thisBus->_C_GPIO_Bus, &thisBus->GPIO_InitStruct);

	//Send Acknowledge on line C0 to main micro
	HAL_GPIO_WritePin(thisBus->_C_GPIO_Bus, thisBus->_C0_GPIO_Pin, GPIO_PIN_SET);
}

void Complete (Bus_t *thisBus)
{
	thisBus->_GPIO_Bus->BRR = (uint32_t) (0x000000FF);
	thisBus->_C_GPIO_Bus->BRR = (uint32_t) (0x00002000);
	//thisBus->_GPIO_Bus->MODER ^= BUS_ACK_MODER;
	//thisBus->_C_GPIO_Bus->MODER ^= BUS_CACK_MODER;
	//Signal data frame is over on line C0 to main micro
	HAL_GPIO_WritePin(thisBus->_C_GPIO_Bus, thisBus->_C0_GPIO_Pin, GPIO_PIN_RESET);

	thisBus->GPIO_InitStruct.Pin = thisBus->_0_GPIO_Pin | thisBus->_1_GPIO_Pin |
						  	  	   thisBus->_2_GPIO_Pin | thisBus->_3_GPIO_Pin |
								   thisBus->_4_GPIO_Pin | thisBus->_5_GPIO_Pin |
								   thisBus->_6_GPIO_Pin | thisBus->_7_GPIO_Pin;
	thisBus->GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	thisBus->GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(thisBus->_GPIO_Bus, &thisBus->GPIO_InitStruct);

	thisBus->GPIO_InitStruct.Pin = thisBus->_C0_GPIO_Pin | thisBus->_C1_GPIO_Pin |
								   thisBus->_C2_GPIO_Pin;
	thisBus->GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	thisBus->GPIO_InitStruct.Pull = GPIO_NOPULL;
	//HAL_GPIO_Init(thisBus->_C_GPIO_Bus, &thisBus->GPIO_InitStruct);
}
