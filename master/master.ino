//Master

#include <SD.h>//Библиотека для SD карты
#include <SPI.h>
#include <Wire.h>
#include <OneWire.h>// Библиотека для DS18B20
#include <BH1750.h>
#include <DS1302.h> //подключение библиотеки для работы с часами
#define SensorPin A0            //pH пин к которому подключен pH датчик
#define Offset -4.0            //Калибровка
#define samplingInterval 20   //Интервал замеров pH датчика
#define ArrayLenth  40    //число замеров pH датчика 
#define OPROS_SLAVE 2//0-раз в пол минуты,1-раз в минуту,2-раз в две минуты,3-раз в пять минуть
OneWire  ds_1(22); //Подключение к 22(D) пину
OneWire  ds_2(23); //Подключение к 23(D) пину
int pHArray[ArrayLenth];   //Масств в котором будут хранится замеры
int pHArrayIndex=0;    
  BH1750 lightMeter;
  int tim_sec1;
  int tim_sec2;
  int tim_min;
  
  int EN = 2;//Пин к которому соеденино rs
  int echoPin = 9;//Пин для дальнометра
  int trigPin = 8;//Пин для дальнометра
  const uint8_t PIN_CS = 10; //Пин для сд модуля
  String Val;//Строка,в которую записывается информация от нано без дальнометра
  String Val2;//Строка,в которую записывается информация от нано с дальнометром
  int  rel_1 = 0;
  int  rel_2 = 0;
  int  rel_3 = 0;
  int  rel_4 = 0;
  long unsigned int count=1;//Счетчик номера записи в таблицу  
  int count_=0;
  long int Pulsar;
  uint16_t lux;
  int flag_ds_1=0;//Если датчик считал успешно - 0,если нет - 1,2-неправильный адрес
  int flag_ds_2=0;//Если датчик считал успешно - 0,если нет - 1,2-неправильный адрес
  float celsius_1;
  float celsius_2;
  File myFile; // Файл для таблицы
  DS1302 rtc(7, 12, 11); //создание объекта часы RST DAT CLK
  Time t;
void setup()
{
  switch(OPROS_SLAVE)
  {
    case 0:
  tim_sec1=29;
  tim_sec2=59;
  tim_min=1;
  break;
  case 1:
  tim_sec1=29;
  tim_sec2=29;
  tim_min=1;
  break;
  case 2:
  tim_sec1=29;
  tim_sec2=29;
  tim_min=2;
  break;
  case 3:
  tim_sec1=29;
  tim_sec2=29;
  tim_min=5;
  break;
  }
  rtc.halt(false); //запуск часов
  rtc.writeProtect(false); //снять защиту от записи
  //Установка даты
  //Установка даты установка происходит следующим образом: 1) устанавливаем нужное время/дату---запускаем скетч: время при закрытии порта сбрасывается и начинается отсчет с установленного
 // rtc.setDOW(TUESDAY); //установка дня недели (только на английском)                                         2) комментируем эти строчки(что сделано сейчас) и запускаем скетч---------- время сохраняется- часы работают в автономном режиме
 // rtc.setTime(3, 12, 0); //установка времени (часы, минуты, секунды)
 // rtc.setDate(25, 7, 2017); //Дата цифрами. "0" перед одиночной цифрой можно не ставить
  pinMode(23, OUTPUT );//реле
  pinMode(24, OUTPUT );//реле
  pinMode(25, OUTPUT );//реле
  pinMode(26, OUTPUT );//реле
  digitalWrite (23, rel_1 ); //реле
  digitalWrite (24, rel_2 ); //реле
  digitalWrite (25, rel_3 ); //реле
  digitalWrite (26, rel_4 ); //реле
  lightMeter.begin();
  if(!SD.begin(PIN_CS)){                           // инициализация SD карты с указанием номера вывода CS
    Serial.println("SD-card not found");   // ошибка инициализации. карта не обнаружена или не подключён (неправильно подключён) адаптер карт MicroSD
    }
  if(!SD.exists("test3.csv"))//Если файла на флешке нет,то создаем его и печатаем шапку,иаче только открывае
    {
    myFile = SD.open("test3.csv", FILE_WRITE);//Открытие или создание файла,где хранится информация
    Writing_Caps();//Функция написания шапки
    }
  else // иначе если файл есть
    myFile = SD.open("test3.csv", FILE_WRITE);//Открытие или создание файла,где хранится информация
  myFile.close();//Закрытие файла
  pinMode(EN, OUTPUT );
  pinMode(trigPin, OUTPUT); //для сонара 
  pinMode(echoPin, INPUT); 
  Serial1.begin (19200);
  Serial.begin(9600);
  Serial1.setTimeout(1000);
}

