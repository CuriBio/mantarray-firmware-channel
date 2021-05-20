#include <stdlib.h>
#include <stdio.h>
#include "main.h"

#ifndef BUS_H_
#define BUS_H_

//todo we do not know about masks before create remove after removing hardcodes
#define BUS_ACK_MODER    0b00000000000000000101010101010101
#define BUS_CACK_MODER    0b01000000000000000000000000000000
#define BUS_ACK_OSPEEDR    0b00000000000000001111111111111111
#define BUS_CACK_OSPEEDR    0b11000000000000000000000000000000
#define BUS_BUSMASK16    0b00000000000000000000000011111111
#define BUS_CBUSMASK16    0b00000000000000001000000000000000
#define BUS_BUSMASK32    0b00000000000000001111111111111111
#define BUS_CBUSMASK32    0b11000000000000000000000000000000

typedef struct
{
	GPIO_TypeDef * bus;
	uint16_t bus_mask;

	GPIO_TypeDef * bus_clk;
	uint16_t bus_clk_mask;

	GPIO_TypeDef * bus_ack;
	uint16_t bus_ack_mask;

	GPIO_InitTypeDef GPIO_InitStruct;  //todo what is the usage of this?

}InternalBus_t;

InternalBus_t * internal_bus_create(GPIO_TypeDef *bus_line,uint16_t bus_pins,GPIO_TypeDef *cl_bus,uint16_t cl_pin,GPIO_TypeDef *ak_bus,uint16_t ak_pin);
void internal_bus_write_data_frame(InternalBus_t *thisInternalBus,uint8_t *buffer,uint8_t buffer_len);
void internal_bus_utilize(InternalBus_t *thisInternalBus);
void internal_bus_release(InternalBus_t *thisInternalBus);

#endif /* BUS_H_ */
