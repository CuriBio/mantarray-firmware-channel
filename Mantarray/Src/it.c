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
			my_sys.i2c_line->new_command_is_ready_flag = 1;
			//is it a single byte command?  command higher than I2C_SET_MULTIPLE_BYTE_COMMAND may have more than 1 byte
			/*if(my_sys.i2c_line->receiveBuffer[0] <= I2C_SET_MULTIPLE_BYTE_COMMAND)
			{
				my_sys.i2c_line->new_command_is_ready_flag = 1;
				//TODO  there is a better solution for this CODE with SIS for the next version data send request will come from an external int service
				if(!my_sys.i2c_line->buffer_index && my_sys.i2c_line->receiveBuffer[0] == I2C_PACKET_SEND_DATA_FRAME)
				{
						internal_bus_write_data_frame(my_sys.data_bus , my_sys.bus_output_buffer , MODULE_SYSTEM_PACKET_LENGHT);
						my_sys.i2c_line->buffer_index = 0;
						my_sys.i2c_line->new_command_is_ready_flag = 0;
				}
			}
			else
			{
				//ok now we have 3 byts for our long command and it is ready for process
				if(my_sys.i2c_line->buffer_index == 3)
					my_sys.i2c_line->new_command_is_ready_flag = 1;
			}*/
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