void loop()  
{
  
   
  t = rtc.getTime(); //Инициализация времени
  if((t.sec%4==0)&&(t.sec<50))
  {
    ReadSensor();
  }

  if((t.sec==tim_sec1)||(t.sec==tim_sec2)||(t.min==tim_min))
  {
     //Инициализация передачи на Slave-2
  digitalWrite (EN, HIGH ); //передача
  delay(10);
  Serial1.print("S2"); //Вызов Slave_2
  delay(10);
  digitalWrite (EN, LOW ); //прием
  delay(10);
  int S2_count=0;
   Val2 = Serial1.readStringUntil('\n');//считывание
      while((Val.indexOf("L") < 0)&&(S2_count < 1)) 
      {
         digitalWrite (EN, HIGH ); //передача
         delay(10);
         Serial1.print("S2");
         delay(10);
         digitalWrite (EN, LOW ); //прием
         delay(20);
         Val2 = Serial1.readStringUntil('\n');//считывание
         S2_count++; 
      }
      if (Val2.indexOf("L") >= 0)
       Val2.setCharAt(0,' ');
    else
      {
        Serial.println("Slave-2 offline...");
        Val2+=";+;+;+;+;+;+;+;+;+;+;";
      }
   //Инициализация передачи на Slave-1
   digitalWrite (EN, HIGH ); //передача
   delay(10);
   Serial1.print("S1");
   delay(10);
   digitalWrite (EN, LOW ); //прием
   delay(20);
   int S1_count=0;
   Val = Serial1.readStringUntil('\n');//считывание
     while((Val.indexOf("H") < 0)&&(S1_count < 1)) 
      {
         digitalWrite (EN, HIGH ); //передача
         delay(10);
         Serial1.print("S1");
         delay(10);
         digitalWrite (EN, LOW ); //прием
         delay(20);
         Val = Serial1.readStringUntil('\n');//считывание
         S1_count++; 
      }
      if (Val.indexOf("H") >= 0)
       Val.setCharAt(0,' ');
      else
     {
      Serial.println("Slave_1 offline");
      Val+=";+;+;+;+;+;+;";
     }
      
    WriteFile();  
  } 
}

void Writing_Caps()
{
  if (myFile) 
  {
    String OutPut  = "Number;;Slave1(Meteo);Slave1(Meteo);Slave1(Meteo);Slave1(Meteo);Slave1(Meteo);Slave1(Meteo);Master;Master;Master;Master;Master;Master;Master;Master;Master;Master;Master;Master;Master;Slave2(Term_stat);Slave2(Term_stat);Slave2(Term_stat);Slave2(Term_stat);Slave2(Term_stat);Slave2(Term_stat);Slave2(Term_stat);Slave2(Term_stat);Slave2(Term_stat);Slave2(Term_stat)";  
    myFile.println(OutPut);
    OutPut  =";DS1307;BH-1750_1 ; BH-1750_2;BMP280;AM-2320;AM-2320;DS18B20;BH-1750;DS18B20;DS18B20;pH;DO Sensor;DCO2 Sensor;HC _SR04;SSR_1;SSR_2;SSR_3;SSR_4;digipot_1;digipot_2;HC_SR04;DS18B20;DS18B20;DS18B20;DS18B20;DS18B20;SSR_1;SSR_2;digipot_1;digipot_2";
    myFile.println(OutPut);
    OutPut  =";Time;True Light of Sun;True Light of Sun;Pressure;Temp;Humidity;Temp;Light;Temp In;Temp In;pH;O ppm;CO2 ppm;distance(mm);on/off(1 or 0);on/off(1 or 0);on/off(1 or 0);on/off (1 or 0);Status 0-255;Status 0-255;distance(mm);Temp_1;Temp_2;Temp_3;Temp_4;Temp_5;on/off(1 or 0);on/off(1 or 0);Status 0-255;Status 0-255";
    myFile.println(OutPut);            
  }
}

