/**
 * Be 200% Sure of that the only I2C device connected is a MLX90615!
 *
 * The idea is to address first to 0x00 (every MLX90615 will answer
 * to this), update the EEPROM address 0x00 (register 0x10) by
 * clearing and setting the new address, and then, addressing with it
 */

#include "MLX90615.h"

// #define SDA_PIN SDA   //define the SDA pin
// #define SCL_PIN SCL   //define the SCL pin
// SoftI2cMaster i2c(SDA_PIN, SCL_PIN);
// MLX90615 mlx90615(0x00, &i2c);
MLX90615 mlx90615(0x00, &Wire);

#define CUSTOM_ADDR 0x5D
// #define CUSTOM_ADDR MLX90615_DefaultAddr // if needed to get back normal

void setup()
{
  Serial.begin(9600);
  while(!Serial); // Only for native USB serial
  delay(2000); // Additional delay to allow open the terminal to see setup() messages
  Serial.println("Setup...");
  Wire.begin(); // Comment this line if using included I2C library!

  int result;

  // 1. Change Addr
  result = mlx90615.writeReg(MLX90615_EEPROM_SA,0x0000); // Erase
  Serial.println(result);
  delay(10); // EEPROM Write/Erase time
  result = mlx90615.writeReg(MLX90615_EEPROM_SA,CUSTOM_ADDR); // Write Desired
  Serial.println(result);
  delay(10); // EEPROM Write/Erase time
  mlx90615.readEEPROM(MLX90615_EEPROM_SA); //read EEPROM data to check whether it's a default one.
  // Invoke the destructor
  mlx90615.~MLX90615();

  if (result)
  {
    Serial.println(result);
    Serial.println("Error while changing addr");
  }
  else
    Serial.println("Addr changed to " + String(CUSTOM_ADDR, HEX));

  //2. Use NEW Addr
  // mlx90615 = MLX90615(CUSTOM_ADDR, &i2c);
  MLX90615 mlx90615(CUSTOM_ADDR, &Wire);
}

void loop()
{
  Serial.print("Object temperature: ");
  Serial.println(mlx90615.getTemperature(MLX90615_OBJECT_TEMPERATURE));
  Serial.print("Ambient temperature: ");
  Serial.println(mlx90615.getTemperature(MLX90615_AMBIENT_TEMPERATURE));

  delay(1000);
}
