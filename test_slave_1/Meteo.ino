/*
  Example of BH1750 library usage.
  This example initalises the BH1750 object using the default
  high resolution mode and then makes a light level reading every second.
  Connection:
    VCC -> 5V (3V3 on Arduino Due, Zero, MKR1000, etc)
    GND -> GND
    SCL -> SCL (A5 on Arduino Uno, Leonardo, etc or 21 on Mega and Due)
    SDA -> SDA (A4 on Arduino Uno, Leonardo, etc or 20 on Mega and Due)
    ADD -> (not connected) or GND
  ADD pin uses to set sensor I2C address. If it has voltage greater or equal to
  0.7VCC voltage (as example, you've connected it to VCC) - sensor address will be
  0x5C. In other case (if ADD voltage less than 0.7 * VCC) - sensor address will
  be 0x23 (by default).
*/

#include <Wire.h>
#include <BH1750.h>
#include "BMP280.h"
#include <OneWire.h>// Библиотека для DS18B20
#include <SoftwareWire.h> //программный i2c, т. к. адрес AM2320 конфликтует с адресом BH1750
#include <AM2320.h> // подключаем библиотеку для работы с датчиком температуры и влажности 
BMP280 bmp;
BH1750 lightMeter1;//Датчик 1 с адресом 0x23
BH1750 lightMeter2(0x5c);//Датчик 2 с адресом 0x5c

AM2320 sensor1(5,6); // AM2320 sensor, SDA - 5 pin, SCL - 6 pin
OneWire  ds(8); //Подключение к 8(D) пину

void setup(){
  Serial.begin(9600);
  pinMode(7,OUTPUT);
  digitalWrite(7,HIGH);
  lightMeter1.begin();
  lightMeter2.begin();
  bmp.begin();
  bmp.setOversampling(4);
}

void loop() {

  uint16_t lux1 = lightMeter1.readLightLevel();
   Serial.print("Light1: ");
  Serial.print(lux1);
  Serial.println(" lx1");
  delay(1000);
  uint16_t lux2 = lightMeter2.readLightLevel();
  Serial.print("Light2: ");
  Serial.print(lux2);
  Serial.println(" lx2");

  //BMP
   double T,P;
  char result = bmp.startMeasurment();
 
  if(result!=0){
    delay(result);
    result = bmp.getTemperatureAndPressure(T,P);
    P=P*100;
      if(result!=0)
      {    
        Serial.print("T = \t");Serial.print(T,2); Serial.print(" degC\t");
        Serial.print("P = \t");Serial.print(P); Serial.print(" mBar\t");
      }
  }

  //AM2320
  switch(sensor1.Read()) {
    case 2:
      Serial.println("CRC failed");
      break;
    case 1:
      Serial.println("Sensor1 offline");
      break;
    case 0:
      Serial.print("Humidity: ");
      Serial.print(sensor1.h);
      Serial.print("%\t Temperature: ");
      Serial.print(sensor1.t);
      Serial.println("*C");
      break;
  }


  //DS18B20
   byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;
  
  if ( !ds.search(addr)) {
    ds.reset_search();
    delay(250);
    return;
  }
  


  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
 

 
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

 
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  Serial.print(" DS18B20 Temperature = ");
  Serial.print(celsius);
  Serial.println(" Celsius");
  Serial.println();
  delay(1000);
}
