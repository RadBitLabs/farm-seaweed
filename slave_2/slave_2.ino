//Скетч для слейва 2
#include <OneWire.h>// Библиотека для DS18B20
int echoPin = 9;//Эхо амн
int trigPin = 8;//Триг пин
int EN = 2;//Iei aey RST
String Val;//Строка которую получим
String Str;//Строка котурую будем передавать
int rel_1 = 0;
int rel_2 = 0;
OneWire  ds1(3); //Подключение к 3(D) пину
OneWire  ds2(4); //Подключение к 4(D) пину
OneWire  ds3(5); //Подключение к 5(D) пину
OneWire  ds4(6); //Подключение к 6(D) пину
OneWire  ds5(7); //Подключение к 7(D) пину
void setup() 
{ 
  pinMode(EN, OUTPUT );
  Serial.begin (19200);
  Serial.setTimeout(1000);
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
  digitalWrite(13, HIGH ); 

}

void loop()    
{ 
  Str = "";
  digitalWrite(EN, LOW ); // Включили прием
  delay(10);
  Val = Serial.readStringUntil('\n');
  if(Val.indexOf("S2") >= 0) 
    {
     digitalWrite(EN, HIGH );//Включили передачу
     delay(10);  
     Serial.print("L"); 
     int duration, mm; 
     digitalWrite(trigPin, LOW); 
     delayMicroseconds(2); 
     digitalWrite(trigPin, HIGH); 
     delayMicroseconds(10); 
     digitalWrite(trigPin, LOW); 
     duration = pulseIn(echoPin, HIGH); 
     mm = duration / 5.8;
     Str+=mm;
     Str+=';';
     //Str+=ds_(ds1);// DS18B20 Temp 1
      Str+='-';
     Str+=';';
     //Str+=ds_(ds2);// DS18B20 Temp 2
     Str+='-';
     Str+=';';
     //Str+=ds_(ds3);// DS18B20 Temp 3
     Str+='-';
     Str+=';';
     //Str+=ds_(ds4);// DS18B20 Temp 4
      Str+='-';
     Str+=';';
     //Str+=ds_(ds5);// DS18B20 Temp 5
     Str+='-';
     Str+=';';
     Str+=rel_1;//Iaii n naini?ii SSR_1
     Str+=';';
     Str+=rel_2;//Iaii n naini?ii SSR_2
     Str+=';';
     Str+='-';//Iaii n naini?ii digipot_1
     Str+=';';
     Str+='-';//Iaii n naini?ii digipot_2
     Str+=';';
     digitalWrite(EN, HIGH );//Включение передачи
      delay(10);
     Serial.print ( Str );
      delay(1000);
    }   
   
}
/*float ds_(OneWire ds)
{
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
      //Serial.println("CRC is not valid!");
      Str+="CRC is not valid!";
      Str+=';';
      return;
  }
  
  //delay(1000);     // maybe 750ms is enough, maybe not
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
  //Serial.print(" DS18B20 Temperature = ");
  //Serial.print(celsius);
 // Serial.println(" Celsius");
  //Serial.println();
  return(celsius);
}*/


