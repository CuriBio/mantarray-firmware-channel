#include "system.h"

//----------------------------------
void module_system_init(System *thisSystem, SPI_HandleTypeDef * h_SPI, I2C_HandleTypeDef * h_I2C, TIM_HandleTypeDef * h_global_timer)
{
	thisSystem->data_bus = internal_bus_create(GPIOB,  BUS0_Pin | BUS1_Pin | BUS2_Pin | BUS3_Pin | BUS4_Pin | BUS5_Pin | BUS6_Pin | BUS7_Pin,
											BUS_CLK_GPIO_Port, BUS_CLK_Pin,
											BUS_C1_GPIO_Port, BUS_C1_Pin);

	thisSystem->ph_global_timer = global_timer_create(h_global_timer);

	HAL_GPIO_WritePin(SPI_A_CS_GPIO_Port, SPI_A_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SPI_B_CS_GPIO_Port, SPI_B_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SPI_C_CS_GPIO_Port, SPI_C_CS_Pin, GPIO_PIN_SET);

	thisSystem->i2c_line = I2C_interface_create(h_I2C,100);

	// init sensors
	thisSystem->sensors[0] = magnetometer_create(MAGNETOMETER_TYPE_MMC5983,h_SPI , SPI_A_CS_GPIO_Port , SPI_A_CS_Pin , mag_int_a_GPIO_Port , mag_int_a_Pin);
	thisSystem->sensors[1] = magnetometer_create(MAGNETOMETER_TYPE_MMC5983,h_SPI , SPI_B_CS_GPIO_Port , SPI_B_CS_Pin , mag_int_b_GPIO_Port , mag_int_b_Pin);
	thisSystem->sensors[2] = magnetometer_create(MAGNETOMETER_TYPE_MMC5983,h_SPI , SPI_C_CS_GPIO_Port , SPI_C_CS_Pin , mag_int_c_GPIO_Port , mag_int_c_Pin);

	return;
}

