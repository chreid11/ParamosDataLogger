/*
v01 rtc
v02 rtc+aht
v03 rtc+aht+seesaw
v04 rtc+aht+seesaw+sd card
*/
#include "RTClib.h"
#include <Adafruit_AHTX0.h>
#include "Adafruit_seesaw.h"
#include <SPI.h> //sd
#include <SD.h> //sd


RTC_PCF8523 rtc;
Adafruit_AHTX0 aht;
Adafruit_seesaw ss;

const int chipSelect = 15;
const int AO_Pin=17;
int i = 0;


void setup () {
  Serial.begin(57600);

  #ifndef ESP8266
    while (!Serial); // wait for serial port to connect. Needed for native USB
  #endif
  delay(3000);
  Serial.println("SETUP...");


  //INITIALIZE REAL TIME CLOCK
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  } else {
    Serial.println("Found RTC");
  }

  if (! rtc.initialized() || rtc.lostPower()) {
    //Serial.println("RTC is NOT initialized, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  rtc.start();
  //


  //INITIALIZE AHT20
  //check air temp humid sensor
  if (! aht.begin()) {
    Serial.println("Could not find AHT");
    while (1) delay(10);
  } else {
    Serial.println("Found AHT");
  }
  //
  
  
  //INITIALIZE seesaw Soil Sensor");
  if (!ss.begin(0x36)) {
    Serial.println("ERROR! seesaw not found");
    while(1) delay(10);
  } else {
    Serial.print("seesaw Soil Sensor started, version: ");
    Serial.println(ss.getVersion(), HEX);
  }
  //
  

  //INITIALIZE SD card setup");
  if (!SD.begin(chipSelect)) {
    Serial.println("SD Card failed, or not present");
    while(1) delay(10);
  } else {
    Serial.println("SD card initialized");
  }
  //


  pinMode(0, OUTPUT); //led
  
  Serial.println("...COMPLETE SETUP");
  Serial.println();
  delay(3000);
}

void loop () {
  digitalWrite(0, HIGH);

  getTime();

  getAir();
  getTime();

  getSoil();
  getTime();

  getMethane();
  getTime();


  Serial.println();
  delay(1000);
  digitalWrite(0, LOW);
  delay(1000);
}

void getTime () {
  //Serial.println("----time");
  DateTime now = rtc.now();

  char buf1[] = "YYYY-MM-DD hh:mm:ss";
  String t = now.toString(buf1);
  Serial.print("time: ");
  Serial.println(t);
}

void getAir () {
  Serial.println("----air");
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  Serial.print("Air Temp: "); Serial.print(temp.temperature); Serial.println(" degrees C");
  Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");
}

void getSoil () {
  Serial.println("----soil");
  float tempC = ss.getTemp();
  uint16_t capread = ss.touchRead(0);
  Serial.print("Soil Temp: "); Serial.print(tempC); Serial.println("*C");
  Serial.print("Capacitive: "); Serial.println(capread);
}

void getMethane () {
  int AO_Out;
  while(i<10) {
    AO_Out= analogRead(AO_Pin);
    Serial.print("Methane Conentration: ");
    Serial.println(AO_Out);//prints the methane value
    i++;
  }  
}