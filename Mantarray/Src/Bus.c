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
		thisInternalBus->BUS_BUSMASK32 = 0;
		thisInternalBus->BUS_BUSMODER = 0;
		thisInternalBus->BUS_BUSOSPEEDR = 0;

		thisInternalBus->bus_clk = cl_bus;
		thisInternalBus->bus_clk_mask = cl_pin;
		thisInternalBus->BUS_CLKMASK32 = 0;
		thisInternalBus->BUS_CLKMODER = 0;
		thisInternalBus->BUS_CLKOSPEEDR = 0;

		thisInternalBus->bus_ack = ak_bus;
		thisInternalBus->bus_ack_mask = ak_pin;
		thisInternalBus->BUS_ACKMASK32 = 0;
		thisInternalBus->BUS_ACKMODER = 0;
		thisInternalBus->BUS_ACKOSPEEDR = 0;

		uint8_t pinShifter = 0;
		for (pinShifter = 0; pinShifter < BUS_GPIO_PINS_PER_BUS; pinShifter++)
		{
			if (bus_pins & (1 << pinShifter))
			{
				thisInternalBus->BUS_BUSMASK32 |= (0b11 << (pinShifter * 2));
				thisInternalBus->BUS_BUSMODER |= (0b01 << (pinShifter * 2));
				thisInternalBus->BUS_BUSOSPEEDR |= (0b11 << (pinShifter * 2));
			}
			if (cl_pin & (1 << pinShifter))
			{
				thisInternalBus->BUS_CLKMASK32 |= (0b11 << (pinShifter * 2));
				thisInternalBus->BUS_CLKMODER |= (0b01 << (pinShifter * 2));
				thisInternalBus->BUS_CLKOSPEEDR |= (0b11 << (pinShifter * 2));
			}
			if (ak_pin & (1 << pinShifter))
			{
				thisInternalBus->BUS_ACKMASK32 |= (0b11 << (pinShifter * 2));
				thisInternalBus->BUS_ACKMODER |= (0b01 << (pinShifter * 2));
				thisInternalBus->BUS_ACKOSPEEDR |= (0b11 << (pinShifter * 2));
			}
		}

		uint32_t temp = 0;
		//Set main bus output speed to very high
		temp = thisInternalBus->bus->OSPEEDR;
		temp &= ~thisInternalBus->BUS_BUSMASK32;
		temp |= thisInternalBus->BUS_BUSOSPEEDR;
		thisInternalBus->bus->OSPEEDR = temp;
		//Set main bus output type to output push-pull
		temp = thisInternalBus->bus->OTYPER;
		temp &= ~thisInternalBus->BUS_BUSMASK32;
		thisInternalBus->bus->OTYPER = temp;
		//Set main bus pullup/down resistors to none
		temp = thisInternalBus->bus->PUPDR;
		temp &= ~thisInternalBus->BUS_BUSMASK32;
		thisInternalBus->bus->PUPDR = temp;

		//Set Clock line, output speed to very high
		temp = thisInternalBus->bus_clk->OSPEEDR;
		temp &= ~thisInternalBus->BUS_CLKMASK32;
		temp |= thisInternalBus->BUS_CLKOSPEEDR;
		thisInternalBus->bus_clk->OSPEEDR = temp;
		//Set C bus output type to output push-pull
		temp = thisInternalBus->bus_clk->OTYPER;
		temp &= ~thisInternalBus->BUS_CLKMASK32;
		thisInternalBus->bus_clk->OTYPER = temp;
		//Set C bus pullup/down resistors to none
		temp = thisInternalBus->bus_clk->PUPDR;
		temp &= ~thisInternalBus->BUS_CLKMASK32;
		thisInternalBus->bus_clk->PUPDR = temp;

		//Set Ack line, output speed to very high
		temp = thisInternalBus->bus_ack->OSPEEDR;
		temp &= ~thisInternalBus->BUS_ACKMASK32;
		temp |= thisInternalBus->BUS_ACKOSPEEDR;
		thisInternalBus->bus_ack->OSPEEDR = temp;
		//Set C bus output type to output push-pull
		temp = thisInternalBus->bus_ack->OTYPER;
		temp &= ~thisInternalBus->BUS_ACKMASK32;
		thisInternalBus->bus_ack->OTYPER = temp;
		//Set C bus pullup/down resistors to none
		temp = thisInternalBus->bus_ack->PUPDR;
		temp &= ~thisInternalBus->BUS_ACKMASK32;
		thisInternalBus->bus_ack->PUPDR = temp;

		//by default we do not have to take the bus before any persmission from the master micro
		internal_bus_release(thisInternalBus);
	}
	else
	{
		//TODO  erro handler
	}
	return thisInternalBus;
}

