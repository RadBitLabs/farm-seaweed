//Скетч для слейва 2
#include <OneWire.h>// Библиотека для DS18B20
#include <DallasTemperature.h>
int echoPin = 9;//Эхо амн
int trigPin = 8;//Триг пин
int EN = 2;//Iei aey RST
String Val;//Строка которую получим
String Str;//Строка котурую будем передавать
int rel_1 = 0;
int rel_2 = 0;
long int mm = 0;
float temp_1 = 0;
float temp_2 = 0;
float temp_3 = 0;
float temp_4 = 0;
float temp_5 = 0;

int count = 0;
OneWire oneWire(6); // вход датчиков 18b20
DallasTemperature ds(&oneWire);

DeviceAddress sensor1 = {0x28, 0xA8, 0x3E, 0xF9, 0x05, 0x0, 0x0, 0x12};
DeviceAddress sensor2 = {0x28, 0xE6, 0xBD, 0x3B, 0x05, 0x0, 0x0, 0xCF};
DeviceAddress sensor3 = {0x28, 0xE6, 0xBD, 0x3B, 0x05, 0x0, 0x0, 0xCF};
DeviceAddress sensor4 = {0x28, 0xE6, 0xBD, 0x3B, 0x05, 0x0, 0x0, 0xCF};
DeviceAddress sensor5 = {0x28, 0xE6, 0xBD, 0x3B, 0x05, 0x0, 0x0, 0xCF};

void setup() 
{ 
  pinMode(EN, OUTPUT );
  Serial.begin (19200);
  Serial.setTimeout(1000);
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
  digitalWrite(13, HIGH ); 
  ds.begin();
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
     if(mm/count==-1)
      Str+="Error";
     else
      Str+=mm/count;
     Str+=';';
     if(temp_1/count == -127.00)
      Str+="No found";
     else
       Str+=temp_1/count;// DS18B20 Temp 1
     Str+=';';
     if(temp_2/count == -127.00)
      Str+="No found";
     else
      Str+=temp_2/count;// DS18B20 Temp 2
     Str+=';';
     if(temp_3/count == -127.00)
      Str+="No found";
     else
      Str+=temp_3/count;// DS18B20 Temp 3
     Str+=';';
     if(temp_4/count == -127.00)
      Str+="No found";
     else
      Str+=temp_4/count;// DS18B20 Temp 4
     Str+=';';
     if(temp_5/count == -127.00)
      Str+="No found";
     else
      Str+=temp_5/count;// DS18B20 Temp 5
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
      mm=0;
      count=0;
      temp_1=0;
      temp_2=0;
      temp_3=0;
      temp_4=0;
      temp_5=0;
    }
    else
    {
      count++;
      int duration; 
     digitalWrite(trigPin, LOW); 
     delayMicroseconds(2); 
     digitalWrite(trigPin, HIGH); 
     delayMicroseconds(10); 
     digitalWrite(trigPin, LOW); 
     duration = pulseIn(echoPin, HIGH); 
     mm += duration / 5.8;
     ds.requestTemperatures(); // считываем температуру с датчиков
     temp_1+=ds.getTempC(sensor1);
     temp_2+=ds.getTempC(sensor2);
     temp_3+=ds.getTempC(sensor3);
     temp_4+=ds.getTempC(sensor4);
     temp_5+=ds.getTempC(sensor5);       
      
    }
    if(count>180)
    {
      mm = 0;
      count = 0;
      temp_1=0;
      temp_2=0;
      temp_3=0;
      temp_4=0;
      temp_5=0;
    }
}



