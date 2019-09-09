#include "MLX90615.h"

#define MLX MLX90615_DefaultAddr

// Uncomment this for included I2C library (may
// interfer if you use other I2C devices in same
// pins and their implementations use Wire)
// #define SDA_PIN SDA   //define the SDA pin
// #define SCL_PIN SCL   //define the SCL pin
// SoftI2cMaster i2c(SDA_PIN, SCL_PIN);
// MLX90615 mlx90615(MLX, &i2c);

// Comment this for old way (with included I2C lib.)
MLX90615 mlx90615(MLX, &Wire);

void setup()
{
  Serial.begin(9600);
  while(!Serial); // Only for native USB serial
  delay(2000); // Additional delay to allow open the terminal to see setup() messages
  Serial.println("Setup...");

  // Comment this for old way (with included I2C lib.)
  Wire.begin();

  // // write data into EEPROM when you need to adjust emissivity.
  // Serial.println(mlx90615.writeReg(MLX90615_EEPROM_EMISSIVITY,0x0000)); // Erase! (and see result)
  // delay(10); // EEPROM Write/Erase time
  // Serial.println(mlx90615.writeReg(MLX90615_EEPROM_EMISSIVITY,Default_Emissivity)); // Desired
  // delay(10); // EEPROM Write/Erase time
  // mlx90615.readEEPROM(); //read EEPROM data to check whether it's a default one.
}

void loop()
{
  Serial.print("Object temperature: ");
  Serial.println(mlx90615.getTemperature(MLX90615_OBJECT_TEMPERATURE));
  Serial.print("Ambient temperature: ");
  Serial.println(mlx90615.getTemperature(MLX90615_AMBIENT_TEMPERATURE));
  delay(1000);
}
