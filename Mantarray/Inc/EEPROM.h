#include "system.h"

#ifndef EEPROM_H_
#define EEPROM_H_

#define I2C_ADDR               (DATA_EEPROM_BASE + 0x00000000UL) //uint8 DATA_EEPROM_BASE is a memory alias and will always point to the EEPROM base agnostic of chip ID
#define FIRST_TIME_INITIATION  (DATA_EEPROM_BASE + 0x00000008UL) //uint32
#define WHICH_MAGNETOMETER	   (DATA_EEPROM_BASE + 0x00000028UL) //uint8
#define OTHERINFO1             (DATA_EEPROM_BASE + 0x00001000UL) //Increment by 0x20 to shift memory by one word, 0x10 by half-word, and 0x08 by byte
#define OTHERINFO2             (DATA_EEPROM_BASE + 0x00001010UL) //Make sure to use the appropriate FLASHEx_Type_Program_Data Where the options are:
#define OTHERINFO3             (DATA_EEPROM_BASE + 0x00001018UL) //FLASH_TYPEPROGRAMDATA_BYTE, FLASH_TYPEPROGRAMDATA_HALFWORD, and FLASH_TYPEPROGRAMDATA_WORD

#define EEPROM_FIRST_TIME_COMPLETE		0x135791

void EEPROMInit(System *thisSystem);
void EEPROMTest(uint32_t addressI2C, uint16_t test1, uint8_t test2);

#endif /* EEPROM_H_ */
