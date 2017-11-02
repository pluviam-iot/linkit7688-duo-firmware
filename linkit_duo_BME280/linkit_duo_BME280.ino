 #include "Seeed_BME280.h"
#include <Wire.h>

BME280 bme280;
      
      void setup() { 
          Serial1.begin(9600);  // open internal serial connection to MT7688 
            if(!bme280.init()){
    Serial1.println("Device error!");
  }
  delay(10000);
      }
      void loop() {
            byte commandByte = Serial1.read();      // read from MT7688
                  if (commandByte == 'A') {
                        Serial1.print(bme280.getTemperature());
                  } else if (commandByte == 'B') {
                        Serial1.print(bme280.getHumidity());  
                  } else if (commandByte == 'C') {
                        Serial1.print(bme280.getPressure());  
                  } else if (commandByte == '+') {
                        Serial1.print("ABC");
                  }
      }