inline void internal_bus_write_data_frame(InternalBus_t *thisInternalBus, uint8_t *buffer, uint8_t buffer_len)
{
	//TODO Link data output to magnetometer memory instead
	uint8_t testData[31] = {0,0,0,100,1,1,1,2,1,3,0,0,0,100,1,4,1,5,1,6,0,0,0,100,1,7,1,8,1,9,100};

	internal_bus_utilize(thisInternalBus);

	//Send dataframe
	//TODO may need a data offset term if the bus pins do not begin at 0
	//ie. thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & (testData[0] << BUSOFFSET))  | ((thisInternalBus->bus_mask & ~(testData[0] << BUSOFFSET))  << 16));
	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[0])  | ((thisInternalBus->bus_mask & ~testData[0])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[1])  | ((thisInternalBus->bus_mask & ~testData[1])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[2])  | ((thisInternalBus->bus_mask & ~testData[2])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[3])  | ((thisInternalBus->bus_mask & ~testData[3])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[4])  | ((thisInternalBus->bus_mask & ~testData[4])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[5])  | ((thisInternalBus->bus_mask & ~testData[5])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[6])  | ((thisInternalBus->bus_mask & ~testData[6])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[7])  | ((thisInternalBus->bus_mask & ~testData[7])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[8])  | ((thisInternalBus->bus_mask & ~testData[8])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[9])  | ((thisInternalBus->bus_mask & ~testData[9])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[10])  | ((thisInternalBus->bus_mask & ~testData[10])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[11])  | ((thisInternalBus->bus_mask & ~testData[11])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[12])  | ((thisInternalBus->bus_mask & ~testData[12])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[13])  | ((thisInternalBus->bus_mask & ~testData[13])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[14])  | ((thisInternalBus->bus_mask & ~testData[14])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[15])  | ((thisInternalBus->bus_mask & ~testData[15])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[16])  | ((thisInternalBus->bus_mask & ~testData[16])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[17])  | ((thisInternalBus->bus_mask & ~testData[17])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[18])  | ((thisInternalBus->bus_mask & ~testData[18])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[19])  | ((thisInternalBus->bus_mask & ~testData[19])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[20])  | ((thisInternalBus->bus_mask & ~testData[20])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[21])  | ((thisInternalBus->bus_mask & ~testData[21])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[22])  | ((thisInternalBus->bus_mask & ~testData[22])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[23])  | ((thisInternalBus->bus_mask & ~testData[23])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[24])  | ((thisInternalBus->bus_mask & ~testData[24])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[25])  | ((thisInternalBus->bus_mask & ~testData[25])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[26])  | ((thisInternalBus->bus_mask & ~testData[26])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[27])  | ((thisInternalBus->bus_mask & ~testData[27])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[28])  | ((thisInternalBus->bus_mask & ~testData[28])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[29])  | ((thisInternalBus->bus_mask & ~testData[29])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	thisInternalBus->bus_clk->BRR = thisInternalBus->bus_clk_mask;
	thisInternalBus->bus->BSRR = (uint32_t) ((thisInternalBus->bus_mask & testData[30])  | ((thisInternalBus->bus_mask & ~testData[30])  << 16));
	thisInternalBus->bus_clk->BSRR = (uint32_t) thisInternalBus->bus_clk_mask;

	internal_bus_release(thisInternalBus);
}

inline void internal_bus_utilize(InternalBus_t *thisInternalBus)
{
	uint32_t temp = 0;
	//Set Bus pins to output
	temp = thisInternalBus->bus->MODER;
	temp &= ~thisInternalBus->BUS_BUSMASK32;
	temp |= thisInternalBus->BUS_BUSMODER;
	thisInternalBus->bus->MODER = temp;

	//Set clock pin to output
	temp = thisInternalBus->bus_clk->MODER;
	temp &= ~thisInternalBus->BUS_CLKMASK32;
	temp |= thisInternalBus->BUS_CLKMODER;
	thisInternalBus->bus_clk->MODER = temp;

	//Set ack pin to output
	temp = thisInternalBus->bus_ack->MODER;
	temp &= ~thisInternalBus->BUS_ACKMASK32;
	temp |= thisInternalBus->BUS_ACKMODER;
	thisInternalBus->bus_ack->MODER = temp;

	thisInternalBus->bus_ack->BSRR = (uint32_t) thisInternalBus->bus_ack_mask;
}

inline void internal_bus_release(InternalBus_t *thisInternalBus)
{
	uint32_t temp = 0;
	//Set all bus pins to low and send complete
	thisInternalBus->bus->BRR = thisInternalBus->bus_mask;
	thisInternalBus->bus_ack->BRR = thisInternalBus->bus_ack_mask;

	//Set Bus pins to input
	temp = thisInternalBus->bus->MODER;
	temp &= ~thisInternalBus->BUS_BUSMASK32;
	thisInternalBus->bus->MODER = temp;

	//Set clock pin to input
	temp = thisInternalBus->bus_clk->MODER;
	temp &= ~thisInternalBus->BUS_CLKMASK32;
	thisInternalBus->bus_clk->MODER = temp;

	//Set ack pins to input
	temp = thisInternalBus->bus_ack->MODER;
	temp &= ~thisInternalBus->BUS_ACKMASK32;
	thisInternalBus->bus_ack->MODER = temp;
}
