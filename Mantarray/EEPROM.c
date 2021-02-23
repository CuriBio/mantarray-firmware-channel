#include <string.h>
#include <stdio.h>
#include "main.h"
#include "EEPROM.h"

extern UART_HandleTypeDef huart2;

void EEPROMTest(uint32_t addressI2C, uint16_t test1, uint8_t test2)
{
	uint8_t buffer[100];
	size_t len = sprintf(buffer, "%u %u %u \r\n", *(uint32_t*) I2C_ADDR, *(uint16_t*) OTHERINFO1, *(uint8_t*) OTHERINFO2);
	HAL_UART_Transmit(&huart2, buffer, len, 100);
	//Call unlock before programming operations to ensure that you have control of the EEPROM memory
	HAL_FLASHEx_DATAEEPROM_Unlock();
	//Check if the address you are trying to write to is in fact within the EEPROM memory map
	if (IS_FLASH_DATA_ADDRESS(I2C_ADDR))
	{
		//Program the set of data
		HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, (uint32_t) I2C_ADDR, addressI2C);
	}
	if (IS_FLASH_DATA_ADDRESS(OTHERINFO1))
	{
		HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_HALFWORD, (uint32_t) OTHERINFO1, (uint32_t) test1);
	}
	if (IS_FLASH_DATA_ADDRESS(OTHERINFO2))
	{
		HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, (uint32_t) OTHERINFO2, (uint32_t) test2);
	}
	HAL_FLASHEx_DATAEEPROM_Lock();
	//Lock the EEPROM afterwards to protect it from accidental memory writes

	len = sprintf(buffer, "%u %u %u \r\n", *(uint32_t*) I2C_ADDR, *(uint16_t*) OTHERINFO1, *(uint8_t*) OTHERINFO2);
	HAL_UART_Transmit(&huart2, buffer, len, 100);
}
