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

	EEPROM_load(EEPROM_FIRST_TIME_INITIATION, temp_data, 1);  //TODO  this is bungee jumping without rope we assume everything if good no error check
	if (temp_data[0] == EEPROM_FIRST_TIME_BOOT_MARKE )
	{
		EEPROM_load(EEPROM_I2C_ADDR, i2c_new_address, 1);

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);//green we set from eeprom  //todo test
		my_sys.i2c_line = I2C_interface_create(&hi2c2,i2c_new_address[0]);

	}
	else
	{
		my_sys.i2c_line = I2C_interface_create(&hi2c2,100 );   //TDOD hard code this to correct default value
		  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);//red on we hard coded  address//todo test
	}
	return;
}



void state_machine(System *thisSystem)
{
	uint8_t testData[23] = {3,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,1};  //TODO remove after Link data output to magnetometer memory instead

	while(1)
	{
	//internal_bus_write_data_frame(my_sys.data_bus,testData,22);HAL_Delay(500);
		if(my_sys.i2c_line->buffer_index)
		{
			switch(my_sys.i2c_line->receiveBuffer[0])
			{
				//-------------------------------
				case I2C_PACKET_SEND_DATA_FRAME:
				{

					//TODO Link data output to magnetometer memory instead
					internal_bus_write_data_frame(my_sys.data_bus,testData,22);
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
					  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
					break;
				}
				//-------------------------------
				case I2C_PACKET_SET_RED_OFF:
				{
					  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
					break;
				}
				//-------------------------------
				case I2C_PACKET_SET_GREEN_ON:
				{
					  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
					break;
				}
				//-------------------------------
				case I2C_PACKET_SET_GREEN_OFF:
				{
					  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
					break;
				}
				//-------------------------------
				case I2C_PACKET_SET_BLUE_ON:
				{
					  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
					break;
				}
				//-------------------------------
				case I2C_PACKET_SET_BLUE_OFF:
				{
					  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
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
					temp_data[0] = EEPROM_FIRST_TIME_BOOT_MARKE;
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
