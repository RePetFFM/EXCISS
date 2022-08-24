/***********************************************************************
* DESCRIPTION :
*
* For more information and documentation go to https://github.com/exciss-space/EXCISS
* AUTHOR :    Shintaro Fujita
*
**/


#ifndef _SPI_EEPROM_H_
#define _SPI_EEPROM_H_

#include <SPI.h>

// define default port settings
#define SPI_EEPROM_PORT_DATAOUT 51//MOSI
#define SPI_EEPROM_PORT_DATAIN  50//MISO 
#define SPI_EEPROM_PORT_SPICLOCK  52//sck
#define SPI_EEPROM_PORT_SLAVESELECT 12//ss
#define SPI_EEPROM_PORT_WRITEPROTECT 10//ss
#define SPI_EEPROM_PORT_HOLD 6//ss

// opcodes
#define SPI_EEPROM_OPCODE_RDID  0x83
#define SPI_EEPROM_OPCODE_WREN  0x06
#define SPI_EEPROM_OPCODE_WRDI  0x04
#define SPI_EEPROM_OPCODE_RDSR  0x05
#define SPI_EEPROM_OPCODE_WRSR  0x01
#define SPI_EEPROM_OPCODE_READ  0x03
#define SPI_EEPROM_OPCODE_WRITE 0x02

// etc definitions
#define SPI_EEPROM_TIMEOUT_WRITE_WAIT 100UL // maximum wait time for write ready in milliseconds

struct SPI_EEPROM_CHIP_ID {
	char manufacturer_code;
	char spi_family_code;
	char memory_density_code;
};

class SPI_EEPROM {
public:

	// contstructors
	SPI_EEPROM();

	SPI_EEPROM(int a_eeprom_select_port, int a_eeprom_writeprotect_port, int a_eeprom_hold_port );

	// eeprom operations methods
	void write_enable();

	bool writeBytes(uint16_t a_address, char * a_byte, int length);

	bool readBytes(uint16_t a_address, char * a_buf, int length);

	struct SPI_EEPROM_CHIP_ID readChipID();

	void init_ports(int a_eeprom_select_port, int a_eeprom_writeprotect_port, int a_eeprom_hold_port );
private:

	bool write_ready();
};

#endif