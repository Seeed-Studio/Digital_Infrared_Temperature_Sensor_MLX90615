#include "MLX90615.h"
#include <I2cMaster.h>

#define SDA_PIN 18 //define the SDA pin //18 = A4
#define SCL_PIN 19 //define the SCL pin //19 = A5

SoftI2cMaster i2c(SDA_PIN, SCL_PIN);
MLX90615 mlx90615(0x00, &i2c);

#define CUSTOM_ADDR 0x5D
#define SD_EEPROM_ADDR 0x10

void setup()
{
  Serial.begin(9600);
  Serial.println("Setup...");

  // 1. Change Addr
  int result = mlx90615.write(SD_EEPROM_ADDR, CUSTOM_ADDR);
  mlx90615.~MLX90615();

  if (result)
  {
    Serial.println(result);
    Serial.println("Error while changing addr");
  }
  else
    Serial.println("Addr changed to " + String(CUSTOM_ADDR, HEX));

  //2. Use NEW Addr
  mlx90615 = MLX90615(CUSTOM_ADDR, &i2c);
}

void loop()
{
  Serial.print("Object temperature: ");
  Serial.println(mlx90615.getTemperature(MLX90615_OBJECT_TEMPERATURE));
  Serial.print("Ambient temperature: ");
  Serial.println(mlx90615.getTemperature(MLX90615_AMBIENT_TEMPERATURE));

  delay(1000);
}