#include "MLX90615.h"
#include "SoftI2CMaster.h"

void MLX90615::init()
{
	i2c_init();

}

/**************************************************************** 
 * Function Name: crc8_msb
 * Description:  CRC8 check to compare PEC data  
 * Parameters: poly - x8+x2+x1+1, data - array to check, array size
 * Return: 0 – data right; 1 – data Error
****************************************************************/ 
uint8_t MLX90615::crc8Msb(uint8_t poly, uint8_t* data, int size)
{
    uint8_t crc = 0x00;
    int bit;

    while (size--) 
	{
        crc ^= *data++;
        for (bit = 0; bit < 8; bit++) 
		{
            if (crc & 0x80) {
                crc = (crc << 1) ^ poly;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}

/**************************************************************** 
 * Function Name: Print_Temperature
 * Description: receive and print out temperature in Celcius and fahrenheit
 * Parameters: Temperature_kind - choose ambient or object Temperature
 * Return: 
****************************************************************/ 
void MLX90615::printTemperature(int Temperature_kind)
{   
	int dev = DEVICE<<1;
    int dataLow = 0;
    int dataHigh = 0;
    int pec = 0;
    
    i2c_start(dev+I2C_WRITE);
    i2c_write(Temperature_kind);  
    // read
    i2c_rep_start(dev+I2C_READ);
    dataLow = i2c_read(false); 
    dataHigh = i2c_read(false); 
    pec = i2c_read(true);
    i2c_stop();
    
    //This converts high and low bytes together and processes temperature, MSB is a error bit and is ignored for temps
    double tempFactor = 0.02; // 0.02 degrees per LSB (measurement resolution of the MLX90614)
    double tempData = 0x0000; // zero out the data
    int frac; // data past the decimal point
    
    // This masks off the error bit of the high byte, then moves it left 8 bits and adds the low byte.
    tempData = (double)(((dataHigh & 0x007F) << 8) + dataLow);
    tempData = (tempData * tempFactor)-0.01;
    
    float celcius = tempData - 273.15;
    float fahrenheit = (celcius*1.8) + 32;    
 
    Serial.print("Celcius: ");
    Serial.println(celcius);

    Serial.print("Fahrenheit: ");
    Serial.println(fahrenheit); 
}

/**************************************************************** 
 * Function Name: readEEPROM
 * Description:  read from eeprom to see what data in it.
 * Parameters: 
 * Return:
****************************************************************/ 
int MLX90615::readEEPROM() 
{
    int dev = DEVICE<<1;
    int dataLow = 0;
    int dataHigh = 0;
    int pec = 0;
    
	delay(500);
    i2c_start(dev+I2C_WRITE);
    i2c_write(AccessEEPROM);  
    // read
    i2c_rep_start(dev+I2C_READ);
    dataLow = i2c_read(false); //Read 1 byte and then send ack
    dataHigh = i2c_read(false); //Read 1 byte and then send ack
    pec = i2c_read(true);
    i2c_stop();
    
    uint16_t eepromData = 0x0000; // zero out the data
    
    eepromData = (uint16_t)(((dataHigh & 0x007F) << 8) + dataLow);
 
    Serial.print("eepromData: 0x");
    Serial.println(eepromData, HEX);
	
	return 0;
  
}

/**************************************************************** 
 * Function Name: WriteEEPROM
 * Description:  Set  MLX90615 EEPROM data at address 0x13
 * Parameters: eeprom_data – data to write into EEPROM
 * Return: 0 - success; -1 - crc8 check err.
****************************************************************/ 
int MLX90615::writeEEPROM(uint16_t eepromData) 
{
 
   char  buffer[6] = {0};
   char *write = buffer + 0;
   char *cmd = buffer + 1;
   char *data = buffer + 2;
   
   int dev = 0x5B<<1;
   
   *write = dev;
   *cmd = AccessEEPROM;
   *(data++) = eepromData & 0xff ;
   *(data++) = (eepromData >> 8) & 0xff;
   *data = crc8Msb(0x07, (uint8_t*)buffer, 4);
   
   Serial.println(*data, HEX);
       
   if(crc8Msb(0x07,(uint8_t*)buffer, 5)) 
   {
     Serial.println("CRC8 Check Err...");
     return -1;
   }
   Serial.println("CRC8 Check OK...");
   
   i2c_start(dev+I2C_WRITE);  
   i2c_write(AccessEEPROM);
   i2c_write( eepromData & 0xff );
   i2c_write( (eepromData >> 8) & 0xff );     
   i2c_write(data[0]);   
   i2c_stop(); 
   
   return 0;
}