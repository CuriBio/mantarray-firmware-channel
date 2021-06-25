#include "system.h"

extern SPI_HandleTypeDef hspi1;
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
			if(my_sys.i2c_line->receiveBuffer[0] < I2C_SET_MULTIPLE_BYTE_COMMAND)
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

//----------------------------------
void module_system_init(System *thisSystem)
{
	my_sys.data_bus = internal_bus_create(GPIOB,  BUS0_Pin | BUS1_Pin | BUS2_Pin | BUS3_Pin | BUS4_Pin | BUS5_Pin | BUS6_Pin | BUS7_Pin,
											BUS_CLK_GPIO_Port, BUS_CLK_Pin,
											BUS_C1_GPIO_Port, BUS_C1_Pin);

	thisSystem->ph_global_timer = global_timer_create(&htim21);

	HAL_GPIO_WritePin(SPI_A_CS_GPIO_Port, SPI_A_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SPI_B_CS_GPIO_Port, SPI_B_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SPI_C_CS_GPIO_Port, SPI_C_CS_Pin, GPIO_PIN_SET);

	my_sys.i2c_line = I2C_interface_create(&hi2c2,100);

	// init sensors
	my_sys.sensors[0] = magnetometer_create(MAGNETOMETER_TYPE_MMC5983,&hspi1 , SPI_A_CS_GPIO_Port , SPI_A_CS_Pin , mag_int_a_GPIO_Port , mag_int_a_Pin);
	my_sys.sensors[1] = magnetometer_create(MAGNETOMETER_TYPE_MMC5983,&hspi1 , SPI_B_CS_GPIO_Port , SPI_B_CS_Pin , mag_int_b_GPIO_Port , mag_int_b_Pin);
	my_sys.sensors[2] = magnetometer_create(MAGNETOMETER_TYPE_MMC5983,&hspi1 , SPI_C_CS_GPIO_Port , SPI_C_CS_Pin , mag_int_c_GPIO_Port , mag_int_c_Pin);

	return;
}

