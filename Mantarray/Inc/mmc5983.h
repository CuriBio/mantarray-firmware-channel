#include <string.h>
#include <stdio.h>
#include "main.h"

#ifndef MMC5983_H_
#define MMC5983_H_

#define MMC5983_XOUT0		0x00
#define MMC5983_XOUT1		0x01
#define MMC5983_YOUT0		0x02
#define MMC5983_YOUT1		0x03
#define MMC5983_ZOUT0		0x04
#define MMC5983_ZOUT1		0x05
#define MMC5983_XYZOUT2		0x06
#define MMC5983_TOUT		0x07
#define MMC5983_STATUS		0x08

#define MMC5983_INTERNALCONTROL0		0x09
#define MMC5983_INTERNALCONTROL1		0x0A
#define MMC5983_INTERNALCONTROL2		0x0B
#define MMC5983_INTERNALCONTROL3		0x0C
#define MMC5983_WHOAMI					0x2F
#define MMC5983_WHO_ID_RESPONSE			0x30

#define MMC5983_WRITE			0x00
#define MMC5983_READ			0x80

#define MMC5983_MAXREADINGS		10

typedef struct{
	//int32_t magneticX[MMC5983_MAXREADINGS];
	//int32_t magneticY[MMC5983_MAXREADINGS];
	//int32_t magneticZ[MMC5983_MAXREADINGS];
	//uint8_t magneticFront;
	//int16_t temp;
	GPIO_TypeDef * CS_GPIO_Bus;
	uint16_t INT_GPIO_Pin;
	uint16_t CS_GPIO_Pin;
	uint8_t idChar;
	uint8_t idNum;


} MMC5983_t;

void init_MMC5983_struct(MMC5983_t *thisMMC5983);
uint8_t register_read_MMC5983(MMC5983_t *thisMMC5983, uint8_t thisRegister);
void register_write_MMC5983(MMC5983_t *thisMMC5983, uint8_t thisRegister, uint8_t val);


#endif /* MMC5983_H_ */
