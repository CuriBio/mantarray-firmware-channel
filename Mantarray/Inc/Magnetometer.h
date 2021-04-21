#ifndef MAGNETOMETER_H_
#define MAGNETOMETER_H_

#include <string.h>
#include <stdio.h>
#include "lis3mdl.h"
#include "mmc5983.h"

typedef struct{
	uint8_t whichMagnetometer; //1 = lis3mdl, 2 = mmc5983
	LIS3MDL_t sensorA_LIS3MDL;
	LIS3MDL_t sensorB_LIS3MDL;
	LIS3MDL_t sensorC_LIS3MDL;
	MMC5983_t sensorA_MMC5983;
	MMC5983_t sensorB_MMC5983;
	MMC5983_t sensorC_MMC5983;

	uint16_t sampleRate;
	uint16_t sensorConfig;

	uint8_t out[8];
	uint8_t in[8];
	uint8_t common_byte;
	uint32_t timeStamp;
	uint16_t XReadings[3];
	uint16_t YReadings[3];
	uint16_t ZReadings[3];
	uint16_t tempReading;

	//TEST CODE
	char uartBuffer[35];
	uint8_t uartBufLen;
}Magnetometer_t;

void MagnetometerInit(Magnetometer_t *thisMagnetometer);
void readMMC5983_XYZ(Magnetometer_t *thisMagnetometer, MMC5983_t *thisMMC5983);

#endif /* MAGNETOMETER_H_ */
