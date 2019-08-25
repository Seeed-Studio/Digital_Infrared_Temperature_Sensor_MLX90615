#ifndef __MLX90615_H__
#define __MLX90615_H__

#include <Arduino.h>
#include <I2cMaster.h>
#include <stdint.h>
#include <stdbool.h>

#define MLX90615_OBJECT_TEMPERATURE     0x27
#define MLX90615_AMBIENT_TEMPERATURE    0x26
#define AccessEEPROM                    0x13
#define Default_Emissivity              0x4000
#define DEVICE_ADDR                     0x5B



class MLX90615 {

protected:
	union {
		uint8_t	buffer[5];
		struct {
			uint8_t dev;
			uint8_t cmd;
			uint8_t dataLow;
			uint8_t dataHigh;
			uint8_t pec;
		};
	};

public:
	I2cMasterBase *bus;
	uint8_t i2c_addr;

	/*******************************************************************
	 * Function Name: init
	 * Description:  initialize for i2c device.
	 * Parameters: sda pin, scl pin, i2c device address
	 * Return: null
	******************************************************************/
	MLX90615(uint8_t addr, I2cMasterBase *i2c) {
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
	float getTemperature(int Temperature_kind, bool fahrenheit = false) {
		dev = (i2c_addr << 1);  // remain to be seen!
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

		return fahrenheit ? (celcius*1.8) + 32 : celcius;
	}

	float toFahrenheit(float celcius){
		return (celcius*1.8) + 32;
	}

	// In a later version this function might be deleted
	float getTemperatureFahrenheit(float celcius){
		return toFahrenheit(celcius);
	}
	/****************************************************************
	 * Function Name: readEEPROM
	 * Description:  read from eeprom to see what data in it.
	 * Parameters:
	 * Return: -1 for failure, 0 for ok
	****************************************************************/
	int readEEPROM(uint8_t reg = AccessEEPROM) {

		delay(500);

		uint16_t eepromData = readRegS16(reg);

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
	int write(uint8_t reg,uint16_t value, bool checksum = false)
	{
		if (checksum) {
	   dev = i2c_addr<<1;
	   cmd = reg;
	   dataLow = value & 0xff;
	   dataHigh = (value >> 8) & 0xff;
	   pec = crc8Msb(0x07, (uint8_t*)buffer, 4);

	//    Serial.println(*data, HEX);

	   if(crc8Msb(0x07,(uint8_t*)buffer, 5))
	   {
			 Serial.println("CRC8 Check Err...");
			 return -1;
	   }
	   Serial.println("CRC8 Check OK...");
		}

		 if(!bus->start(dev | I2C_WRITE)) return -2;
		 if(!bus->write(cmd)) return -3;
		 if(!bus->write(dataLow)) return -4;
		 if(!bus->write(dataHigh)) return -5;
		if (checksum) {
		 if(!bus->write(pec)) return -6;
		}
		 bus->stop();

	   return 0;
	}

	int writeEmissivity(uint16_t emissivity){
		return write(AccessEEPROM,emissivity,true);
	}

	/****************************************************************
	 * Function Name: read8
	 * Description:  i2c read register for one byte
	 * Parameters:
	 * Return: one byte data from i2c device
	****************************************************************/
	uint8_t read8(uint8_t reg)
	{
		uint8_t data;
		uint8_t dev = i2c_addr << 1;

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
	int readRegS16(uint8_t reg)
	{
		dev = i2c_addr << 1;

		bus->start(dev | I2C_WRITE);
		bus->write(reg);
		// read
		bus->restart(dev | I2C_READ);
		dataLow = bus->read(false);
		dataHigh = bus->read(false);
		pec = bus->read(true);
		bus->stop();

		return (int)(dataHigh << 8) || dataLow;
	}

	/****************************************************************
	 * Function Name: writeReg8
	 * Description:  i2c write register one byte
	 * Parameters:
	 * Return:
	****************************************************************/
	void writeReg8(uint8_t data, uint8_t reg)
	{
		uint8_t dev = i2c_addr << 1;

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
	void writeReg16(uint16_t data, uint8_t reg)
	{
		uint8_t dev = i2c_addr << 1;

		bus->start(dev | I2C_WRITE);
		bus->write(reg);
		bus->write((data >> 8) & 0xFF);
		bus->write(data & 0xFF);
		bus->stop();
	}
};
#endif // __MLX90615_H__
