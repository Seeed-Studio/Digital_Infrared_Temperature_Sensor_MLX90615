
#define SDA_PORT PORTD
#define SDA_PIN 3   //define the SDA pin         
#define SCL_PORT PORTD
#define SCL_PIN 2   //define the SCL pin

#include "MLX90615.h"

MLX90615 mlx90615;

void setup()
{
  Serial.begin(9600);
  Serial.println("Setup...");
  mlx90615.init(); //initialize soft i2c wires
  
  //mlx90615.writeEEPROM(Default_Emissivity); //write data into EEPROM when you need to adjust emissivity.
  //mlx90615.readEEPROM(); //read EEPROM data to check whether it's a default one.
}

void loop()
{ 
    Serial.print("Object temperature: ");
    mlx90615.printTemperature(MLX90615_OBJECT_TEMPERATURE);
    Serial.print("Ambient temperature: ");
    mlx90615.printTemperature(MLX90615_AMBIENT_TEMPERATURE);
    delay(1000);  
}
