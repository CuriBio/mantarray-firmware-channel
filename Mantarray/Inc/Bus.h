#include <string.h>
#include <stdio.h>

#ifndef BUS_H_
#define BUS_H_

#define BUS_ACK_MODER    0b00000000000000000101010101010101
#define BUS_CACK_MODER    0b01010100000000000000000000000000
#define BUS_ACK_OSPEEDR    0b00000000000000001111111111111111
#define BUS_CACK_OSPEEDR    0b11111100000000000000000000000000
#define BUS_BUSMASK16    0b00000000000000000000000011111111
#define BUS_CBUSMASK16    0b00000000000000001110000000000000
#define BUS_BUSMASK32    0b00000000000000001111111111111111
#define BUS_CBUSMASK32    0b11111100000000000000000000000000

typedef struct {
	GPIO_TypeDef * _C_GPIO_Bus;
	GPIO_TypeDef * _GPIO_Bus;

	uint16_t _C0_GPIO_Pin;
	uint16_t _C1_GPIO_Pin;
	uint16_t _C2_GPIO_Pin;
	uint16_t _0_GPIO_Pin;
	uint16_t _1_GPIO_Pin;
	uint16_t _2_GPIO_Pin;
	uint16_t _3_GPIO_Pin;
	uint16_t _4_GPIO_Pin;
	uint16_t _5_GPIO_Pin;
	uint16_t _6_GPIO_Pin;
	uint16_t _7_GPIO_Pin;

	GPIO_InitTypeDef GPIO_InitStruct;

}Bus_t;

void BusInit(Bus_t *thisBus);
void Complete (Bus_t *thisBus);
void Acknowledge (Bus_t *thisBus);
void SendData (Bus_t *thisBus, uint8_t* data, uint8_t len);
void WriteDataFrame(Magnetometer_t * thisMagnetometer, Bus_t *thisBus);
void MockData(Magnetometer_t * thisMagnetometer);

#endif /* BUS_H_ */