void state_machine(System *thisSystem)
{
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
							my_sys.bus_output_buffer[byte_shifter + sensor_num * 11] = (uint32_t)(thisSystem->data_bus->bus_mask & this_byte)  | ((thisSystem->data_bus->bus_mask & ~this_byte)  << 16);
							byte_shifter++;
						}

						while (byte_shifter < 11)
						{
							//output_data[byte_shifter + sensor_num * 11] = *(((uint8_t*)thisSystem->sensors[sensor_num]->Readings) + (byte_shifter - 5));
							this_byte = *(((uint8_t*)thisSystem->sensors[sensor_num]->Readings) + (byte_shifter - 5));
							my_sys.bus_output_buffer[byte_shifter + sensor_num * 11] = (uint32_t)(thisSystem->data_bus->bus_mask & this_byte)  | ((thisSystem->data_bus->bus_mask & ~this_byte)  << 16);
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
				} //Check if magnetometer is functional and if new data is needed
			} //Sensor loop
			b_read_permit =0;
		}
		//------------------------------------------
		if(my_sys.i2c_line->new_command_is_ready_flag)
		{
			switch(my_sys.i2c_line->receiveBuffer[0])
			{
				//-------------------------------
				case I2C_PACKET_SEND_DATA_FRAME:
				{
					my_sys.sensors[0]->b_new_data_needed = 1;
					my_sys.sensors[1]->b_new_data_needed = 1;
					my_sys.sensors[2]->b_new_data_needed = 1;
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
					if(my_sys.sensors[0]->sensor_status == MAGNETOMETER_FAULTY )
					{
						HAL_GPIO_WritePin(SPI_A_CS_GPIO_Port, SPI_A_CS_Pin, GPIO_PIN_RESET);
						HAL_Delay(200);
						HAL_GPIO_WritePin(SPI_A_CS_GPIO_Port, SPI_A_CS_Pin, GPIO_PIN_SET);
						HAL_Delay(250);
					}
					if(my_sys.sensors[1]->sensor_status == MAGNETOMETER_FAULTY )
					{
						HAL_GPIO_WritePin(SPI_B_CS_GPIO_Port, SPI_B_CS_Pin, GPIO_PIN_RESET);
						HAL_Delay(200);
						HAL_GPIO_WritePin(SPI_B_CS_GPIO_Port, SPI_B_CS_Pin, GPIO_PIN_SET);
						HAL_Delay(250);
					}
					if(my_sys.sensors[2]->sensor_status == MAGNETOMETER_FAULTY )
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
					magnetometer_direct_register_write(thisSystem->sensors[0]->magnetometer,(uint8_t)my_sys.i2c_line->receiveBuffer[1],(uint8_t)my_sys.i2c_line->receiveBuffer[2]);
					break;
				}
				//-------------------------------
				case I2C_SET_SENSOR2_REGISTER:
				{
					magnetometer_direct_register_write(thisSystem->sensors[1]->magnetometer,(uint8_t)my_sys.i2c_line->receiveBuffer[1],(uint8_t)my_sys.i2c_line->receiveBuffer[2]);
					break;
				}
				//-------------------------------
				case I2C_SET_SENSOR3_REGISTER:
				{
					magnetometer_direct_register_write(thisSystem->sensors[2]->magnetometer,(uint8_t)my_sys.i2c_line->receiveBuffer[1],(uint8_t)my_sys.i2c_line->receiveBuffer[2]);
					break;
				}
				//-------------------------------
				case I2C_SET_SENSORS_REGISTER:
				{
					magnetometer_direct_register_write(thisSystem->sensors[0]->magnetometer,(uint8_t)my_sys.i2c_line->receiveBuffer[1],(uint8_t)my_sys.i2c_line->receiveBuffer[2]);
					magnetometer_direct_register_write(thisSystem->sensors[1]->magnetometer,(uint8_t)my_sys.i2c_line->receiveBuffer[1],(uint8_t)my_sys.i2c_line->receiveBuffer[2]);
					magnetometer_direct_register_write(thisSystem->sensors[2]->magnetometer,(uint8_t)my_sys.i2c_line->receiveBuffer[1],(uint8_t)my_sys.i2c_line->receiveBuffer[2]);
					break;
				}
			}
			//-------- if we get any data higher than 0x80 in the first byte it mean it is a new address
			if ( thisSystem->i2c_line->receiveBuffer[0] > I2C_PACKET_SET_NEW_ADDRESS )
			{
				__HAL_I2C_DISABLE_IT(thisSystem->i2c_line->I2C_line, I2C_IT_RXI | I2C_IT_STOPI | I2C_IT_ADDRI);
				uint8_t i2c_new_address =  (uint8_t)my_sys.i2c_line->receiveBuffer[0] & 0x7f;
				thisSystem->i2c_line->I2C_line->Instance->OAR1 &= ~I2C_OAR1_OA1EN;
				thisSystem->i2c_line->I2C_line->Instance->OAR1 = (I2C_OAR1_OA1EN | ( i2c_new_address << 1) );
				__HAL_I2C_ENABLE_IT(thisSystem->i2c_line->I2C_line, I2C_IT_RXI | I2C_IT_STOPI | I2C_IT_ADDRI);
			}
		my_sys.i2c_line->buffer_index =0;
		my_sys.i2c_line->new_command_is_ready_flag = 0;
		}
	}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		switch(thisSystem->state)
		{
			case MODULE_SYSTEM_STATUS_START:
				//Check if system has undergone first time setup by looking for a 32-bit random number in EEPROM
				//IF A NEW FIRST TIME SETUP IS DESIRED TO BE RUN just change the value of EEPROM_FIRST_TIME_COMPLETE in <EEPROM.h>
				//uint32_t test = *(uint32_t*) FIRST_TIME_INITIATION;
				//thisSystem->state = (*(uint32_t*) FIRST_TIME_INITIATION != EEPROM_FIRST_TIME_COMPLETE) ?						MODULE_SYSTEM_STATUS_FIRST_TIME :						MODULE_SYSTEM_STATUS_INITIATION;
			break;
			//-----------
			case MODULE_SYSTEM_STATUS_FIRST_TIME:
				//EEPROMInit(thisSystem);
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
					//MockData(&thisSystem->Magnetometer);
					//WriteDataFrame(&thisSystem->Magnetometer, &thisSystem->Bus) ;
					//GPIOC->BSRR = GPIO_PIN_0;
					//GPIOC->BRR = GPIO_PIN_0;
					//Set all bus pins to low and send complete
					///thisBus->_GPIO_Bus->BRR = (uint32_t) (0x000000FF);
					//GPIOA->BRR = GPIO_PIN_15;
					//GPIOA->BRR = (uint32_t) (0x00002000);
					//Set Bus pins to input
					//temp = GPIOB->MODER;
					///temp &= ~BUS_BUSMASK32;
					//GPIOB->MODER = temp;
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
