//EEPROM test
//Contains memory locations for test writes

#define I2C_ADDR               (DATA_EEPROM_BASE + 0x00000000UL) //DATA_EEPROM_BASE is a memory alias and will always point to the EEPROM base agnostic of chip ID
#define OTHERINFO1             (DATA_EEPROM_BASE + 0x00000020UL) //Increment by 0x20 to shift memory by one word, 0x10 by half-word, and 0x08 by byte
#define OTHERINFO2             (DATA_EEPROM_BASE + 0x00000030UL) //Make sure to use the appropriate FLASHEx_Type_Program_Data Where the options are:
#define OTHERINFO3             (DATA_EEPROM_BASE + 0x00000038UL) //FLASH_TYPEPROGRAMDATA_BYTE, FLASH_TYPEPROGRAMDATA_HALFWORD, and FLASH_TYPEPROGRAMDATA_WORD

void EEPROMTest(uint32_t addressI2C, uint16_t test1, uint8_t test2);
