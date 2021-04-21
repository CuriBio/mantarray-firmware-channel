#include <string.h>
#include <stdio.h>
#include "main.h"

#ifndef LIS3MDL_H_
#define LIS3MDL_H_

#define LIS3MDL_WHO_AM_I 		0x0F
#define LIS3MDL_WHO_ID_RESPONSE  0x3D
#define LIS3MDL_STATUS_REG 		0x27

#define LIS3MDL_OUT_X_L 			0x28
#define LIS3MDL_OUT_X_H 			0x29
#define LIS3MDL_OUT_Y_L 			0x2A
#define LIS3MDL_OUT_Y_H 			0x2B
#define LIS3MDL_OUT_Z_L 			0x2C
#define LIS3MDL_OUT_Z_H			0x2D
#define LIS3MDL_TEMP_OUT_L 		0x2E
#define LIS3MDL_TEMP_OUT_H 		0x2f

#define LIS3MDL_READ_SINGLEBIT 	0x80
#define LIS3MDL_READ_CONT 		0xC0
#define LIS3MDL_WRITE_SINGLEBIT 	0x00
#define LIS3MDL_WRITE_CONT 		0x40

#define LIS3MDL_CTRL_REG1 		0x20
#define LIS3MDL_CTRL_REG2 		0x21
#define LIS3MDL_CTRL_REG3 		0x22
#define LIS3MDL_CTRL_REG4 		0x23
#define LIS3MDL_CTRL_REG5 		0x24

#define LIS3MDL_MAXREADINGS		10

typedef struct{
	int16_t magneticX[LIS3MDL_MAXREADINGS];
	int16_t magneticY[LIS3MDL_MAXREADINGS];
	int16_t magneticZ[LIS3MDL_MAXREADINGS];
	uint8_t magneticFront;
	GPIO_TypeDef * CS_GPIO_Bus;
	uint16_t DRDY_GPIO_Pin;
	uint16_t CS_GPIO_Pin;
	uint8_t id;
	uint64_t timestamp;

	uint8_t out[7];
	uint8_t in[7];

	int16_t temp;

	//TEST CODE
	char uartBuffer[30];
	uint8_t uartBufLen;
} LIS3MDL_t;

void init_LIS3MDL_struct(LIS3MDL_t *thisLIS3MDL);

uint8_t register_read(LIS3MDL_t *thisLIS3MDL, uint8_t thisRegister);

void register_write(LIS3MDL_t *thisLIS3MDL, uint8_t thisRegister, uint8_t val);

void readLIS3MDL_XYZ(LIS3MDL_t *thisLIS3MDL);

#endif /* LIS3MDL_H_ */
