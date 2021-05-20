#include "Bus.h"

InternalBus_t * internal_bus_create(GPIO_TypeDef *bus_line,uint16_t bus_pins,GPIO_TypeDef *cl_bus,uint16_t cl_pin,GPIO_TypeDef *ak_bus,uint16_t ak_pin)
{
	InternalBus_t * thisInternalBus = (InternalBus_t *) malloc(sizeof(InternalBus_t));
	if(thisInternalBus != NULL)
	{
		//assign desired value for clock pin and other bus  so this bus will now which pins assigned for clock ack and bus line it self
		//everything else in this lib should use this data for other settings
		thisInternalBus->bus = bus_line;
		thisInternalBus->bus_mask = bus_pins;

		thisInternalBus->bus_clk = cl_bus;
		thisInternalBus->bus_ack_mask = cl_pin;

		thisInternalBus->bus_ack = ak_bus;
		thisInternalBus->bus_ack_mask = ak_pin;

		uint32_t temp = 0;
		//Set main bus output speed to very high
		temp = thisInternalBus->bus->OSPEEDR;
		temp &= ~thisInternalBus->bus_mask;
		temp |= BUS_ACK_OSPEEDR;//TODO  this should not be hard code
		thisInternalBus->bus->OSPEEDR = temp;
		//Set main bus output type to output push-pull
		temp = thisInternalBus->bus->OTYPER;
		temp &= ~thisInternalBus->bus_mask;
		thisInternalBus->bus->OTYPER = temp;
		//Set main bus pullup/down resistors to none
		temp = thisInternalBus->bus->PUPDR;
		temp &= ~BUS_BUSMASK32;//TODO  this should not be hard code
		thisInternalBus->bus->PUPDR = temp;




		//Set Clock line, output speed to very high
		temp = thisInternalBus->bus_clk->OSPEEDR;
		temp &= ~BUS_CBUSMASK32; //TODO  this should not be hard code
		temp |= BUS_CACK_OSPEEDR; //TODO  this should not be hard code
		thisInternalBus->bus_clk->OSPEEDR = temp;
		//Set C bus output type to output push-pull
		temp = thisInternalBus->bus_clk->OTYPER;
		temp &= ~BUS_CBUSMASK32; //TODO  this should not be hard code
		thisInternalBus->bus_clk->OTYPER = temp;
		//Set C bus pullup/down resistors to none
		temp = thisInternalBus->bus_clk->PUPDR;
		temp &= ~BUS_CBUSMASK32; //TODO  this should not be hard code
		thisInternalBus->bus_clk->PUPDR = temp;

		//todo we need same thing for ack line

		//by default we do not have to take the bus before any persmission from the master micro
		internal_bus_release(thisInternalBus);
	}
	else
	{
		//TODO  erro handler
	}
	return thisInternalBus;
}

