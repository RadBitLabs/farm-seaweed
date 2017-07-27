
//Master

#include <SD.h>//Библиотека для SD карты
#include <SPI.h>
#include <Wire.h>
#include <BH1750.h>
#include <LiquidCrystal_I2C.h>//для аботы с экраном
#include <DS1302.h> //подключение библиотеки для работы с часами
#define SensorPin A0            //pH пин к которому подключен pH датчик
#define Offset -4.0            //Калибровка
#define samplingInterval 20   //Интервал замеров pH датчика
#define ArrayLenth  40    //число замеров pH датчика


uint16_t Trs;//lux ds18b20
int CO_2;
int pH;
int DO;
double Ti;// ds18b20 temp in
double To;// ds8b20 temp out
int SpM; //digitpot1
int SpP; //digitpot2

int pHArray[ArrayLenth];   //Масств в котором будут хранится замеры
int pHArrayIndex=0;    
  BH1750 lightMeter;
  int ledPin=13;
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
  int count=1;//Счетчик номера записи в таблицу  
  File myFile; // Файл для таблицы
  LiquidCrystal_I2C lcd(0x3F,20,4);//работа с экраном 20x04
  DS1302 rtc(7, 12, 11); //создание объекта часы RST DAT CLK
  Time t;
void setup()
{
  rtc.halt(false); //запуск часов
  rtc.writeProtect(false); //снять защиту от записи
  //Установка даты
  //Установка даты установка происходит следующим образом: 1) устанавливаем нужное время/дату---запускаем скетч: время при закрытии порта сбрасывается и начинается отсчет с установленного
 // rtc.setDOW(TUESDAY); //установка дня недели (только на английском)                                         2) комментируем эти строчки(что сделано сейчас) и запускаем скетч---------- время сохраняется- часы работают в автономном режиме
 // rtc.setTime(3, 12, 0); //установка времени (часы, минуты, секунды)
 // rtc.setDate(25, 7, 2017); //Дата цифрами. "0" перед одиночной цифрой можно не ставить
 
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
  pinMode(ledPin, OUTPUT );
  pinMode(EN, OUTPUT );
  pinMode(trigPin, OUTPUT); //для сонара 
  pinMode(echoPin, INPUT); 
  Serial1.begin (19200);
  Serial.begin(9600);
  Serial1.setTimeout(1000);
}

void loop()  
{  
   digitalWrite (13, LOW );
   
   t = rtc.getTime(); //Инициализация времени
  
  //Инициализация передачи на Slave-2
//if(t.sec==3)
 // {
  digitalWrite (EN, HIGH ); //передача
  delay(10);
  Serial1.print("S2"); //Вызов Slave_2
  delay(10);
  digitalWrite (EN, LOW ); //прием
  delay(10);
   Val2 = Serial1.readStringUntil('\n');//считывание
     if(Val2.indexOf("L") >= 0) 
      {
        Val2.setCharAt(0,' ');
      }
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
   Val = Serial1.readStringUntil('\n');//считывание
     if(Val.indexOf("H") >= 0) 
      {
      Val.setCharAt(0,' ');
      }
      else
     {
      Serial.println("Slave_1 offline");
      Val+=";+;+;+;+;+;+;";
     }
      
    WriteFile();  
//  } 
}

void Writing_Caps()//шапка
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

 int Pulsar()
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
      uint16_t lux = lightMeter.readLightLevel();
      //Trs=lux;// значение для вывода на экран
      test+=lux;//Датчик мастера BH-1750
      test+=';';
      test+='-';//Датчик мастера DS18B20
      //Ti=     ;//вывод на экран temp in
      test+=';';
      test+='-';//Датчик мастера DS18B20
      //To=    ;//вывод на экран temp out
      test+=';';
      int PH =PH_Metering();
      //pH=PH;//вывод на экран 
      int PH_2=PH/100;
      test+=PH_2;//Датчик мастера PH
      test+=',';
      if (PH%100<10)
        test+='0';
      test+=PH % 100;
      test+=';';
      test+='-';//Датчик мастера 0 ppm
      test+=';';
      test+='-';//Датчик мастера C02 ppm
      //DO=CO2;// для вывода на экран
      test+=';';    
      test+=Pulsar();//Датчик мастера растояние
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
      //SpM=digitpot_1;//вывод на экран
      test+=';';
      test+='-';//Мастер digipot_2
      //SpP=digitpot_2;//вывод на экран
      test+=';';  
      test+=Val2;
      Serial.println(test);
      myFile = SD.open("test3.csv", FILE_WRITE);//Открытие или создание файла,где хранится информация
      myFile.print(test);
      myFile.println();
      myFile.close();
  }
  /*
    uint16_t Trs;//lux ds18b20
    int CO_2;
    int pH;
    int DO;
    double Ti;// ds18b20 temp in
    double To;// ds8b20 temp out
    int SpM; //digitpot1
    int SpP; //digitpot2
*/
 /*  
   void DisplayForDevices()
  {
      lcd.init();                     
      lcd.backlight();
      //=====1 столб данных=====//
      lcd.setCursor(0,0);
      lcd.print("Trs-");//100%
      lcd.setCursor(4,0);
      lcd.print(Trs);//
      lcd.setCursor(7,0);
      lcd.print("%");
      lcd.setCursor(0,1);
      lcd.print("CO2-");//200%
      lcd.setCursor(4,1);
      lcd.print(CO_2);
      lcd.setCursor(7,1);
      lcd.print("%");
      lcd.setCursor(1,2);
      lcd.print("pH-");//12.7
      lcd.setCursor(4,2);
      lcd.print(pH);
      lcd.setCursor(1,3);
      lcd.print("D0-");
      lcd.setCursor(4,3);
      lcd.print(DO);
      lcd.setCursor(6,3);
      lcd.print("ppm");
      //=====2 столб данных=====//
      lcd.setCursor(11,0);
      lcd.print("Ti-");
      lcd.setCursor(14,0);
      lcd.print(Ti);
      lcd.setCursor(18,0);
      lcd.print("C");
      lcd.setCursor(11,1);
      lcd.print("To-");
      lcd.setCursor(14,1);
      lcd.print(To);
      lcd.setCursor(18,1);
      lcd.print("C");
      lcd.setCursor(11,2);
      lcd.print("SpM-");
      lcd.setCursor(15,2);
      lcd.print(SpM);
      lcd.setCursor(18,2);
      lcd.print("%");
      lcd.setCursor(11,3);
      lcd.print("SpP-");
      lcd.setCursor(15,3);
      lcd.print(SpP);
      lcd.setCursor(18,3);
      lcd.print("%");
  }
  */