int PH_Metering()
{
    static float pHValue,voltage;
    for(int i=0;i<40;i++)
      {       
        pHArray[pHArrayIndex++]=analogRead(SensorPin);
        if(pHArrayIndex==ArrayLenth)pHArrayIndex=0;
        voltage = avergearray(pHArray, ArrayLenth)*5.0/1024;
        pHValue = 3.5*voltage+Offset;
      }
  /*  Serial.print("    pH value: ");
    Serial.println(pHValue,2);*/
   pHValue=pHValue*100;
    return(pHValue);
}

double avergearray(int* arr, int number){
  int i;
  int max,min;
  double avg;
  long amount=0;
  if(number<=0)
  {
    Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if(number<5)
  {   //less than 5, calculated directly statistics
    for(i=0;i<number;i++){
      amount+=arr[i];
   }
    avg = amount/number;
    return avg;
  }
  else
  {
    if(arr[0]<arr[1])
    {
      min = arr[0];max=arr[1];
    }
    else
    {
      min=arr[1];max=arr[0];
    }
    for(i=2;i<number;i++){
      if(arr[i]<min){
        amount+=min;        //arr<min
        min=arr[i];
      }else {
        if(arr[i]>max){
          amount+=max;    //arr>max
          max=arr[i];
        }else{
          amount+=arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount/(number-2);
  }//if
  return avg;
}

 int Pulsar_()
 {
      int duration, mm; 
      digitalWrite(trigPin, LOW); 
      delayMicroseconds(2); 
      digitalWrite(trigPin, HIGH); 
      delayMicroseconds(10); 
      digitalWrite(trigPin, LOW); 
      duration = pulseIn(echoPin, HIGH); 
      return (duration / 5.8);
}

  void WriteFile()
  {
      String test; // Инициализация буфера
      test+=count;//Счетчик строк
      test +=";";
      count++;
      test +=t.hour; //получить время
      test +=':';
      test +=t.min;
      test +=':';
      test +=t.sec;
      test+=';';
      test +=Val;
      test+=lux/count_;//Датчик мастера BH-1750
      test+=';';
      //Датчик мастера DS18B20
      if (flag_ds_1==1) 
     {
        test+="CRC is not valid!";
     }
      if (flag_ds_1==2) 
     {     
        test+="No more addresses."; 
     }
      if (flag_ds_1==0) 
     {     
        test+=celsius_1/count_;
     }
      test+=';';
     //Датчик мастера DS18B20
      if (flag_ds_2==1) 
     {
        test+="CRC is not valid!";
     }
      if (flag_ds_2==2) 
     {     
        test+="No more addresses."; 
     }
     if (flag_ds_2==0) 
     {     
        test+=celsius_2/count_;
     }
      test+=';';
      int PH =  PH_Metering();
      int PH_2  = PH/100;
      test+=PH_2;//Датчик мастера PH
      test+=',';
      if (PH%100<10)
        test+='0';
      test+=PH % 100;
      test+=';';
      test+='-';//Датчик мастера 0 ppm
      test+=';';
      test+='-';//Датчик мастера C02 ppm
      test+=';';    
      test+=Pulsar/count_;//Датчик мастера растояние
      test+=";";
      test+=rel_1;//Мастер SSR_1
      test+=';';
      test+=rel_2;//Мастер SSR_2
      test+=';';
      test+=rel_3;//Мастер SSR_3
      test+=';';
      test+=rel_4;//Мастер SSR_4
      test+=';'; 
      test+='-';//Мастер digipot_1
      test+=';';
      test+='-';//Мастер digipot_2
      test+=';';  
      test+=Val2;
      Serial.println(test);
      myFile = SD.open("test3.csv", FILE_WRITE);//Открытие или создание файла,где хранится информация
      myFile.print(test);
      myFile.println();
      myFile.close();
      count_=0;
      Pulsar=0;
      flag_ds_2=0;
      celsius_2=0; 
      flag_ds_1=0;
      celsius_1=0;
  }
void ReadSensor()
{
  count_++;
  Pulsar+=Pulsar_();
  lux += lightMeter.readLightLevel();
  celsius_1+=DS_READ(ds_1,flag_ds_1);
  celsius_2+=DS_READ(ds_2,flag_ds_2);
}
float DS_READ(OneWire ds,bool flag_ds)
{
//DS18B20
      byte i;
      byte present = 0;
      byte type_s;
      byte data[12];
      byte addr[8];    
      float celsius;
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
      celsius = (float)raw / 16.0;
      
}


