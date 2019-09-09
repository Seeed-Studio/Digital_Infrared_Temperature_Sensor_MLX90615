#include "MLX90615.h"

// Way 1: Using additional pins

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

// // Way 2: If you have set your MLX90615s up to have unique
// // addresses, like in the "changeAddr.ino" sketch, you can
// // have all your MLX90615 sharing same pins/bus

// // Update with your real addresses
// #define DEVICE1_ADDR DEVICE_ADDR
// #define DEVICE2_ADDR DEVICE_ADDR+1
// #define DEVICE3_ADDR DEVICE_ADDR+2

// // // Using included I2C lib
// // #define SDA_PIN SDA   //define the SDA pin
// // #define SCL_PIN SCL   //define the SCL pin
// // SoftI2cMaster i2c_bus(SDA_PIN, SCL_PIN);
// // MLX90615 mlx90615_1(DEVICE1_ADDR, &i2c_bus);
// // MLX90615 mlx90615_2(DEVICE2_ADDR, &i2c_bus);
// // MLX90615 mlx90615_3(DEVICE3_ADDR, &i2c_bus);

// // Using Wire
// MLX90615 mlx90615_1(DEVICE1_ADDR, &Wire);
// MLX90615 mlx90615_2(DEVICE2_ADDR, &Wire);
// MLX90615 mlx90615_3(DEVICE3_ADDR, &Wire);


void setup()
{
  Serial.begin(9600);
  while(!Serial); // Only for native USB serial
  delay(2000); // Additional delay to allow open the terminal to see setup() messages
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
