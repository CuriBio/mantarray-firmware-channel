#include "mmc5983.h"
#include "system.h"
#include "main.h"
#include <string.h>
#include <stdio.h>

extern SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef huart2;

extern System my_sys;

void init_MMC5983_struct(MMC5983_t *thisMMC5983)
{
	HAL_GPIO_WritePin(thisMMC5983->CS_GPIO_Bus, thisMMC5983->CS_GPIO_Pin, GPIO_PIN_SET);   //Set CS pin on sensor A to high to ensure no SPI communication enabled initially

	//Set all of the configuration registers every time on bootup
	register_write_MMC5983(thisMMC5983, MMC5983_INTERNALCONTROL1, 0b10000000); //128: Reset chip, operation takes 10 msec
	HAL_Delay(20);
	register_write_MMC5983(thisMMC5983, MMC5983_INTERNALCONTROL0, 0b00000100);  //32: Auto set/reset   16: Reset   8: Set   4: Enable interrupt
	//HAL_Delay(15);
	register_write_MMC5983(thisMMC5983, MMC5983_INTERNALCONTROL1, 0b00000000);  //16/8: Disable Y/Z measure   4: Disable X measure   2/1: Sampling rate pt. 1
	//HAL_Delay(15);
	register_write_MMC5983(thisMMC5983, MMC5983_INTERNALCONTROL2, 0b00001110);  //128: Enable auto set   64/32/16: Set period   8: Continuous conversion mode   4/2/1: Sampling rate pt. 2
	//HAL_Delay(15);
	register_write_MMC5983(thisMMC5983, MMC5983_INTERNALCONTROL3, 0b00000000);  //64: SPI 3-wire mode   4/2: Saturation checks.
	//HAL_Delay(1000);
	//Check whether you are communicating with the ST sensor
	uint8_t SPITestWHOAMI = register_read_MMC5983(thisMMC5983, (uint8_t)MMC5983_WHOAMI);
	if (SPITestWHOAMI==MMC5983_WHO_ID_RESPONSE)
	{
		//TODO Implement Sensor found subroutine
		//thisMMC5983->uartBufLen = sprintf(thisMMC5983->uartBuffer, "Sensor %c Found!\r\n", thisMMC5983->id);
		//serialSend(&huart2, thisMMC5983->uartBuffer, thisMMC5983->uartBufLen);
	}
}

uint8_t register_read_MMC5983(MMC5983_t *thisMMC5983, uint8_t thisRegister)
{
	uint8_t result;
	uint8_t out[2];
	uint8_t in[2] = {0 , 0};
	out[0] = MMC5983_READ | thisRegister;   //adding 128 to writing 1 in MSB bit 7
	out[1]= 0;   //transfer dummy byte to get response
	HAL_GPIO_WritePin(thisMMC5983->CS_GPIO_Bus, thisMMC5983->CS_GPIO_Pin, GPIO_PIN_RESET); //! Set CS pin low to begin SPI read on target device
	HAL_SPI_TransmitReceive(&hspi1, out, in, 2, 10);
	HAL_GPIO_WritePin(thisMMC5983->CS_GPIO_Bus, thisMMC5983->CS_GPIO_Pin, GPIO_PIN_SET); //! Set CS pin high to signal SPI read as done
	result = in[1];
	return result;
}

void register_write_MMC5983(MMC5983_t *thisMMC5983, uint8_t thisRegister, uint8_t val)
{
	uint8_t out[2];
	out[0] = MMC5983_WRITE | thisRegister;
	out[1] = val;
	HAL_GPIO_WritePin(thisMMC5983->CS_GPIO_Bus, thisMMC5983->CS_GPIO_Pin, GPIO_PIN_RESET); //! Set CS pin low to begin SPI read on target device
	HAL_SPI_Transmit(&hspi1, out, 2, 10);
	HAL_GPIO_WritePin(thisMMC5983->CS_GPIO_Bus, thisMMC5983->CS_GPIO_Pin, GPIO_PIN_SET); //! Set CS pin high to signal SPI read as done

}


