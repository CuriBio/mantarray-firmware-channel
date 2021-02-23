#include "GlobalTimer.h"
#include "lis3mdl.h"
#include "UART_Comm.h"

#ifndef SYSTEM_H_
#define SYSTEM_H_
//-------------------------------------------------

//-------------------------------------------------
#define MODULE_NUMBER						24
//--------------------------------------------------
#define MODULE_SYSTEM_STATUS_START				0

#define	MODULE_BUS_STATUS_ACTIVE				10
#define	MODULE_BUS_STATUS_DOWN					20

#define MODULE_SYSTEM_STATUS_FIRST_TIME			40
#define MODULE_SYSTEM_STATUS_INITIATION			50
#define MODULE_SYSTEM_STATUS_IDLE				60
#define MODULE_SYSTEM_STATUS_CONNECTED			70
#define MODULE_SYSTEM_STATUS_CALIBRATION			80
#define MODULE_SYSTEM_STATUS_FAULTY				90
#define MODULE_SYSTEM_STATUS_FIRMWARE_UPDATE		100
//------------------------------------------------------
typedef struct
{
	uint8_t type;  	// what type of module we are  are we a reference module or camera module? we will find out here
	uint8_t ID;    	//what is our module ID   during the first run we got this ID from master we will load that every time we bootup from eeprom or flash
	uint8_t status;   // what is our status now active disabled ....
	uint8_t new_data_ready;    //we have a new data ready to send  /for tmp mag ....  place holder for assigning each bit to a flag
	uint8_t state;    // this is the current state of this module which is used and update in state machine
	LIS3MDL sensorA;
	LIS3MDL sensorB;
	LIS3MDL sensorC;
	GlobalTimer GlobalTimerTest;
	//UART_Comm SerialTest;

	uint8_t new_lis3mdl_data;
} System;

void state_machine(System *thisSystem);
void module_system_init(System *thisSystem);

#endif /* SYSTEM_H_ */
