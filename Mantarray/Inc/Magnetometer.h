#ifndef MAGNETOMETER_H_
#define MAGNETOMETER_H_

#include <lis3mdl_driver.h>
#include <string.h>
#include <stdio.h>
#include "mmc5983_driver.h"

#define MAGNETOMETER_TYPE_LIS3MDL	10
#define MAGNETOMETER_TYPE_MMC5983	20

typedef struct
{
	uint8_t whichMagnetometer; //MAGNETOMETER_TYPE_LIS3MDL   or   MAGNETOMETER_TYPE_MMC5983

	void *magnetometer;
	uint16_t sampleRate;
	uint16_t sensorConfig;

	uint32_t timeStamp;
	uint16_t XReadings;
	uint16_t YReadings;
	uint16_t ZReadings;

}Magnetometer_t;
//thses two function should be impliment with polymorphism
Magnetometer_t * magnetometer_create(uint8_t,SPI_HandleTypeDef *,GPIO_TypeDef *,uint16_t,GPIO_TypeDef *,uint16_t);
void magnetometer_destroy(Magnetometer_t *thisMagnetometer);
void magnetometer_read(Magnetometer_t *thisMagnetometer);

#endif /* MAGNETOMETER_H_ */
