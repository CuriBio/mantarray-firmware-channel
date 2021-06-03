#include "system.h"

extern SPI_HandleTypeDef hspi1;
extern I2C_HandleTypeDef hi2c2;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim21;
extern TIM_HandleTypeDef htim22;
extern System my_sys;

void module_system_init(System *thisSystem)
{
	my_sys.data_bus = internal_bus_create(GPIOB,  BUS0_Pin | BUS1_Pin | BUS2_Pin | BUS3_Pin | BUS4_Pin | BUS5_Pin | BUS6_Pin | BUS7_Pin,
											BUS_CLK_GPIO_Port, BUS_CLK_Pin,
											BUS_C1_GPIO_Port, BUS_C1_Pin);

	//GlobalTimerInit(&thisSystem->GlobalTimer);

	uint8_t temp_data[4]={0,0,0,0};
	uint8_t i2c_new_address[4]={0,0,0,0};

	HAL_GPIO_WritePin(SPI_A_CS_GPIO_Port, SPI_A_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SPI_B_CS_GPIO_Port, SPI_B_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SPI_C_CS_GPIO_Port, SPI_C_CS_Pin, GPIO_PIN_SET);
	EEPROM_load(EEPROM_FIRST_TIME_INITIATION, temp_data, 1);  //TODO  this is bungee jumping without rope we assume everything if good no error check
	if (temp_data[0] == EEPROM_FIRST_TIME_BOOT_MARKER )
	{
		EEPROM_load(EEPROM_I2C_ADDR, i2c_new_address, 1);
		my_sys.i2c_line = I2C_interface_create(&hi2c2,i2c_new_address[0]);
	}
	else
	{
		my_sys.i2c_line = I2C_interface_create(&hi2c2,100);   //TDOD hard code this to correct default value
	}
	// init sensors
	my_sys.sensors[0] = magnetometer_create(MAGNETOMETER_TYPE_MMC5983,&hspi1 , SPI_A_CS_GPIO_Port , SPI_A_CS_Pin , mag_int_a_GPIO_Port , mag_int_a_Pin);
	my_sys.sensors[1] = magnetometer_create(MAGNETOMETER_TYPE_MMC5983,&hspi1 , SPI_B_CS_GPIO_Port , SPI_B_CS_Pin , mag_int_b_GPIO_Port , mag_int_b_Pin);
	my_sys.sensors[2] = magnetometer_create(MAGNETOMETER_TYPE_MMC5983,&hspi1 , SPI_C_CS_GPIO_Port , SPI_C_CS_Pin , mag_int_c_GPIO_Port , mag_int_c_Pin);

	return;
}

void state_machine(System *thisSystem)
{
	uint8_t testData[40];// = {255,0,0,100,0,1,0,2,0,3,0,0,0,200,0,4,0,5,0,6,0,0,0,30,0,7,0,8,0,9,255};  //TODO remove after Link data output to magnetometer memory instead
	int read_permit =0;
	while(1)
	{
		if(read_permit)
		{
			if( my_sys.sensors[0]->sensor_status == MAGNETOMETER_OK )
				if(magnetometer_read(my_sys.sensors[0]))
				{
					(*(uint32_t *)(testData + 0))++;   //uint_40   from 0-4    only 4 byte used for test
					testData[5] = *((uint8_t*)my_sys.sensors[0]->Readings + 0);
					testData[6] = *((uint8_t*)my_sys.sensors[0]->Readings + 1);
					testData[7] = *((uint8_t*)my_sys.sensors[0]->Readings + 2);
					testData[8] = *((uint8_t*)my_sys.sensors[0]->Readings + 3);
					testData[9] = *((uint8_t*)my_sys.sensors[0]->Readings + 4);
					testData[10] = *((uint8_t*)my_sys.sensors[0]->Readings + 5);
				}
			//---------------
			if( my_sys.sensors[1]->sensor_status == MAGNETOMETER_OK )
				if(magnetometer_read(my_sys.sensors[1]))
				{
					(*(uint32_t *)(testData + 11))++;   //uint_40   from 11-15    only 4 byte used for test
					testData[16] = *((uint8_t*)my_sys.sensors[1]->Readings + 0);
					testData[17] = *((uint8_t*)my_sys.sensors[1]->Readings + 1);
					testData[18] = *((uint8_t*)my_sys.sensors[1]->Readings + 2);
					testData[19] = *((uint8_t*)my_sys.sensors[1]->Readings + 3);
					testData[20] = *((uint8_t*)my_sys.sensors[1]->Readings + 4);
					testData[21] = *((uint8_t*)my_sys.sensors[1]->Readings + 5);
				}
			//-------------
			if( my_sys.sensors[2]->sensor_status == MAGNETOMETER_OK )
				if(magnetometer_read(my_sys.sensors[2]))
				{
					(*(uint32_t *)(testData + 22))++;   //uint_40   from 22-26    only 4 byte used for test
					testData[27] = *((uint8_t*)my_sys.sensors[2]->Readings + 0);
					testData[28] = *((uint8_t*)my_sys.sensors[2]->Readings + 1);
					testData[29] = *((uint8_t*)my_sys.sensors[2]->Readings + 2);
					testData[30] = *((uint8_t*)my_sys.sensors[2]->Readings + 3);
					testData[31] = *((uint8_t*)my_sys.sensors[2]->Readings + 4);
					testData[32] = *((uint8_t*)my_sys.sensors[2]->Readings + 5);
				}
		}
		//------------------------------------------
		if(my_sys.i2c_line->buffer_index)
		{
			switch(my_sys.i2c_line->receiveBuffer[0])
			{
				//-------------------------------
				case I2C_PACKET_SEND_DATA_FRAME:
				{

					//TODO Link data output to magnetometer memory instead
					internal_bus_write_data_frame(my_sys.data_bus,testData,33);
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
				//----------test cases---------------------
				case 31:
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
				}
				break;
				case 30://TODO remove just test
				{
					read_permit =1;
					break;
				}
			}
			//-------- if we get any data higher than 0x80  it mean it is a new address
			if ( my_sys.i2c_line->receiveBuffer[0] > I2C_PACKET_SET_NEW_ADDRESS )
			{
				uint8_t i2c_new_address[4]={3,3,3,3};
				uint8_t temp_data[4]={0,0,0,0};
				i2c_new_address[0] =  (uint8_t)my_sys.i2c_line->receiveBuffer[0] & 0x7f;
				if( !EEPROM_save(EEPROM_I2C_ADDR, i2c_new_address, 1) )
				{
					//TODO we  failed to save what should we do now?
					//this is bad we can kill the whole system master micro should now about this
					//we donot have any valid address for now we go to idle mode we never activate common bus

				}
				else
				{
					temp_data[0] = EEPROM_FIRST_TIME_BOOT_MARKER;
					if( !EEPROM_save(EEPROM_FIRST_TIME_INITIATION, temp_data,1) )  //TODO  this is bungee jumping without rope we assume everything if good no error check
					{
						//TODO we failed to saved
						//this is bad we can kill the whole system master micro should now about this
						//we donot have any valid address for now we go to idle mode we never activate common bus
					}
				}
			}
		my_sys.i2c_line->buffer_index =0;
		}
	}

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
