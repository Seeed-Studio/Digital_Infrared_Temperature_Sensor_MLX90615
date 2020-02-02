#include "MLX90615.h"

#define MLX MLX90615_DefaultAddr

/*
    Uncomment the following line to use included I2C library

    Avoid (keep commented) if your I2C bus is being shared with
    other devices: they may be using Wire as its basis,
    incompatible with the included I2C library
*/
// #define INCLUDED_I2C

#ifdef INCLUDED_I2C
    #define SDA_PIN SDA   //define the SDA pin
    #define SCL_PIN SCL   //define the SCL pin
    SoftI2cMaster i2c(SDA_PIN, SCL_PIN);
    MLX90615 mlx90615(MLX, &i2c);
#else // Using Wire
    MLX90615 mlx90615(MLX, &Wire);
#endif // INCLUDED_I2C not defined

void setup() {
    Serial.begin(9600);
    while (!Serial); // Only for native USB serial
    delay(2000); // Additional delay to allow open the terminal to see setup() messages
    Serial.println("Setup...");

    #ifndef INCLUDED_I2C // If using Wire:
    Wire.begin();
    #endif // INCLUDED_I2C not defined

    // // write data into EEPROM when you need to adjust emissivity.
    // Serial.println(mlx90615.writeReg(MLX90615_EEPROM_EMISSIVITY,0x0000)); // Erase! (and see result)
    // delay(10); // EEPROM Write/Erase time
    // Serial.println(mlx90615.writeReg(MLX90615_EEPROM_EMISSIVITY,Default_Emissivity)); // Desired
    // delay(10); // EEPROM Write/Erase time
    // mlx90615.readEEPROM(); //read EEPROM data to check whether it's a default one.
}

void loop() {
    Serial.print("Object temperature: ");
    Serial.println(mlx90615.getTemperature(MLX90615_OBJECT_TEMPERATURE));
    Serial.print("Ambient temperature: ");
    Serial.println(mlx90615.getTemperature(MLX90615_AMBIENT_TEMPERATURE));
    delay(1000);
}
