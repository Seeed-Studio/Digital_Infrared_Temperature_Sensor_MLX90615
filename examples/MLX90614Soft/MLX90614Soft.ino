#include "MLX90615.h"

MLX90615 mlx90615;

void setup()
{
	Serial.begin(9600);
	Serial.println("Setup...");
        mlx90615.init();
        mlx90615.writeEEPROM(Default_EEPROM_DATA);	
        mlx90615.readEEPROM();
}

void loop()
{ 
    mlx90615.printTemperature(MLX90615_OBJECT_TEMPERATURE);
    delay(1000);  
}
