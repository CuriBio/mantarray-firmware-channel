#ifndef GLOBALTIMER_H_
#define GLOBALTIMER_H_

#include <string.h>
#include <stdio.h>

typedef struct
{
	uint8_t overflowCounter;
	uint32_t countReg;
	uint8_t overflowCountBegin;
	uint64_t totValue;
} GlobalTimer_t;

void GlobalTimerInit(GlobalTimer_t *thisGlobalTimer);

uint64_t getGlobalTimer(GlobalTimer_t *thisGlobalTimer);

void setGlobalTimer(GlobalTimer_t *thisGlobalTimer, uint64_t newValue);

#endif /* GLOBALTIMER_H_ */