inline void internal_bus_write_data_frame(InternalBus_t *thisInternalBus,uint8_t *buffer,uint8_t *buffer_len)
{
	//TODO Link data output to magnetometer memory instead
	uint8_t testData[23] = {0,0,0,100,1,1,1,2,1,3,1,4,1,5,1,6,1,7,1,8,1,9,100};

	internal_bus_utilize(thisInternalBus);

	//Send dataframe

	GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[0])  | ((0x000000FF & ~testData[0])  << 16));GPIOA->BSRR = GPIO_PIN_0;
	GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[1])  | ((0x000000FF & ~testData[1])  << 16));GPIOA->BSRR = GPIO_PIN_0;
	GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[2])  | ((0x000000FF & ~testData[2])  << 16));GPIOA->BSRR = GPIO_PIN_0;
	GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[3])  | ((0x000000FF & ~testData[3])  << 16));GPIOA->BSRR = GPIO_PIN_0;
	GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[4])  | ((0x000000FF & ~testData[4])  << 16));GPIOA->BSRR = GPIO_PIN_0;
	GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[5])  | ((0x000000FF & ~testData[5])  << 16));GPIOA->BSRR = GPIO_PIN_0;
	GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[6])  | ((0x000000FF & ~testData[6])  << 16));GPIOA->BSRR = GPIO_PIN_0;
	GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[7])  | ((0x000000FF & ~testData[7])  << 16));GPIOA->BSRR = GPIO_PIN_0;
	GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[8])  | ((0x000000FF & ~testData[8])  << 16));GPIOA->BSRR = GPIO_PIN_0;
	GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[9])  | ((0x000000FF & ~testData[9])  << 16));GPIOA->BSRR = GPIO_PIN_0;
	GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[10]) | ((0x000000FF & ~testData[10]) << 16));GPIOA->BSRR = GPIO_PIN_0;
	GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[11]) | ((0x000000FF & ~testData[11]) << 16));GPIOA->BSRR = GPIO_PIN_0;
	GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[12]) | ((0x000000FF & ~testData[12]) << 16));GPIOA->BSRR = GPIO_PIN_0;
	GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[13]) | ((0x000000FF & ~testData[13]) << 16));GPIOA->BSRR = GPIO_PIN_0;
	GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[14]) | ((0x000000FF & ~testData[14]) << 16));GPIOA->BSRR = GPIO_PIN_0;
	GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[15]) | ((0x000000FF & ~testData[15]) << 16));GPIOA->BSRR = GPIO_PIN_0;
	GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[16]) | ((0x000000FF & ~testData[16]) << 16));GPIOA->BSRR = GPIO_PIN_0;
	GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[17]) | ((0x000000FF & ~testData[17]) << 16));GPIOA->BSRR = GPIO_PIN_0;
	GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[18]) | ((0x000000FF & ~testData[18]) << 16));GPIOA->BSRR = GPIO_PIN_0;
	GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[19]) | ((0x000000FF & ~testData[19]) << 16));GPIOA->BSRR = GPIO_PIN_0;
	GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[20]) | ((0x000000FF & ~testData[20]) << 16));GPIOA->BSRR = GPIO_PIN_0;
	GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[21]) | ((0x000000FF & ~testData[21]) << 16));GPIOA->BSRR = GPIO_PIN_0;
	GPIOA->BRR = GPIO_PIN_0;GPIOB->BSRR = (uint32_t) ((0x000000FF & testData[22]) | ((0x000000FF & ~testData[22]) << 16));GPIOA->BSRR = GPIO_PIN_0;
	GPIOA->BRR = GPIO_PIN_0;

	internal_bus_release(thisInternalBus);
}

inline void internal_bus_utilize(InternalBus_t *thisInternalBus)
{
	uint32_t temp = 0;
	//Set Bus pins to output
	temp = thisInternalBus->bus->MODER;
	temp &= ~BUS_BUSMASK32;  //TODO  this should not be hard code
	temp |= BUS_ACK_MODER;  //TODO  this should not be hard code
	thisInternalBus->bus->MODER = temp;

	//Set clock pin to output
	temp = thisInternalBus->bus_clk->MODER;
	temp &= ~BUS_CBUSMASK32;    //TODO  this should not be hard code
	temp |= BUS_CACK_MODER;    //TODO  this should not be hard code
	thisInternalBus->bus_clk->MODER = temp;

	thisInternalBus->bus_clk->BSRR = thisInternalBus->bus_ack_mask;


	//todo we need same thing for ack line
}

inline void internal_bus_release(InternalBus_t *thisInternalBus)
{
	uint32_t temp = 0;
	//Set all bus pins to low and send complete
	thisInternalBus->bus->BRR = (uint32_t) (0x000000FF);
	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_ack_mask;
	//Set Bus pins to input
	temp = thisInternalBus->bus->MODER;
	temp &= ~BUS_BUSMASK32;  //TODO  this should not be hard code
	thisInternalBus->bus_clk->MODER = temp;
	//Set clock pins to input
	temp = thisInternalBus->bus_clk->MODER;
	temp &= ~BUS_CBUSMASK32;  //TODO  this should not be hard code
	thisInternalBus->bus_clk->MODER = temp;


	//todo we need same thing for ack line
}
