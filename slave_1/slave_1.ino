//Slave_1

#include <Wire.h>
#include <BH1750.h>
#include "BMP280.h"
#include <OneWire.h>// Библиотека для DS18B20
#include <SoftwareWire.h> //программный i2c, т. к. адрес AM2320 конфликтует с адресом BH1750
#include <AM2320.h> // подключаем библиотеку для работы с датчиком температуры и влажности 
#include <SoftwareSerial.h>
int EN = 2;//Пин для RS
String Val;//Строка,которая считывает переданные данные
String Str;//Строка,в которую нужно передать 
BMP280 bmp;
BH1750 lightMeter1;//Датчик 1 с адресом 0x23
BH1750 lightMeter2(0x5c);//Датчик 2 с адресом 0x5c 
AM2320 sensor1(5,6); // AM2320 sensor, SDA - 5 pin, SCL - 6 pin 
SoftwareSerial mySerial(4, 3); // RX, TX*/
OneWire  ds(8); //Подключение к 8(D) пину
uint16_t lux1;//Число,в которое будет суммироваться все освещение с 1 датчика
uint16_t lux2;//Число,в которое будет суммироваться все освещение со 2 датчика
double T,P;//Число,в которое будет считываться давление с датчика bmp 
double Pres;//Число,в которое будет суммироваться всё давление с датчкика bmp
double t,h;//Числа,в которые будут суммироваться вся температура и влажность с датчика AM2320
char result;
float celsius;//Число,в которое будет суммироваться вся температура с датчка 
bool flag_bmp=0;//Если датчик считал успешно - 0,если нет - 1
bool flag_ds=0;//Если датчик считал успешно - 0,если нет - 1
int flag_am=0;
int count=0;//Счетчик измерений для вычисления среднего значения

void setup(){
  pinMode(EN, OUTPUT );//Пин для RS
  mySerial.begin(19200);
  mySerial.setTimeout(1000);
  Serial.begin(9600);
  pinMode(7,OUTPUT);//Пин для адресации BH1750
  digitalWrite(7,HIGH);
  lightMeter1.begin();//Инициализация 1 датчика BH1750
  lightMeter2.begin();//Инициализация 2 датчика BH1750
  bmp.begin();//Инициадизация датчика bmp
  bmp.setOversampling(4);
}

void loop() {
  Str = "";//Строка,которая отправит информацию
  digitalWrite(EN, LOW ); // Включение приема
  delay(10);
 Val = mySerial.readStringUntil('\n');
  if(Val.indexOf("S1") >= 0) //Если пришел сигнал для передачи данных
  {   
    digitalWrite(EN, HIGH );//Включение передачи
    delay(10);
    mySerial.print("H"); 
    Str+=lux1/count;
    Str+=';';
    Str+=lux2/count;
    Str+=';';
  //BMP  
    if(flag_bmp==0)
    {  
      //Str+=T;
     // Str+=';';
      Str+=Pres/count;
      Str+=';';    
    }
  else
  {
   // Str+='-';
   // Str+=';';
    Str+='-';
    Str+=';';
  }
  flag_bmp=0;
  //AM2320
  switch(flag_am) {
    case 2:
      Str+="CRC failed";
      Str+=';';
      Str+="CRC failed";
      Str+=';';
      break;
    case 1:
      Str+="Sensor1 offline";
      Str+=';';
      Str+="Sensor1 offline";
      Str+=';';
      break;
    case 0:
      Str+=t/count;
      Str+=';';
      Str+=h/count;
      Str+=';';
      break;
    }
    flag_am=0; 
  //DS18B20
     if (flag_ds==1) 
     {
        Str+="CRC is not valid!";
        Str+=';';
     }
     else
     {     
        Str+=celsius/count;
        Str+=';';
     }  
    digitalWrite(EN, HIGH );//Включение передачи
    delay(10);
    mySerial.print(Str);//Передача строки
    delay(1000);
    lux1=0;
    lux2=0;
    Pres=0;
    t=0;
    h=0;
    celsius=0;
    count=0;
  }
  else
    if(Val.indexOf("WR") >= 0)
    {  
      count++;
      lux1 += lightMeter1.readLightLevel(); 
      lux2 += lightMeter2.readLightLevel();
      result = bmp.startMeasurment(); 
      if(result!=0)
      {  
        delay(result);
        result = bmp.getTemperatureAndPressure(T,P);
        P=P*100; 
        Pres+=P;
      }
      else
        flag_bmp=1;   
      switch(sensor1.Read()) 
      {
      case 2:
        flag_am=2;
        break;
      case 1:
        flag_am=1;
        break;
      case 0:
        flag_am=0;
        t+=sensor1.t;
        h+=sensor1.h;
        break;
      }
 //DS18B20
      byte i;
      byte present = 0;
      byte type_s;
      byte data[12];
      byte addr[8];    
      int q=0;
      while( !ds.search(addr)&&q<2)//Пока адрес неправильный,продолжать проверку адреса!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        {ds.reset_search();
        q++;
        }
      if (OneWire::crc8(addr, 7) != addr[7])
        flag_ds=1;
      ds.reset();
      ds.select(addr);
      ds.write(0x44, 1);
  //delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
      present = ds.reset();
      ds.select(addr);    
      ds.write(0xBE);         // Read Scratchpad
      for ( i = 0; i < 9; i++) 
      {           // we need 9 bytes
        data[i] = ds.read();
      }
      int16_t raw = (data[1] << 8) | data[0];
      if (type_s) 
      {
       raw = raw << 3; // 9 bit resolution default
       if (data[7] == 0x10) 
       {// "count remain" gives full 12 bit resolution
        raw = (raw & 0xFFF0) + 12 - data[6];
       }
      }
      else 
      {
        byte cfg = (data[4] & 0x60);
        // at lower res, the low bits are undefined, so let's zero them
        if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
        else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
        else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
      }
      celsius += (float)raw / 16.0;
      Serial.println(celsius);
 }
}
