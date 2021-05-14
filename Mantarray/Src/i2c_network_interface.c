#include "i2c_network_interface.h"


I2C_t * I2C_interface_create(I2C_HandleTypeDef *I2C_handle,uint8_t channel_address)
{
	I2C_t * thisI2C = (I2C_t *) malloc(sizeof(I2C_t));
	if(thisI2C != NULL)
	{
		thisI2C->I2C_line = I2C_handle;
		i2c2_interrupt_interface_pointer = thisI2C;
		thisI2C->buffer_index=0;

		// Disable Own Address1 before setting the new address configuration
		//TODO it is much safer to use HAL compatible address change instead of manual mode
		//is ther any reason for using manual mode?
		thisI2C->I2C_line->Instance->OAR1 &= ~I2C_OAR1_OA1EN;
		thisI2C->I2C_line->Instance->OAR1 = (I2C_OAR1_OA1EN | ( channel_address << 1) );
		__HAL_I2C_ENABLE_IT(thisI2C->I2C_line, I2C_IT_RXI | I2C_IT_STOPI | I2C_IT_ADDRI);
		HAL_I2C_Slave_Receive_IT(thisI2C->I2C_line, thisI2C->receiveBuffer, I2C_RECEIVE_LENGTH);
	}
	else
	{
		//TODO  erro handler
	}
	return thisI2C;
}

void I2C2_IRQHandler(void)
{
	//uint32_t ITFlags   = READ_REG(hi2c2.Instance->ISR);
	//uint32_t ITSources = READ_REG(hi2c2.Instance->CR1);
	if ((I2C_CHECK_FLAG(i2c2_interrupt_interface_pointer->I2C_line->Instance->ISR, I2C_FLAG_STOPF) != RESET) && (I2C_CHECK_IT_SOURCE(i2c2_interrupt_interface_pointer->I2C_line->Instance->CR1, I2C_IT_STOPI) != RESET))
	{
		// Clear STOP Flag
		__HAL_I2C_CLEAR_FLAG(i2c2_interrupt_interface_pointer->I2C_line, I2C_FLAG_STOPF);
	}
	if ((I2C_CHECK_FLAG(i2c2_interrupt_interface_pointer->I2C_line->Instance->ISR, I2C_FLAG_RXNE) != RESET) && (I2C_CHECK_IT_SOURCE(i2c2_interrupt_interface_pointer->I2C_line->Instance->CR1, I2C_IT_RXI) != RESET))
	{
		__HAL_I2C_CLEAR_FLAG(i2c2_interrupt_interface_pointer->I2C_line, I2C_FLAG_RXNE);
		if(i2c2_interrupt_interface_pointer->buffer_index < I2C_RECEIVE_LENGTH)
		{
			i2c2_interrupt_interface_pointer->receiveBuffer[i2c2_interrupt_interface_pointer->buffer_index] = (uint8_t)i2c2_interrupt_interface_pointer->I2C_line->Instance->RXDR;
			i2c2_interrupt_interface_pointer->buffer_index++;
		}
	}
	if ((I2C_CHECK_FLAG(i2c2_interrupt_interface_pointer->I2C_line->Instance->ISR, I2C_FLAG_ADDR) != RESET) && (I2C_CHECK_IT_SOURCE(i2c2_interrupt_interface_pointer->I2C_line->Instance->CR1, I2C_IT_ADDRI) != RESET))
	{
		// Clear ADDR Flag and turn off line hold
		__HAL_I2C_CLEAR_FLAG(i2c2_interrupt_interface_pointer->I2C_line, I2C_FLAG_ADDR);
	}
	return;
}

/******************************************************************************/
/* STM32L0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l0xx.s).                    */
/******************************************************************************/

/*
void I2C2_IRQHandler(void)
{
  if (hi2c2.Instance->ISR & (I2C_FLAG_BERR | I2C_FLAG_ARLO | I2C_FLAG_OVR)) {
    HAL_I2C_ER_IRQHandler(&hi2c2);
  } else {
    HAL_I2C_EV_IRQHandler(&hi2c2);
  }
}*/

