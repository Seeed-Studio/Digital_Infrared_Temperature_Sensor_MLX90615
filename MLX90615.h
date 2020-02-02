#ifndef __MLX90615_H__
#define __MLX90615_H__

#include <Arduino.h>
#include <Wire.h>
#include <I2cMaster.h>
#include <stdint.h>
#include <stdbool.h>

#define MLX90615_EEPROM_SA          0x10
#define MLX90615_EEPROM_PWMT_MIN    MLX90615_EEPROM_SA
#define MLX90615_EEPROM_PWMT_RNG    0x11
#define MLX90615_EEPROM_CONFIG      0x12
#define MLX90615_EEPROM_EMISSIVITY  0x13

#define MLX90615_RAW_IR_DATA            0x25
#define MLX90615_AMBIENT_TEMPERATURE    0x26
#define MLX90615_OBJECT_TEMPERATURE     0x27

#define MLX90615_SLEEP	0xC6

// DEPRECATED! (just emissivity, not the whole EEPROM)
#define AccessEEPROM                    MLX90615_EEPROM_EMISSIVITY

#define Default_Emissivity              0x4000
#define MLX90615_DefaultAddr			0x5B

// DEPRECATED! (too ambiguous in some setups)
#define DEVICE_ADDR                     MLX90615_DefaultAddr

class MLX90615 {

  protected:
    TwoWire* wbus;
    I2cMasterBase* bus;

    union {
        uint8_t	buffer[5];
        struct {
            union {
                uint8_t dev;
                struct {
                    uint8_t             : 1;
                    uint8_t i2c_addr    : 7;
                };
            };
            uint8_t cmd;
            uint8_t dataLow;
            uint8_t dataHigh;
            uint8_t pec;
        };
    };

  public:

    /*******************************************************************
        Function Name: init
        Description:  initialize for i2c device.
        Parameters: sda pin, scl pin, i2c device address
        Return: null
    ******************************************************************/
    MLX90615(uint8_t addr, I2cMasterBase* i2c) {
        dev = addr << 1;
        bus = i2c;
        wbus = 0;
    }

    MLX90615(uint8_t addr, TwoWire* i2c) {
        dev = addr << 1;
        bus = 0;
        wbus = i2c;
    }

    /****************************************************************
        Function Name: crc8_msb
        Description:  CRC8 check to compare PEC data
        Parameters: poly - x8+x2+x1+1, data - array to check, array size
        Return: 0 – data right; 1 – data Error
    ****************************************************************/
    uint8_t crc8Msb(uint8_t poly, uint8_t* data, int size)	{
        uint8_t crc = 0x00;
        int bit;

        while (size--) {
            crc ^= *data++;
            for (bit = 0; bit < 8; bit++) {
                if (crc & 0x80) {
                    crc = (crc << 1) ^ poly;
                } else {
                    crc <<= 1;
                }
            }
        }

        return crc;
    }

    /**
        Get temperature in Celcius or Fahrenheit
        Parameters:
        > Temperature_kind: MLX90615_AMBIENT_TEMPERATURE or MLX90615_OBJECT_TEMPERATURE
        > bool: true for Fahrenheit scale, false (or unspec) for Celsius scale
        Return:  true for ok, false for failure
    */
    float getTemperature(int Temperature_kind, bool fahrenheit = false) {
        float celsius;
        uint16_t tempData;

        readReg(Temperature_kind, &tempData);

        double tempFactor = 0.02; // 0.02 degrees per LSB (measurement resolution of the MLX90614)

        // This masks off the error bit of the high byte
        celsius = ((float)tempData * tempFactor) - 0.01;

        celsius = (float)(celsius - 273.15);

        return fahrenheit ? (celsius * 1.8) + 32.0 : celsius;
    }

    // DEPRECATED (use getTemperature)
    float toFahrenheit(float celsius) {
        return (celsius * 1.8) + 32;
    }

    // DEPRECATED (use getTemperature)
    float getTemperatureFahrenheit(float celsius) {
        return toFahrenheit(celsius);
    }
    /**
        Function Name: readEEPROM - DEPRECATED! (use readReg and print outside)
        Description:  read from eeprom to see what data in it.
        Parameters:
        Return: -1 for failure, 0 for ok
    */
    int readEEPROM(uint8_t reg = AccessEEPROM) {

        // delay(500);

        uint16_t eepromData = readRegS16(reg);

        Serial.print("eepromData: 0x");
        Serial.println(eepromData, HEX);

        return 0;
    }

