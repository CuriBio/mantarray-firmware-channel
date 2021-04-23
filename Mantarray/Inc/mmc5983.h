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
//---------------------------------------------------------
typedef struct
{
	//int32_t magneticX[MMC5983_MAXREADINGS];
	//int32_t magneticY[MMC5983_MAXREADINGS];
	//int32_t magneticZ[MMC5983_MAXREADINGS];
	//uint8_t magneticFront;
	//int16_t temp;
	GPIO_TypeDef * INT_GPIO_Bus;
	uint16_t INT_GPIO_Pin;
	GPIO_TypeDef * CS_GPIO_Bus;
	uint16_t CS_GPIO_Pin;
	uint8_t idChar;
	uint8_t idNum;
} MMC5983_t;

void MMC5983_register_init(MMC5983_t *thisMMC5983);
uint8_t MMC5983_register_read(MMC5983_t *thisMMC5983, uint8_t thisRegister);
void MMC5983_register_write(MMC5983_t *thisMMC5983, uint8_t thisRegister, uint8_t val);


#endif /* MMC5983_H_ */
