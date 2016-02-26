#include "MLX90615.h"
#include <I2cMaster.h>

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


void setup()
{
  Serial.begin(9600);
  Serial.println("Setup...");
}

void loop()
{  
  float temperatureObj1 = mlx90615_1.getTemperature(MLX90615_OBJECT_TEMPERATURE);
  float temperatureObj2 = mlx90615_2.getTemperature(MLX90615_OBJECT_TEMPERATURE);
  float temperatureObj3 = mlx90615_3.getTemperature(MLX90615_OBJECT_TEMPERATURE);
  float temperatureAmb1 = mlx90615_1.getTemperature(MLX90615_AMBIENT_TEMPERATURE);
  float temperatureAmb2 = mlx90615_2.getTemperature(MLX90615_AMBIENT_TEMPERATURE);
  float temperatureAmb3 = mlx90615_3.getTemperature(MLX90615_AMBIENT_TEMPERATURE);
  
  Serial.print("Temp_1: ");
  Serial.print(temperatureObj1);
  Serial.print("`C  ");
  Serial.print(temperatureAmb1);
  Serial.println("`C  ");
  
  Serial.print("Temp_2: ");
  Serial.print(temperatureObj2);
  Serial.print("`C  ");
  Serial.print(temperatureAmb2);
  Serial.println("`C  ");
  
  Serial.print("Temp_3: ");
  Serial.print(temperatureObj3);
  Serial.print("`C  ");
  Serial.print(temperatureAmb3);
  Serial.println("`C  ");
  
  Serial.println("\n=======================================\n\r");
  
  delay(1000);
}