    /**
        Function Name: WriteEEPROM - DEPRECATED! (use writeReg)
        Description:  write EEPROM at address 0x13 to adjust emissivity
        Parameters: emissivity – data to write into EEPROM
        Return: same as writeReg()
    */
    int writeEEPROM(uint16_t emissivity) {
        return writeReg(AccessEEPROM, emissivity);
    }

    /**
        Read a MLX90615 register.
        @param MLXaddr: MLX90615 EEPROM/RAM address
        @param result: Pointer to variable to store the readed value
        @return: status:   0  OK
                         -1  Bad CRC calc
                         -2  I2C Error
                        -10  I2C Connector not specified yet
    */
    int readReg(uint8_t MLXaddr, uint16_t* resultReg) {
        if (bus && !wbus) {
            // Using alternative I2C library
            bus->start(dev | I2C_WRITE);
            bus->write(MLXaddr);
            bus->restart(dev | I2C_READ);
            dataLow = bus->read(false);
            dataHigh = bus->read(false);
            *resultReg = (uint16_t)dataHigh << 8 | dataLow;
            pec = bus->read(true);
            bus->stop();
            return 0;
        } else if (wbus && !bus) {
            // Using Wire
            wbus->beginTransmission(i2c_addr);
            wbus->write(MLXaddr);
            wbus->endTransmission(false);
            if (wbus->requestFrom(i2c_addr, (uint8_t)3) == 3) {
                dataLow = wbus->read();
                dataHigh = wbus->read();
                pec = wbus->read();
                *resultReg = dataHigh << 8 | dataLow;
                return 0;
            } else {
                return -2;
            }
        } else {
            return -10;
        }
    }

    /**
        Function Name: read8 - DEPRECATED! (use readReg)
        Description:  i2c read register for one byte
        Parameters: reg: address of the register to read
        Return: LSB of the requested register
    */
    uint8_t read8(uint8_t reg) {
        return readRegS16(reg);
    }

    /**
        Function Name: readRegS16 - DEPRECATED! (use readReg)
        Description:  i2c read register for int data
        Parameters: reg: address of the register to read
        Return: int data
    */
    int readRegS16(uint8_t reg) {
        uint16_t res;
        readReg(reg, &res);
        return res;
    }

    /**
        Write a MLX90615 register. If its an EEPROM register,
        please call twice: first with 0x0000, second with desired value
        @param MLXaddr: MLX90615 EEPROM/RAM address
        @param value: ... to be writen
        @return: status:   0  OK
                         -1  Bad CRC calc
                         -2  I2C Error
                        -10  I2C Connector not specified yet
    */
    int writeReg(uint8_t MLXaddr, uint16_t value) {
        // CRC calculation
        cmd = MLXaddr;
        dataLow = value & 0xff;
        dataHigh = (value >> 8) & 0xff;
        pec = crc8Msb(0x07, buffer, 4);
        if (crc8Msb(0x07, buffer, 5)) {
            return -1;
        }

        int sent = 0;
        if (bus && !wbus) {
            // Using alternative I2C library
            bus->start(dev | I2C_WRITE);
            for (int i = 1 ; i <= 4  ; i++) {
                if (bus->write(buffer[i])) {
                    sent++;
                } else {
                    break;
                }
            }
            bus->stop();
            if (sent != 4) {
                return -2;
            }
            return 0;
        } else if (wbus && !bus) {
            // Using Wire
            wbus->beginTransmission(i2c_addr);
            sent = wbus->write(&buffer[1], 4);
            wbus->endTransmission();
            if (sent != 4) {
                return -2;
            }
            return 0;
        } else {
            return -10;
        }
    }

    /**
        Function Name: writeReg8 - DEPRECATED! (use writeReg)
        Description:  i2c write register one byte
        Parameters:
        Return:
    */
    void writeReg8(uint8_t data, uint8_t reg) {
        writeReg(reg, data);
    }

    /**
        Function Name: writeReg16 - DEPRECATED! (use writeReg)
        Description:  i2c write register int data
        Parameters:
        Return:
    */
    void writeReg16(uint16_t data, uint8_t reg) {
        writeReg(reg, data);
    }
};
#endif // __MLX90615_H__
