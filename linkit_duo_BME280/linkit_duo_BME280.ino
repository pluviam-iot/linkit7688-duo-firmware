#include "Seeed_BME280.h"
#include <Wire.h>

BME280 bme280;

#define PIN_WIND_SPEED 9
#define PIN_WIND_DIRECTION 3
#define PIN_BATTERY 0

volatile unsigned int rain = 0;
volatile unsigned long lastInterruptTimeRain = 0;
volatile unsigned long interruptTimeRain = 0;
      
      void setup() { 
        pinMode(PIN_WIND_SPEED, INPUT);
          Serial1.begin(9600);  // open internal serial connection to MT7688 
          Serial.begin(9600);
            if(!bme280.init()){
    Serial1.println("Device error!");
  }
  delay(2000);
    attachInterrupt(4, interruptRain, FALLING);
    Serial.println("START");
      }

      
      void loop() {
            byte commandByte = Serial1.read();      // read from MT7688
                  if (commandByte == 'A') {
                        Serial1.print(bme280.getTemperature());
                  } else if (commandByte == 'B') {
                        Serial1.print(bme280.getHumidity());  
                  } else if (commandByte == 'C') {
                        Serial1.print(bme280.getPressure());  
                  } else if (commandByte == 'D') {
                        Serial1.print(getRain());  
                  } else if (commandByte == 'E') {
                        Serial1.print(getWindDirection());  
                  } else if (commandByte == 'G') {
                        Serial1.print(getWindInterval());
                  } else if (commandByte == 'K') {
                        Serial1.print(getBattery());
                  } else if (commandByte == '+') {
                        Serial1.print("ABCDEGK");
                  }
                  delay(100);
      }





 void interruptRain()
{
    interruptTimeRain = millis();
  // If interrupts come faster than 100ms, assume it's a bounce and ignore
  if (interruptTimeRain - lastInterruptTimeRain > 100)
  {
    rain++;  

    Serial.println("rain");
  }
  lastInterruptTimeRain = interruptTimeRain;
}

unsigned int getRain(){
  unsigned int result = rain;
  rain = 0;
  return result;
}

int getWindInterval () {
  unsigned long lastInterruptTimeWind = 0;
  unsigned long interruptTimeWind = 0;
  unsigned long start = millis();    
  bool done = false;
  bool timeout = false;
  int result = -1;
  bool pinStatus = digitalRead(PIN_WIND_SPEED);
  bool lastStatus = digitalRead(PIN_WIND_SPEED);
  bool firstStep = true;
  
  while (!timeout && !done) {
    
    pinStatus = digitalRead(PIN_WIND_SPEED);

    if (lastStatus != pinStatus) {
      if (pinStatus == HIGH){
        if (firstStep) {
          interruptTimeWind = millis();
          firstStep = false;
        } else {
          lastInterruptTimeWind = millis();
          done = true;
        }
      }
    }
    lastStatus = pinStatus;

    if (millis() - start > 2000) {      
      timeout = true;
    } 
  }
  if (lastInterruptTimeWind > 0) {
    result = lastInterruptTimeWind - interruptTimeWind;    
  }
  return result;
}

int getWindDirection (){
  return analogRead(PIN_WIND_DIRECTION);
}

int getBattery (){
  return analogRead(PIN_BATTERY);
}


