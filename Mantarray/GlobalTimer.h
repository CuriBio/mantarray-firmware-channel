#include <string.h>
#include <stdio.h>

#ifndef GLOBALTIMER_H_
#define GLOBALTIMER_H_

typedef struct
{
	uint8_t overflowCounter;
	uint32_t countReg;
	uint8_t overflowCountBegin;
	uint64_t totValue;

	//TEST CODE
	char uartBuffer[30];
	uint8_t uartBufLen;
} GlobalTimer;

void init_Global_Timer_struct(GlobalTimer *thisGlobalTimer);

uint64_t getGlobalTimer(GlobalTimer *thisGlobalTimer);

void setGlobalTimer(GlobalTimer *thisGlobalTimer, uint64_t newValue);

#endif /* GLOBALTIMER_H_ */
