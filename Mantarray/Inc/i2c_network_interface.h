#ifndef I2C_H_
#define I2C_H_

#include "stm32l0xx_hal.h"
#include <stdlib.h>
#include <stdio.h>

#define I2C_MAX_RECEIVE_LENGTH				31

//Commands between 1 and 99 take a single bytes
#define I2C_PACKET_SEND_DATA_FRAME        10
#define I2C_PACKET_SET_BOOT0_LOW          11
#define I2C_PACKET_SET_BOOT0_HIGH         12
#define I2C_PACKET_SET_RESET_LOW          13
#define I2C_PACKET_SET_RESET_HIGH         14
#define I2C_PACKET_SET_RED_ON             15
#define I2C_PACKET_SET_RED_OFF            16
#define I2C_PACKET_SET_GREEN_ON           17
#define I2C_PACKET_SET_GREEN_OFF          18
#define I2C_PACKET_SET_BLUE_ON            19
#define I2C_PACKET_SET_BLUE_OFF           20
#define I2C_PACKET_RESET_GLOBAL_TIMER     29
#define I2C_PACKET_BEGIN_MAG_CONVERSION   30
#define I2C_PACKET_SENSOR_TEST_ROUTINE    31
//Commands between 100 and 109 take three bytes
#define I2C_3_BYTE_COMMAND_START          100
#define I2C_SET_SENSORS_REGISTER          101
#define I2C_SET_SENSOR1_REGISTER          102
#define I2C_SET_SENSOR2_REGISTER          103
#define I2C_SET_SENSOR3_REGISTER          104
//Commands between 110 and 127 take 31 bytes
#define I2C_31_BYTE_COMMAND_START         110
//Commands above 128 are a set address command
#define I2C_PACKET_SET_NEW_ADDRESS        128

typedef struct
{
	volatile uint8_t buffer_index;
	volatile uint8_t new_command_is_ready_flag;
	volatile uint8_t receiveBuffer[I2C_MAX_RECEIVE_LENGTH];
	I2C_HandleTypeDef *I2C_line;
} I2C_t;

I2C_t * I2C_interface_create(I2C_HandleTypeDef *I2C_line,uint8_t channel_address);

#endif /* I2C_H_ */
