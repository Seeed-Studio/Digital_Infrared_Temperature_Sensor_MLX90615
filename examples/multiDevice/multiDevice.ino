/**
    This example covers the following options to communicate
    with additional MLX90615:

    1. Using additional pins (each MLX90615 has its own bus)
    2. Using a single bus for all MLX90615, only if each MLX
      has an unique address among the same I2C bus. Note that
      this sketch doesn't cover the address changing! See
      "changeAddr" for this.
*/

#include "MLX90615.h"

// #define BY_PINS // Uncomment this to use pins for every MLX
#define BY_ADDR // Uncomment this to use single bus

#ifdef BY_PINS // USING ADDITIONAL PINS
    byte sda_1 = 3;
    byte scl_1 = 2;
    byte sda_2 = 5;
    byte scl_2 = 4;
    byte sda_3 = 7;
    byte scl_3 = 6;

    SoftI2cMaster i2c_1(sda_1, scl_1);
    MLX90615 mlx90615_1(DEVICE_ADDR, &i2c_1);
    SoftI2cMaster i2c_2(sda_2, scl_2);
    MLX90615 mlx90615_2(DEVICE_ADDR, &i2c_2);
    SoftI2cMaster i2c_3(sda_3, scl_3);
    MLX90615 mlx90615_3(DEVICE_ADDR, &i2c_3);
#endif // BY_PINS


#ifdef BY_ADDR
    // TODO: Update with your real addresses and quantity of MLXs!
    #define DEVICE1_ADDR DEVICE_ADDR
    #define DEVICE2_ADDR DEVICE_ADDR+1
    #define DEVICE3_ADDR DEVICE_ADDR+2

    /*
    Uncomment the following line to use included I2C library

    Avoid (keep commented) if your I2C bus is being shared with
    other devices: they may be using Wire as its basis,
    incompatible with the included I2C library
    */
    // #define INCLUDED_I2C

    #ifdef INCLUDED_I2C // Using included I2C Library
        #define SDA_PIN SDA   //define the SDA pin
        #define SCL_PIN SCL   //define the SCL pin
        SoftI2cMaster i2c_bus(SDA_PIN, SCL_PIN);
        MLX90615 mlx90615_1(DEVICE1_ADDR, &i2c_bus);
        MLX90615 mlx90615_2(DEVICE2_ADDR, &i2c_bus);
        MLX90615 mlx90615_3(DEVICE3_ADDR, &i2c_bus);
    #else // Using Wire
        MLX90615 mlx90615_1(DEVICE1_ADDR, &Wire);
        MLX90615 mlx90615_2(DEVICE2_ADDR, &Wire);
        MLX90615 mlx90615_3(DEVICE3_ADDR, &Wire);
    #endif // WITH_WIRE defined
#endif // BY_ADDR defined

void setup() {
    Serial.begin(9600);
    while (!Serial); // Only for native USB serial
    delay(2000); // Additional delay to allow open the terminal to see setup() messages
    Serial.println("Setup...");

    #ifdef WITH_WIRE
    Wire.begin();
    #endif // WITH_WIRE defined
}

void loop() {
    float temperatureObj1 = mlx90615_1.getTemperature(MLX90615_OBJECT_TEMPERATURE);
    float temperatureObj2 = mlx90615_2.getTemperature(MLX90615_OBJECT_TEMPERATURE);
    float temperatureObj3 = mlx90615_3.getTemperature(MLX90615_OBJECT_TEMPERATURE);
    float temperatureAmb1 = mlx90615_1.getTemperature(MLX90615_AMBIENT_TEMPERATURE);
    float temperatureAmb2 = mlx90615_2.getTemperature(MLX90615_AMBIENT_TEMPERATURE);
    float temperatureAmb3 = mlx90615_3.getTemperature(MLX90615_AMBIENT_TEMPERATURE);

    Serial.print("Temp_1: ");
    Serial.print(temperatureObj1);
    Serial.print("°C  ");
    Serial.print(temperatureAmb1);
    Serial.println("°C  ");

    Serial.print("Temp_2: ");
    Serial.print(temperatureObj2);
    Serial.print("°C  ");
    Serial.print(temperatureAmb2);
    Serial.println("°C  ");

    Serial.print("Temp_3: ");
    Serial.print(temperatureObj3);
    Serial.print("°C  ");
    Serial.print(temperatureAmb3);
    Serial.println("°C  ");

    Serial.println("\n=======================================\n\r");

    delay(1000);
}
