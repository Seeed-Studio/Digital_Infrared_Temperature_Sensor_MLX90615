/**
    Be 200% Sure of that the only I2C device connected is a MLX90615!

    The idea is to address first to 0x00 (every MLX90615 --and maybe
    anyother I2C device-- will answer to this address), then update
    the EEPROM address 0x00 (register 0x10) by clearing and setting
    the new address, and finally, addressing with this new I2C address
*/

#include "MLX90615.h"

/*
    Uncomment the following line to use included I2C library

    Avoid (keep commented) if your I2C bus is being shared with
    other devices: they may be using Wire as its basis,
    incompatible with the included I2C library
*/
// #define INCLUDED_I2C

#ifdef INCLUDED_I2C
    // #pragma message "Using included I2C"
    #define SDA_PIN SDA   //define the SDA pin
    #define SCL_PIN SCL   //define the SCL pin
    SoftI2cMaster i2c(SDA_PIN, SCL_PIN);
    MLX90615 mlx90615(0x00, &i2c);
#else // Using Wire
    MLX90615 mlx90615(0x00, &Wire);
#endif // INCLUDED_I2C not defined

#define CUSTOM_ADDR 0x5D

void setup() {
    Serial.begin(9600);
    while (!Serial); // Only for native USB serial
    delay(2000); // Additional delay to allow open the terminal to see setup() messages
    Serial.println("Setup...");

    #ifndef INCLUDED_I2C // If using Wire:
    Wire.begin();
    #endif // INCLUDED_I2C not defined

    int result;

    // 1. Change Addr
    result = mlx90615.writeReg(MLX90615_EEPROM_SA, 0x0000); // Erase
    Serial.println(result);
    delay(10); // EEPROM Write/Erase time
    result = mlx90615.writeReg(MLX90615_EEPROM_SA, CUSTOM_ADDR); // Write Custom Address
    // result = mlx90615.writeReg(MLX90615_EEPROM_SA,MLX90615_DefaultAddr);
    Serial.println(result);
    delay(10); // EEPROM Write/Erase time
    mlx90615.readEEPROM(MLX90615_EEPROM_SA); //read EEPROM data to check whether it's a default one.
    // Invoke the destructor
    mlx90615.~MLX90615();

    if (result) {
        Serial.println(result);
        Serial.println("Error while changing addr");
    } else {
        Serial.println("Addr changed to " + String(CUSTOM_ADDR, HEX));
    }

    //2. Use NEW Addr
    // mlx90615 = MLX90615(CUSTOM_ADDR, &i2c);
    MLX90615 mlx90615(CUSTOM_ADDR, &Wire);
}

void loop() {
    Serial.print("Object temperature: ");
    Serial.println(mlx90615.getTemperature(MLX90615_OBJECT_TEMPERATURE));
    Serial.print("Ambient temperature: ");
    Serial.println(mlx90615.getTemperature(MLX90615_AMBIENT_TEMPERATURE));

    delay(1000);
}
