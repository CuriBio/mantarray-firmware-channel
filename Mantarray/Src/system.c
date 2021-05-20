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
	my_sys.data_bus = internal_bus_create(GPIOB, 0xFF,
										  BUS_CLK_GPIO_Port, BUS_CLK_Pin,
										  BUS_C0_GPIO_Port, BUS_C0_Pin);



	//GlobalTimerInit(&thisSystem->GlobalTimer);

	uint8_t temp_data[1];
	uint8_t i2c_new_address[1];

	EEPROM_load(EEPROM_FIRST_TIME_INITIATION, temp_data, 1);  //TODO  this is bungee jumping without rope we assume everything if good no error check
	if (temp_data[0] == EEPROM_FIRST_TIME_BOOT_MARKE )
	{
		i2c_new_address[0] = my_sys.i2c_line->receiveBuffer[0] && 0xef;
		EEPROM_load(EEPROM_I2C_ADDR, i2c_new_address, 1);
		my_sys.i2c_line = I2C_interface_create(&hi2c2,i2c_new_address[0]);
	}
	else
	{
		my_sys.i2c_line = I2C_interface_create(&hi2c2,100 );   //TDOD hard code this to correct default value
	}
	return;
}



void state_machine(System *thisSystem)
{
	uint8_t testData[23] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22};
	uint32_t temp = 0;
	while(1)
	{
		if(my_sys.i2c_line->buffer_index)
		{
			switch(my_sys.i2c_line->receiveBuffer[0])
			{
				//-------------------------------
				case I2C_PACKET_SEND_DATA_FRAME:
				{
					internal_bus_write_data_frame(my_sys.data_bus, testData, 10);
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
				HAL_GPIO_WritePin(CHN_OUT_BT0_GPIO_Port, CHN_OUT_BT0_Pin, GPIO_PIN_RESET);
				uint8_t i2c_new_address[1];
				uint8_t temp_data[1];
				i2c_new_address[0] = my_sys.i2c_line->receiveBuffer[0] && 0xef;
				if( !EEPROM_save(EEPROM_I2C_ADDR, i2c_new_address, 1) )
				{
					//TODO we  failed to load what should we do now?
					//this is bad we can kill the whole system master micro should now about this
					//we donot have any valid address for now we go to idle mode we never activate common bus
				}
				else
				{
					temp_data[0] = EEPROM_FIRST_TIME_BOOT_MARKE;
					EEPROM_save(EEPROM_FIRST_TIME_INITIATION, temp_data, 1);  //TODO  this is bungee jumping without rope we assume everything if good no error check
				}
			}
		my_sys.i2c_line->buffer_index =0;
		}
	}
}
