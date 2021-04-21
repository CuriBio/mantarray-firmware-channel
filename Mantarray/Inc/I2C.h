#ifndef I2C_H_
#define I2C_H_

#include <string.h>
#include <stdio.h>

#define I2C_RECEIVE_LENGTH		2

typedef struct
{
	uint8_t receiveBuffer[I2C_RECEIVE_LENGTH];
} I2C_t;


void I2CInit(I2C_t *thisI2C);

#endif /* I2C_H_ */
