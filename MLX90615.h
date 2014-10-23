

#ifndef __MLX90615_H__
#define __MLX90615_H__

#include <Arduino.h>


#define MLX90615_OBJECT_TEMPERATURE     0x27
#define MLX90615_AMBIENT_TEMPERATURE    0x26
#define AccessEEPROM                    0x13
#define Default_EEPROM_DATA             0x4000
#define DEVICE                          0x5B

class MLX90615 {
public:
	void init();
	uint8_t crc8Msb(uint8_t poly, uint8_t* data, int size);
	void printTemperature(int Temperature_kind);
	int readEEPROM();
	int writeEEPROM(uint16_t eeprom_data);

};

#endif // __MLX90615_H__
