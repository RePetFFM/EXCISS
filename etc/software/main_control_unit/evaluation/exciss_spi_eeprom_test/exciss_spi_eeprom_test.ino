
#include <Arduino.h>
#include "SPI_EEPROM.h"

#define SLAVESELECT 12//ss
#define WRITEPROTECT 10//ss
#define EEPROM_HOLD 6//ss


SPI_EEPROM spi_eeprom;

void setup()
{
  Serial.begin(9600);
  Serial.println("start");

  spi_eeprom.write_enable();

  // read status register
  Serial.println("Status");

  struct SPI_EEPROM_CHIP_ID chipID = spi_eeprom.readChipID();

  Serial.println(chipID.manufacturer_code, HEX);
  Serial.println(chipID.spi_family_code, HEX);
  Serial.println(chipID.memory_density_code, HEX);

  Serial.println("Write ");

  static char test[3];

  test[0] = 'B';
  test[1] = 'b';
  test[2] = 'c';

  spi_eeprom.writeBytes(0xfff8,&test[0],3);

  test[0] = 0xFF;
  test[1] = 0xFF;
  test[2] = 0xFF;

  
  Serial.println("Read ");

  spi_eeprom.readBytes(0xfff8,&test[0],3);

  Serial.println( test[0]);
  Serial.println( test[1]);
  Serial.println( test[2]);
 
    
}

void loop()
{
  /*
  eeprom_output_data = read_eeprom(address);
  Serial.print(eeprom_output_data,DEC);
  Serial.print('\n');
  address++;
  if (address == 128)
    address = 0;
  delay(500); //pause for readability
  */
}