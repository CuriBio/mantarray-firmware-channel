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
		HAL_I2C_Slave_Receive_IT(thisI2C->I2C_line, (uint8_t *)thisI2C->receiveBuffer, I2C_RECEIVE_LENGTH);
	}
	else
	{
		//TODO  erro handler
	}
	return thisI2C;
}


