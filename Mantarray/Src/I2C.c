#include <string.h>
#include <stdio.h>
#include "I2C.h"
#include "system.h"
#include "EEPROM.h"

extern I2C_HandleTypeDef hi2c2;
extern UART_HandleTypeDef huart1;
extern System my_sys;

void I2CInit(I2C_t *thisI2C)
{
	// Disable Own Address1 before setting the new address configuration
	hi2c2.Instance->OAR1 &= ~I2C_OAR1_OA1EN;
	hi2c2.Instance->OAR1 = (I2C_OAR1_OA1EN | 0b10001110);
	//hi2c2.Instance->OAR1 = (I2C_OAR1_OA1EN | (*(uint8_t*) I2C_ADDR) << 1);
	//hi2c2->Instance->CR2 &= ~I2C_CR2_NACK;
	__HAL_I2C_ENABLE_IT(&hi2c2, I2C_IT_RXI | I2C_IT_STOPI | I2C_IT_ADDRI);
	//uint8_t test = *(uint8_t*) I2C_ADDR;
	//HAL_I2C_Slave_Receive_IT(&hi2c2, thisI2C->receiveBuffer, I2C_RECEIVE_LENGTH);
}

void I2C2_IRQHandler(void)
{
	uint8_t newData;
	uint32_t ITFlags   = READ_REG(hi2c2.Instance->ISR);
	uint32_t ITSources = READ_REG(hi2c2.Instance->CR1);
	if ((I2C_CHECK_FLAG(hi2c2.Instance->ISR, I2C_FLAG_STOPF) != RESET) && (I2C_CHECK_IT_SOURCE(hi2c2.Instance->CR1, I2C_IT_STOPI) != RESET))
	{
		GPIOC->BSRR = GPIO_PIN_0;
		GPIOC->BRR = GPIO_PIN_0;
		// Clear STOP Flag
		__HAL_I2C_CLEAR_FLAG(&hi2c2, I2C_FLAG_STOPF);
		my_sys.BUS_FLAG = 1;
	}
	if ((I2C_CHECK_FLAG(hi2c2.Instance->ISR, I2C_FLAG_RXNE) != RESET) && (I2C_CHECK_IT_SOURCE(hi2c2.Instance->CR1, I2C_IT_RXI) != RESET))
	{
		GPIOC->BSRR = GPIO_PIN_0;
		GPIOC->BRR = GPIO_PIN_0;
		__HAL_I2C_CLEAR_FLAG(&hi2c2, I2C_FLAG_RXNE);
		newData = (uint8_t)hi2c2.Instance->RXDR;
	}
	if ((I2C_CHECK_FLAG(hi2c2.Instance->ISR, I2C_FLAG_ADDR) != RESET) && (I2C_CHECK_IT_SOURCE(hi2c2.Instance->CR1, I2C_IT_ADDRI) != RESET))
	{
		GPIOC->BSRR = GPIO_PIN_0;
		GPIOC->BRR = GPIO_PIN_0;
		// Clear ADDR Flag and turn off line hold
		__HAL_I2C_CLEAR_FLAG(&hi2c2, I2C_FLAG_ADDR);
	}
	return;
}

