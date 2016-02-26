#ifndef __MLX90615_H__
#define __MLX90615_H__

#include <Arduino.h>
#include <I2cMaster.h>

#define MLX90615_OBJECT_TEMPERATURE     0x27
#define MLX90615_AMBIENT_TEMPERATURE    0x26
#define AccessEEPROM                    0x13
#define Default_Emissivity              0x4000
#define DEVICE_ADDR                     0x5B



class MLX90615 {
public:
	I2cMasterBase *bus;
	byte i2c_addr;

	/*******************************************************************
	 * Function Name: init
	 * Description:  initialize for i2c device.
	 * Parameters: sda pin, scl pin, i2c device address
	 * Return: null
	******************************************************************/
	MLX90615(byte addr, I2cMasterBase *i2c) {
		i2c_addr = addr;
		bus = i2c;
	}

	/****************************************************************
	 * Function Name: crc8_msb
	 * Description:  CRC8 check to compare PEC data
	 * Parameters: poly - x8+x2+x1+1, data - array to check, array size
	 * Return: 0 – data right; 1 – data Error
	****************************************************************/
	uint8_t crc8Msb(uint8_t poly, uint8_t* data, int size)	{
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
	 * Return:  true for ok, false for failure
	****************************************************************/
	float getTemperature(int Temperature_kind) {
		byte dev = (i2c_addr << 1);  // remain to be seen!
		byte dataLow = 0;
		byte dataHigh = 0;
		byte pec = 0;
		float celcius = 0.0;

		bus->start(dev | I2C_WRITE);
		bus->write(Temperature_kind);
		// read
		bus->restart(dev | I2C_READ);
		dataLow = bus->read(false);
		dataHigh = bus->read(false);
		pec = bus->read(true);
		bus->stop();

		//This converts high and low bytes together and processes temperature, MSB is a error bit and is ignored for temps
		double tempFactor = 0.02; // 0.02 degrees per LSB (measurement resolution of the MLX90614)
		double tempData = 0x0000; // zero out the data
		int frac; // data past the decimal point

		// This masks off the error bit of the high byte, then moves it left 8 bits and adds the low byte.
		tempData = (double)(((dataHigh & 0x007F) << 8) | dataLow);
		tempData = (tempData * tempFactor) - 0.01;

		celcius = (float)(tempData - 273.15);

		return celcius;
		//float fahrenheit = (celcius*1.8) + 32;
	}

	float getTemperatureFahrenheit(float celcius){
		return (celcius*1.8) + 32;
	}
	/****************************************************************
	 * Function Name: readEEPROM
	 * Description:  read from eeprom to see what data in it.
	 * Parameters:
	 * Return: -1 for failure, 0 for ok
	****************************************************************/
	int readEEPROM() {
		int dev = i2c_addr << 1;
		int dataLow = 0;
		int dataHigh = 0;
		int pec = 0;

		delay(500);
		bus->start(dev | I2C_WRITE);
		bus->write(AccessEEPROM);
		// read
		if(!bus->restart(dev | I2C_READ))
		dataLow = bus->read(false);
		dataHigh = bus->read(false);
		pec = bus->read(true);
		bus->stop();

		uint16_t eepromData = 0x0000; // zero out the data

		eepromData = (uint16_t)(((dataHigh & 0x007F) << 8) + dataLow);

		Serial.print("eepromData: 0x");
		Serial.println(eepromData, HEX);

		return 0;
	}

	/****************************************************************
	 * Function Name: WriteEEPROM
	 * Description:  write EEPROM at address 0x13 to adjust emissivity
	 * Parameters: emissivity – data to write into EEPROM
	 * Return: 0 - success; -1 - crc8 check err.
	****************************************************************/
	int writeEEPROM(uint16_t emissivity) {
		char  buffer[6] = {0};
	   char *write = buffer + 0;
	   char *cmd = buffer + 1;
	   char *data = buffer + 2;
	   int dev = i2c_addr<<1;

	   *write = dev;
	   *cmd = AccessEEPROM;
	   *(data++) = emissivity & 0xff;
	   *(data++) = (emissivity >> 8) & 0xff;
	   *data = crc8Msb(0x07, (uint8_t*)buffer, 4);

	   Serial.println(*data, HEX);

	   if(crc8Msb(0x07,(uint8_t*)buffer, 5))
	   {
			 Serial.println("CRC8 Check Err...");
			 return -1;
	   }
	   Serial.println("CRC8 Check OK...");

		 if(!bus->start(dev | I2C_WRITE)) return -1;
		 if(!bus->write(AccessEEPROM)) return -1;
		 if(!bus->write(emissivity & 0xff)) return -1;
		 if(!bus->write((emissivity >> 8) & 0xff)) return -1;
		 if(!bus->write(data[0])) return -1;
		 bus->stop();

	   return 0;
	}

	/****************************************************************
	 * Function Name: read8
	 * Description:  i2c read register for one byte
	 * Parameters:
	 * Return: one byte data from i2c device
	****************************************************************/
	byte read8(byte reg)
	{
		byte data;
		byte dev = i2c_addr << 1;

		bus->start(dev | I2C_WRITE);
		bus->write(reg);
		// read
		bus->restart(i2c_addr | I2C_READ);
		data = bus->read(false);
		bus->read(true);
		bus->stop();

		return data;
	}

	/****************************************************************
	 * Function Name: readRegS16
	 * Description:  i2c read register for int data
	 * Parameters:
	 * Return: int data
	****************************************************************/
	int readRegS16(byte reg)
	{
		uint8_t dataLow, dataHigh;
		byte dev = i2c_addr << 1;

		bus->start(dev | I2C_WRITE);
		bus->write(reg);
		// read
		bus->restart(i2c_addr | I2C_READ);
		dataLow = bus->read(false);
		dataHigh = bus->read(false);
		bus->read(true);
		bus->stop();

		return (int)(dataHigh << 8) || dataLow;
	}

	/****************************************************************
	 * Function Name: writeReg8
	 * Description:  i2c write register one byte
	 * Parameters:
	 * Return:
	****************************************************************/
	void writeReg8(byte data, byte reg)
	{
		byte dev = i2c_addr << 1;

		bus->start(dev | I2C_WRITE);
		bus->write(reg);
		bus->write(data);
		bus->stop();
	}

	/****************************************************************
	 * Function Name: writeReg16
	 * Description:  i2c write register int data
	 * Parameters:
	 * Return:
	****************************************************************/
	void writeReg16(uint16_t data, byte reg)
	{
		byte dev = i2c_addr << 1;

		bus->start(dev | I2C_WRITE);
		bus->write(reg);
		bus->write((data >> 8) & 0xFF);
		bus->write(data & 0xFF);
		bus->stop();
	}
};
#endif // __MLX90615_H__