void state_machine(System *thisSystem)
{
	uint32_t zeros[33] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	uint8_t b_read_permit =0;
	uint8_t byte_shifter = 0;
	uint8_t this_byte = 0;
	while(1)
	{
		if(b_read_permit)
		{
			for (uint8_t sensor_num = 0; sensor_num < MODULE_SYSTEM_NUM_SENSORS; sensor_num++)
			{
				if( (thisSystem->sensors[sensor_num]->sensor_status == MAGNETOMETER_OK) & thisSystem->sensors[sensor_num]->b_new_data_needed)
				{
					if(magnetometer_read(thisSystem->sensors[sensor_num]))
					{
						byte_shifter = 0;
						while (byte_shifter < 5)
						{
							//output_data[byte_shifter + sensor_num * 11] = *(((uint8_t*)&thisSystem->sensors[sensor_num]->time_stamp) + byte_shifter);
							this_byte = *(((uint8_t*)&thisSystem->sensors[sensor_num]->time_stamp) + byte_shifter);
							thisSystem->bus_output_buffer[byte_shifter + sensor_num * 11] = (uint32_t)(thisSystem->data_bus->bus_mask & this_byte)  | ((thisSystem->data_bus->bus_mask & ~this_byte)  << 16);
							byte_shifter++;
						}

						while (byte_shifter < 11)
						{
							//output_data[byte_shifter + sensor_num * 11] = *(((uint8_t*)thisSystem->sensors[sensor_num]->Readings) + (byte_shifter - 5));
							this_byte = *(((uint8_t*)thisSystem->sensors[sensor_num]->Readings) + (byte_shifter - 5));
							thisSystem->bus_output_buffer[byte_shifter + sensor_num * 11] = (uint32_t)(thisSystem->data_bus->bus_mask & this_byte)  | ((thisSystem->data_bus->bus_mask & ~this_byte)  << 16);
							byte_shifter++;
						}

						//Declare that new data is no longer needed
						thisSystem->sensors[sensor_num]->b_new_data_needed = 0;
						//Begin a new data conversion immediately
						//TODO we should not directly touch anything in low layer lib evrythingh must go through magnetometer.c  otherwise there we can not have a sensor agnostic architect
						MMC5983_register_write((MMC5983_t*)thisSystem->sensors[sensor_num]->magnetometer, MMC5983_INTERNALCONTROL0, MMC5983_CTRL0_TM_M);
						//Timestamp the new data conversion you ordered
						thisSystem->sensors[sensor_num]->time_stamp = get_global_timer(thisSystem->ph_global_timer);
						//thisSystem->sensors[sensor_num]->time_stamp++;

					} //Check if the magnetometer has new data ready
					else
					{
						memcpy(&thisSystem->bus_output_buffer[sensor_num * 11], zeros, 44);
					}
				} //Check if magnetometer is functional and if new data is needed
			} //Sensor loop
			b_read_permit =0;
		}
		//------------------------------------------
		if(thisSystem->i2c_line->new_command_is_ready_flag)
		{
			switch(thisSystem->i2c_line->receiveBuffer[0])
			{
				//-------------------------------
				case I2C_PACKET_SEND_DATA_FRAME:
				{
					thisSystem->sensors[0]->b_new_data_needed = 1;
					thisSystem->sensors[1]->b_new_data_needed = 1;
					thisSystem->sensors[2]->b_new_data_needed = 1;
					break;
				}
				//-------------------------------
				case I2C_PACKET_SET_BOOT0_LOW:
				{
					  HAL_GPIO_WritePin(CHN_OUT_BT0_GPIO_Port, CHN_OUT_BT0_Pin, GPIO_PIN_RESET);
					break;
				}
				//-------------------------------
				case I2C_PACKET_SET_BOOT0_HIGH:
				{
					  HAL_GPIO_WritePin(CHN_OUT_BT0_GPIO_Port, CHN_OUT_BT0_Pin, GPIO_PIN_SET);
					break;
				}
				//-------------------------------
				case I2C_PACKET_SET_RESET_LOW:
				{
					  HAL_GPIO_WritePin(CHN_OUT_RST_GPIO_Port, CHN_OUT_RST_Pin, GPIO_PIN_RESET);
					break;
				}
				//-------------------------------
				case I2C_PACKET_SET_RESET_HIGH:
				{
					 HAL_GPIO_WritePin(CHN_OUT_RST_GPIO_Port, CHN_OUT_RST_Pin, GPIO_PIN_SET);
					break;
				}
				//---------this is a code for testing LED and making fun demo we can not have them in production release version
				//---------since it may make serious conflicts and issue with magnetometer reader and scheduler ----------------
				case I2C_PACKET_SET_RED_ON:
				{
					  HAL_GPIO_WritePin(SPI_C_CS_GPIO_Port, SPI_C_CS_Pin, GPIO_PIN_RESET);
					break;
				}
				//-------------------------------
				case I2C_PACKET_SET_RED_OFF:
				{
					  HAL_GPIO_WritePin(SPI_C_CS_GPIO_Port, SPI_C_CS_Pin, GPIO_PIN_SET);
					break;
				}
				//-------------------------------
				case I2C_PACKET_SET_GREEN_ON:
				{
					  HAL_GPIO_WritePin(SPI_A_CS_GPIO_Port, SPI_A_CS_Pin, GPIO_PIN_RESET);
					break;
				}
				//-------------------------------
				case I2C_PACKET_SET_GREEN_OFF:
				{
					  HAL_GPIO_WritePin(SPI_A_CS_GPIO_Port, SPI_A_CS_Pin, GPIO_PIN_SET);
					break;
				}
				//-------------------------------
				case I2C_PACKET_SET_BLUE_ON:
				{
					  HAL_GPIO_WritePin(SPI_B_CS_GPIO_Port, SPI_B_CS_Pin, GPIO_PIN_RESET);
					break;
				}
				//-------------------------------
				case I2C_PACKET_SET_BLUE_OFF:
				{
					  HAL_GPIO_WritePin(SPI_B_CS_GPIO_Port, SPI_B_CS_Pin, GPIO_PIN_SET);
					break;
				}
				case I2C_PACKET_RESET_GLOBAL_TIMER:
				{
					thisSystem->ph_global_timer->h_timer->Instance->CNT = 0;
					thisSystem->ph_global_timer->overflow_counter = 0;
					break;
				}

				//----------test cases---------------------
				case I2C_PACKET_SENSOR_TEST_ROUTINE:
				{
					if(thisSystem->sensors[0]->sensor_status == MAGNETOMETER_FAULTY )
					{
						HAL_GPIO_WritePin(SPI_A_CS_GPIO_Port, SPI_A_CS_Pin, GPIO_PIN_RESET);
						HAL_Delay(200);
						HAL_GPIO_WritePin(SPI_A_CS_GPIO_Port, SPI_A_CS_Pin, GPIO_PIN_SET);
						HAL_Delay(250);
					}
					if(thisSystem->sensors[1]->sensor_status == MAGNETOMETER_FAULTY )
					{
						HAL_GPIO_WritePin(SPI_B_CS_GPIO_Port, SPI_B_CS_Pin, GPIO_PIN_RESET);
						HAL_Delay(200);
						HAL_GPIO_WritePin(SPI_B_CS_GPIO_Port, SPI_B_CS_Pin, GPIO_PIN_SET);
						HAL_Delay(250);
					}
					if(thisSystem->sensors[2]->sensor_status == MAGNETOMETER_FAULTY )
					{
						HAL_GPIO_WritePin(SPI_C_CS_GPIO_Port, SPI_C_CS_Pin, GPIO_PIN_RESET);
						HAL_Delay(200);
						HAL_GPIO_WritePin(SPI_C_CS_GPIO_Port, SPI_C_CS_Pin, GPIO_PIN_SET);
						HAL_Delay(250);
					}
					//after this point we can not use HAL Delay function we disable that because systick interrupt make unpredictable timing for time sensitive procedures
					HAL_SuspendTick();
					break;
				}
				//-------------------------------------------
				case I2C_PACKET_BEGIN_MAG_CONVERSION:
				{
					b_read_permit =1;
					break;
				}
				//-------------------------------
				case I2C_SET_SENSOR1_REGISTER:
				{
					magnetometer_direct_register_write(thisSystem->sensors[0]->magnetometer,(uint8_t)thisSystem->i2c_line->receiveBuffer[1],(uint8_t)thisSystem->i2c_line->receiveBuffer[2]);
					break;
				}
				//-------------------------------
				case I2C_SET_SENSOR2_REGISTER:
				{
					magnetometer_direct_register_write(thisSystem->sensors[1]->magnetometer,(uint8_t)thisSystem->i2c_line->receiveBuffer[1],(uint8_t)thisSystem->i2c_line->receiveBuffer[2]);
					break;
				}
				//-------------------------------
				case I2C_SET_SENSOR3_REGISTER:
				{
					magnetometer_direct_register_write(thisSystem->sensors[2]->magnetometer,(uint8_t)thisSystem->i2c_line->receiveBuffer[1],(uint8_t)thisSystem->i2c_line->receiveBuffer[2]);
					break;
				}
				//-------------------------------
				case I2C_SET_SENSORS_REGISTER:
				{
					magnetometer_direct_register_write(thisSystem->sensors[0]->magnetometer,(uint8_t)thisSystem->i2c_line->receiveBuffer[1],(uint8_t)thisSystem->i2c_line->receiveBuffer[2]);
					magnetometer_direct_register_write(thisSystem->sensors[1]->magnetometer,(uint8_t)thisSystem->i2c_line->receiveBuffer[1],(uint8_t)thisSystem->i2c_line->receiveBuffer[2]);
					magnetometer_direct_register_write(thisSystem->sensors[2]->magnetometer,(uint8_t)thisSystem->i2c_line->receiveBuffer[1],(uint8_t)thisSystem->i2c_line->receiveBuffer[2]);
					break;
				}
			}
			//-------- if we get any data higher than 0x80 in the first byte it mean it is a new address
			if ( thisSystem->i2c_line->receiveBuffer[0] > I2C_PACKET_SET_NEW_ADDRESS )
			{
				__HAL_I2C_DISABLE_IT(thisSystem->i2c_line->I2C_line, I2C_IT_RXI | I2C_IT_STOPI | I2C_IT_ADDRI);
				uint8_t i2c_new_address =  (uint8_t)thisSystem->i2c_line->receiveBuffer[0] & 0x7f;
				thisSystem->i2c_line->I2C_line->Instance->OAR1 &= ~I2C_OAR1_OA1EN;
				thisSystem->i2c_line->I2C_line->Instance->OAR1 = (I2C_OAR1_OA1EN | ( i2c_new_address << 1) );
				__HAL_I2C_ENABLE_IT(thisSystem->i2c_line->I2C_line, I2C_IT_RXI | I2C_IT_STOPI | I2C_IT_ADDRI);
			}
			thisSystem->i2c_line->buffer_index =0;
			thisSystem->i2c_line->new_command_is_ready_flag = 0;
		}

	}
}
