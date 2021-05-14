#ifndef I2C_H_
#define I2C_H_

#include "stm32l0xx_hal.h"
#include <stdlib.h>
#include <stdio.h>

#define I2C_RECEIVE_LENGTH				1

#define I2C_PACKET_SET_NEW_ADDRESS		128
#define I2C_PACKET_SEND_DATA_FRAME		10
#define I2C_PACKET_SET_BOOT0_LOW		11
#define I2C_PACKET_SET_BOOT0_HIGH		12
#define I2C_PACKET_SET_RESET_LOW		13
#define I2C_PACKET_SET_RESET_HIGH		14

typedef struct
{
	uint8_t buffer_index;
	uint8_t receiveBuffer[I2C_RECEIVE_LENGTH];
	I2C_HandleTypeDef *I2C_line;
} I2C_t;

I2C_t * i2c2_interrupt_interface_pointer;   //TODO better implementation is possible by modifying weak I2C*_IRQHandler and make it part of the I2C_t
I2C_t * I2C_interface_create(I2C_HandleTypeDef *I2C_line,uint8_t channel_address);

#endif /* I2C_H_ */
