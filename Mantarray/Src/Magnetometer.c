#include <string.h>
#include <stdio.h>
#include "system.h"
#include "EEPROM.h"

extern System my_sys;
extern SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef huart2;

void MagnetometerInit(Magnetometer_t *thisMagnetometer)
{
	thisMagnetometer->whichMagnetometer = *(uint8_t*) WHICH_MAGNETOMETER;
	thisMagnetometer->sensorConfig = 0b0000000111111111;
	thisMagnetometer->XReadings[0] = 'ba';
	thisMagnetometer->XReadings[1] = 'dc';
	thisMagnetometer->XReadings[2] = 'fe';
	thisMagnetometer->YReadings[0] = 'hg';
	thisMagnetometer->YReadings[1] = 'ji';
	thisMagnetometer->YReadings[2] = 'lk';
	thisMagnetometer->ZReadings[0] = 'nm';
	thisMagnetometer->ZReadings[1] = 'po';
	thisMagnetometer->ZReadings[2] = 'rq';
	thisMagnetometer->timeStamp = 'emit';
	thisMagnetometer->tempReading = '\r\n';

	if (thisMagnetometer->whichMagnetometer==1)
	{
		//Start lis3mdl and initialize struct
		thisMagnetometer->sensorA_LIS3MDL.CS_GPIO_Bus = GPIOA;
		thisMagnetometer->sensorA_LIS3MDL.CS_GPIO_Pin = GPIO_PIN_6;
		thisMagnetometer->sensorB_LIS3MDL.CS_GPIO_Bus = GPIOA;
		thisMagnetometer->sensorB_LIS3MDL.CS_GPIO_Pin = GPIO_PIN_7;
		thisMagnetometer->sensorC_LIS3MDL.CS_GPIO_Bus = GPIOA;
		thisMagnetometer->sensorC_LIS3MDL.CS_GPIO_Pin = GPIO_PIN_8;
		thisMagnetometer->sensorA_LIS3MDL.DRDY_GPIO_Pin = GPIO_PIN_8;
		thisMagnetometer->sensorB_LIS3MDL.DRDY_GPIO_Pin = GPIO_PIN_12;
		thisMagnetometer->sensorC_LIS3MDL.DRDY_GPIO_Pin = GPIO_PIN_4;
		thisMagnetometer->sensorA_LIS3MDL.id = 'A';
		thisMagnetometer->sensorB_LIS3MDL.id = 'B';
		thisMagnetometer->sensorC_LIS3MDL.id = 'C';
		//init_LIS3MDL_struct(&thisMagnetometer->sensorA_LIS3MDL);
		//init_LIS3MDL_struct(&thisMagnetometer->sensorB_LIS3MDL);
		//init_LIS3MDL_struct(&thisMagnetometer->sensorC_LIS3MDL);
		//thisMagnetometer->sensorA.uartBufLen = sprintf(thisMagnetometer->sensorA.uartBuffer, "SPI Config Complete\r\n");
		//serialSend(&huart2, thisMagnetometer->sensorC.uartBuffer, thisMagnetometer->sensorC.uartBufLen);
	}
	else if (thisMagnetometer->whichMagnetometer==2)
	{
		//Start mmc5983 and initialize struct
		thisMagnetometer->sensorA_MMC5983.CS_GPIO_Bus = GPIOA;
		thisMagnetometer->sensorA_MMC5983.CS_GPIO_Pin = GPIO_PIN_6;
		thisMagnetometer->sensorB_MMC5983.CS_GPIO_Bus = GPIOA;
		thisMagnetometer->sensorB_MMC5983.CS_GPIO_Pin = GPIO_PIN_7;
		thisMagnetometer->sensorA_MMC5983.INT_GPIO_Pin = GPIO_PIN_8;
		thisMagnetometer->sensorB_MMC5983.INT_GPIO_Pin = GPIO_PIN_12;
		thisMagnetometer->sensorA_MMC5983.idChar = 'A';
		thisMagnetometer->sensorB_MMC5983.idChar = 'B';
		thisMagnetometer->sensorC_MMC5983.idChar = 'C';
		thisMagnetometer->sensorA_MMC5983.idNum = 0;
		thisMagnetometer->sensorB_MMC5983.idNum = 1;
		thisMagnetometer->sensorC_MMC5983.idNum = 2;
		//init_MMC5983_struct(&thisMagnetometer->sensorA_MMC5983);
		//readMMC5983_XYZ(thisMagnetometer, &thisMagnetometer->sensorA_MMC5983);
		//init_MMC5983_struct(&thisMagnetometer->sensorB_MMC5983);
		//readMMC5983_XYZ(thisMagnetometer, &thisMagnetometer->sensorB_MMC5983);
		//init_MMC5983_struct(&thisMagnetometer->sensorC_MMC5983);
		thisMagnetometer->uartBufLen = sprintf(thisMagnetometer->uartBuffer, "SPI Config Complete\r\n");
		serialSend(&huart2, thisMagnetometer->uartBuffer, thisMagnetometer->uartBufLen);
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	/*if (GPIO_Pin==my_sys.Magnetometer.sensorA_MMC5983.INT_GPIO_Pin)
	{
		readMMC5983_XYZ(&my_sys.Magnetometer, &my_sys.Magnetometer.sensorA_MMC5983);
	}*/
	/*if (GPIO_Pin==my_sys.Magnetometer.sensorB_MMC5983.INT_GPIO_Pin)
	{
		readMMC5983_XYZ(&my_sys.Magnetometer, &my_sys.Magnetometer.sensorB_MMC5983);
	}*/
}

void readMMC5983_XYZ(Magnetometer_t *thisMagnetometer, MMC5983_t *thisMMC5983)
{
	thisMagnetometer->out[0] = MMC5983_READ | MMC5983_XOUT0;   //Doing a continuous read and starting at the first measurement register (X_Low)
	HAL_GPIO_WritePin(thisMMC5983->CS_GPIO_Bus, thisMMC5983->CS_GPIO_Pin, GPIO_PIN_RESET); //! Set CS pin low to begin SPI read on target device
	HAL_SPI_TransmitReceive(&hspi1, thisMagnetometer->out, thisMagnetometer->in, 8, 10);   //Read all the data at once
	HAL_GPIO_WritePin(thisMMC5983->CS_GPIO_Bus, thisMMC5983->CS_GPIO_Pin, GPIO_PIN_SET); //! Set CS pin high to signal SPI read as done
	register_write_MMC5983(thisMMC5983, MMC5983_STATUS, 0b00000001);
	//Concatenate low and high bits
	thisMagnetometer->XReadings[thisMMC5983->idNum] = (thisMagnetometer->in[1]<<10) | thisMagnetometer->in[2]<<2 | ((thisMagnetometer->in[7]>>6) & 3);
	thisMagnetometer->YReadings[thisMMC5983->idNum] = (thisMagnetometer->in[3]<<10) | thisMagnetometer->in[4]<<2 | ((thisMagnetometer->in[7]>>4) & 3);
	thisMagnetometer->ZReadings[thisMMC5983->idNum] = (thisMagnetometer->in[5]<<10) | thisMagnetometer->in[6]<<2 | ((thisMagnetometer->in[7]>>2) & 3);
	//thisMMC5983->magneticX[thisMMC5983->magneticFront]= (thisMMC5983->in[1]<<10) | thisMMC5983->in[2]<<2 | ((thisMMC5983->in[7]>>6) & 3);
	//thisMMC5983->magneticY[thisMMC5983->magneticFront]= (thisMMC5983->in[3]<<10) | thisMMC5983->in[4]<<2 | ((thisMMC5983->in[7]>>4) & 3);
	//thisMMC5983->magneticZ[thisMMC5983->magneticFront]= (thisMMC5983->in[5]<<10) | thisMMC5983->in[6]<<2 | ((thisMMC5983->in[7]>>2) & 3);

	//TEST CODE
	thisMagnetometer->timeStamp = getGlobalTimer(&my_sys.GlobalTimer);
	thisMagnetometer->uartBufLen = sprintf(thisMagnetometer->uartBuffer, "%c %u %u %u %u\r\n",
									  thisMMC5983->idChar,
									  thisMagnetometer->XReadings[thisMMC5983->idNum],
									  thisMagnetometer->YReadings[thisMMC5983->idNum],
									  thisMagnetometer->ZReadings[thisMMC5983->idNum],
									  thisMagnetometer->timeStamp);
	serialSend(&huart2, thisMagnetometer->uartBuffer, thisMagnetometer->uartBufLen);

	//thisMMC5983->magneticFront++;
	//if (thisMMC5983->magneticFront == MMC5983_MAXREADINGS)
	//{
	//	thisMMC5983->magneticFront = 0;
	//}
}
