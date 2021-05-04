//this is the driver layer to connect with specific chip
#include <string.h>
#include <stdio.h>
#include "main.h"

#ifndef MMC5983_H_
#define MMC5983_H_
//---------------------------------------------------------
#define	MMC5983_MAXIMUM_NO_OF_MPU_REGISTERS		14
#define	MMC5983_XOUT0							0x00 		//Xout [17:10]
#define MMC5983_XOUT1							0x01 		//Xout [9:2]
#define MMC5983_YOUT0							0x02 		//Yout [17:10]
#define MMC5983_YOUT1							0x03 		//Yout [9:2]
#define MMC5983_ZOUT0							0x04 		//Zout [17:10]
#define MMC5983_ZOUT1							0x05 		//Zout [9:2]
#define MMC5983_XYZOUT2							0x06 		//Xout[1:0], Yout[1:0], Zout[1:0]
#define MMC5983_TOUT							0x07 		//Temperature output
#define MMC5983_STATUS							0x08 		//Device status

#define MMC5983_INTERNALCONTROL0				0x09 		//Control register 0
#define MMC5983_INTERNALCONTROL1				0x0A 		//Control register 1
#define MMC5983_INTERNALCONTROL2				0x0B		//Control register 2
#define MMC5983_INTERNALCONTROL3				0x0C 		//Control register 3
#define MMC5983_WHOAMI							0x2F 		//Product ID
#define MMC5983_WHO_ID_RESPONSE					0x30

#define MMC5983_WRITE							0x00
#define MMC5983_READ							0x80

#define MMC5983_MAXREADINGS						10

#define MMC5983_SENSOR_FOUND					0x20
#define MMC5983_SENSOR_NOT_FOUND				0x30
//---------------------------------------------------------
//anything specific for this chip driver layer will be here this class driven from magnetometer
typedef struct
{
	uint8_t sensor_status;
	SPI_HandleTypeDef *spi_channel;

	GPIO_TypeDef * INT_GPIO_Bus;
	uint16_t INT_GPIO_Pin;

	GPIO_TypeDef * CS_GPIO_Bus;
	uint16_t CS_GPIO_Pin;
} MMC5983_t;


MMC5983_t * MMC5983_create(SPI_HandleTypeDef *,GPIO_TypeDef *,uint16_t,GPIO_TypeDef *,uint16_t);
void MMC5983_destroy(MMC5983_t *);
uint8_t MMC5983_register_read(MMC5983_t *, uint8_t );
void MMC5983_register_write(MMC5983_t *, uint8_t, uint8_t);
void MMC5983_read_XYZ(MMC5983_t *);

#endif /* MMC5983_H_ */
