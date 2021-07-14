#include "system.h"

extern I2C_HandleTypeDef hi2c2;
extern TIM_HandleTypeDef htim21;
extern System my_sys;

//------------------------i2c int------------------
void I2C2_IRQHandler(void)
{

	if ((I2C_CHECK_FLAG(my_sys.i2c_line->I2C_line->Instance->ISR, I2C_FLAG_RXNE) != RESET) && (I2C_CHECK_IT_SOURCE(my_sys.i2c_line->I2C_line->Instance->CR1, I2C_IT_RXI) != RESET))
	{
		if(my_sys.i2c_line->buffer_index < I2C_MAX_RECEIVE_LENGTH)
		{
			my_sys.i2c_line->receiveBuffer[my_sys.i2c_line->buffer_index] = (uint8_t)my_sys.i2c_line->I2C_line->Instance->RXDR;
			my_sys.i2c_line->buffer_index++;
			//is it a single byte command?  command higher than I2C_SET_MULTIPLE_BYTE_COMMAND may have more than 1 byte
			//If it is a send data frame command, execute and return immediately
			if (my_sys.i2c_line->receiveBuffer[0] == I2C_PACKET_SEND_DATA_FRAME)
			{
				internal_bus_write_data_frame(my_sys.data_bus , my_sys.bus_output_buffer , MODULE_SYSTEM_PACKET_LENGHT);
				my_sys.i2c_line->new_command_is_ready_flag = 1;
			}

			//Single byte range
			else if (my_sys.i2c_line->receiveBuffer[0] < I2C_3_BYTE_COMMAND_START)
			{
				my_sys.i2c_line->new_command_is_ready_flag = 1;
			}

			//Three byte command range
			else if (my_sys.i2c_line->receiveBuffer[0] < I2C_31_BYTE_COMMAND_START)
			{
				if(my_sys.i2c_line->buffer_index == 3)
				{
					my_sys.i2c_line->new_command_is_ready_flag = 1;
				}
			}

			//Thirty-one byte command range
			else if (my_sys.i2c_line->receiveBuffer[0] < I2C_PACKET_SET_NEW_ADDRESS)
			{
				if(my_sys.i2c_line->buffer_index == 31)
				{
					my_sys.i2c_line->new_command_is_ready_flag = 1;
				}
			}

			//Only thing left for it to be is a new I2C address
			else
			{
				my_sys.i2c_line->new_command_is_ready_flag = 1;
			}
		}
		__HAL_I2C_CLEAR_FLAG(my_sys.i2c_line->I2C_line, I2C_FLAG_RXNE);
	}
	if ((I2C_CHECK_FLAG(my_sys.i2c_line->I2C_line->Instance->ISR, I2C_FLAG_ADDR) != RESET) && (I2C_CHECK_IT_SOURCE(my_sys.i2c_line->I2C_line->Instance->CR1, I2C_IT_ADDRI) != RESET))
	{
		// Clear ADDR Flag and turn off line hold
		__HAL_I2C_CLEAR_FLAG(my_sys.i2c_line->I2C_line, I2C_FLAG_ADDR);
	}
	if ((I2C_CHECK_FLAG(my_sys.i2c_line->I2C_line->Instance->ISR, I2C_FLAG_STOPF) != RESET) && (I2C_CHECK_IT_SOURCE(my_sys.i2c_line->I2C_line->Instance->CR1, I2C_IT_STOPI) != RESET))
	{
		// Clear STOP Flag
		__HAL_I2C_CLEAR_FLAG(my_sys.i2c_line->I2C_line, I2C_FLAG_STOPF);
	}

	return;
}
//---------------------------  tim21 int----------------
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim21)
	{
		my_sys.ph_global_timer->overflow_counter++;
	}
}
