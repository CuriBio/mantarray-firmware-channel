#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "i2c_network_interface.h"
#include "Magnetometer.h"
#include "GlobalTimer.h"

#include "Bus.h"
#include "main.h"
#include "GlobalTimer.h"
#include "EEPROM.h"
#include "I2C.h"
//-------------------------------------------------

//-------------------------------------------------
#define MODULE_SYSTEM_NUM_SENSORS               3
#define MODULE_SYSTEM_PACKET_LENGHT             33
//--------------------------------------------------
#define MODULE_SYSTEM_STATUS_START				0

#define	MODULE_BUS_STATUS_ACTIVE				10
#define	MODULE_BUS_STATUS_DOWN					20

#define MODULE_SYSTEM_STATUS_FIRST_TIME			40
#define MODULE_SYSTEM_STATUS_INITIATION			50
#define MODULE_SYSTEM_STATUS_IDLE				60
#define MODULE_SYSTEM_STATUS_CONNECTED			70
#define MODULE_SYSTEM_STATUS_CALIBRATION		80
#define MODULE_SYSTEM_STATUS_FAULTY				90
#define MODULE_SYSTEM_STATUS_FIRMWARE_UPDATE	100
//------------------------------------------------------
typedef struct
{
	uint8_t type;  		// what type of module we are  are we a reference module or camera module? we will find out here
	uint8_t ID;    		//what is our module ID   during the first run we got this ID from master we will load that every time we bootup from eeprom or flash
	uint8_t status;   	// what is our status now active disabled ....
	uint8_t state;    	// this is the current state of this module which is used and update in state machine
	uint8_t BUS_FLAG;
	uint32_t bus_output_buffer[MODULE_SYSTEM_PACKET_LENGHT];

	GlobalTimer_t * ph_global_timer;

	Magnetometer_t *sensors[MODULE_SYSTEM_NUM_SENSORS];

	I2C_t *i2c_line;

	InternalBus_t *data_bus;

} System;

void state_machine(System *thisSystem);
void module_system_init(System *thisSystem, SPI_HandleTypeDef * h_SPI, I2C_HandleTypeDef * h_I2C, TIM_HandleTypeDef * h_global_timer);

#endif /* SYSTEM_H_ */
