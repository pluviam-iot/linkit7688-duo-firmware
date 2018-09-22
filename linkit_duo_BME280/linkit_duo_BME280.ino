#include "Seeed_BME280.h"
#include <Wire.h>

BME280 bme280;

#define PIN_WIND_SPEED 9
#define PIN_WIND_DIRECTION 3
#define PIN_BATTERY 0
#define WIND_MILLISECONDS_TO_READ 15000

//vars to use inside IRQs
volatile unsigned int rain = 0;
volatile unsigned long lastInterruptTimeRain = 0;
volatile unsigned long interruptTimeRain = 0;

//wind speed is not inside IRQs
unsigned int windPulses = 0;

void setup() {
  //open serial to computer 
  Serial.begin(9600);
  Serial.println("START");
  
  //open internal serial connection to MT7688 
  Serial1.begin(9600);
  
  pinMode(PIN_WIND_SPEED, INPUT);

  if(!bme280.init()){
    Serial1.println("Device error!");
    Serial.println("Device error!");
  }

  //wait a little to stabilize all sensors
  delay(2000);

  //attach rain interrupt
  attachInterrupt(4, interruptRain, FALLING);
  
}

void loop() {
  byte commandByte = Serial1.read();

  // respond just when receive 'A' command over Serial1 (internal serial to linkit)
  if (commandByte == 'A') {
    Serial1.print(bme280.getTemperature());
    //Serial1.print(20.2);
    Serial1.print(';');
    Serial1.print(bme280.getHumidity());
    //Serial1.print(85);
    Serial1.print(';');
    Serial1.print(bme280.getPressure());
    //Serial1.print(1013.25);
    Serial1.print(';');
    Serial1.print(getRain());
    Serial1.print(';');
    Serial1.print(getWindDirection());
    Serial1.print(';');
    Serial1.print(windPulses);
    Serial1.print(';');
    Serial1.print(getBattery());
    Serial1.print('/');

    /* after response was send, start measuring wind speed for next interaction.
     * First response after boot up wind speed is always 0
     */
    countWindPulses();
  }
  //wait use less energy
  delay(100);
}

void interruptRain() {
  //remember, millis will be frozen inside that function because its called by an IRQ
  interruptTimeRain = millis();

  // If interrupts come faster than 100ms, assume it's a bounce and ignore it
  if (interruptTimeRain - lastInterruptTimeRain > 1000) {
    rain++;
    //Serial.println("its raining!!!");
  }

  lastInterruptTimeRain = interruptTimeRain;
}

//return rain and empty var
unsigned int getRain(){
  unsigned int result = rain;
  rain = 0;
  return result;
}

// wind direction is an analog pin, so, just read and send.
int getWindDirection (){
  return analogRead(PIN_WIND_DIRECTION);
}

// analog, read and send too
int getBattery (){
  return analogRead(PIN_BATTERY);
}

// run after a serial response, its just counts all pulses in given interval time.
void countWindPulses() {
  windPulses = 0;
  unsigned long start = millis();
  bool timeout = false;
  bool pinStatus = LOW;
  bool lastStatus = LOW;

  while (!timeout) {
    pinStatus = digitalRead(PIN_WIND_SPEED);
    if (lastStatus == HIGH && pinStatus == LOW) {
      windPulses ++;
    }
    lastStatus = pinStatus;
    if (millis() - start > WIND_MILLISECONDS_TO_READ) {
      timeout = true;
    }
  }
}
