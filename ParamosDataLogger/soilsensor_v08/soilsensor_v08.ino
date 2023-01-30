/*
v01 rtc
v02 +aht
v03 +seesaw
v04 +sd card detect
v05 +mq-4
v06 return string to write
v07 +milliseconds from power on to output (millis())
    if power management succeeds this will reset every time readings are taken
v08 +TPL5110 - call DONE_Pin for DONE to turn off power
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
const int AO_Pin = 17;
const int DONE_Pin = 16; //was 15 but used for SD?
const int warmUpTime = 3000;
const char* fileName = "datalog.csv";
const bool isDebug = true;

void setup () {

  //----
  //Sensor warm up time?
  delay(warmUpTime);
  //----


  //----
  //INITIALIZE serial port
  Serial.begin(57600);
  #ifndef ESP8266
    while (!Serial); // wait for serial port to connect. Needed for native USB
  #endif
  debugPrintLn("SETUP...");
  //----


  //----
  //INITIALIZE REAL TIME CLOCK
  if (! rtc.begin()) {
    debugPrintLn("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  } else {
    debugPrintLn("Found RTC");
  }

  if (! rtc.initialized() || rtc.lostPower()) {
    //debugPrintLn("RTC is NOT initialized, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  rtc.start();
  //----


  //----
  //INITIALIZE AHT20
  //check air temp humid sensor
  if (! aht.begin()) {
    debugPrintLn("Could not find AHT");
    while (1) delay(10);
  } else {
    debugPrintLn("Found AHT");
  }
  //----
  
  

  //----
  //INITIALIZE seesaw Soil Sensor");
  if (!ss.begin(0x36)) {
    debugPrintLn("seesaw not found");
    while(1) delay(10);
  } else {
    debugPrintLn("seesaw Soil Sensor started, version: ");
    debugPrintLn(ss.getVersion(), HEX);
  }
  //----
  

  //----
  //INITIALIZE SD card setup");
  if (!SD.begin(chipSelect)) {
    debugPrintLn("SD Card failed, or not present");
    while(1) delay(10);
  } else {
    debugPrintLn("SD card initialized");
  }
  //----


  //----
  //set output pins
  pinMode(0, OUTPUT); //led
  pinMode(DONE_Pin, OUTPUT);
  //----


  debugPrintLn("...COMPLETE SETUP");
  debugPrintLn();
  if (isDebug) {delay(3000)};
}


void loop () {
  digitalWrite(0, LOW); //red led on

  //----
  //build output string from sensors and write to SD
  String strData = "";
  strData += getTime();
  strData += "," + getAir();
  strData += "," + getSoil();
  strData +=  "," + getMethane();
  strData +=  "," + String(millis());
  writeData(strData);
  //----

  debugPrintLn(strData);
  debugPrintLn();
  if (isDebug) {delay(3000)};

  digitalWrite(0, HIGH);    //red led off

  //----
  //tell TPL to turn power off
  toggleOff();
  //----
}


String getTime () {
  //get the date and time from the Real Time Clock (RTC)
  //https://learn.adafruit.com/adafruit-adalogger-featherwing/using-the-real-time-clock

  String strTime;
  DateTime now = rtc.now();
  char buf1[] = "YYYY-MM-DD hh:mm:ss";
  strTime = now.toString(buf1);

  debugPrintLn("time: ");
  debugPrintLn(strTime);
  
  return strTime;
}


String getAir () {
  //read the air temperature and humidity from the AHT20 sensor
  //https://learn.adafruit.com/adafruit-aht20/arduino

  debugPrintLn("----air");

  String strAir;
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  strAir = String(temp.temperature) + "," + String(humidity.relative_humidity);

  debugPrintLn("Air Temp: "); 
  debugPrintLn(temp.temperature); 
  debugPrintLn(" degrees C");
  debugPrintLn("Humidity: "); 
  debugPrintLn(humidity.relative_humidity); 
  debugPrintLn("% rH");
  
  return strAir;
}


String getSoil () {
  //get the soil temp and capacitance from the soil moisture sensor (seesaw)
  //https://learn.adafruit.com/adafruit-stemma-soil-sensor-i2c-capacitive-moisture-sensor/arduino-test
  
  debugPrintLn("----soil");

  String strSoil;
  float tempC = ss.getTemp();
  uint16_t capread = ss.touchRead(0);
  strSoil = String(tempC) + "," + String(capread);

  debugPrintLn("Soil Temp: "); 
  debugPrintLn(tempC); 
  debugPrintLn("*C");
  debugPrintLn("Capacitive: "); 
  debugPrintLn(capread);

  return strSoil;
}

String getMethane () {
  //get the methane reading from the MQ-4 sensor
  //https://microcontrollerslab.com/mq4-methane-gas-sensor-pinout-interfacing-with-arduino-features/
  //maybe?? https://webcache.googleusercontent.com/search?q=cache:2XnjRsSkf0MJ:https://www.teachmemicro.com/mq-4-methane-gas-sensor-arduino/&cd=1&hl=en&ct=clnk&gl=us
  
  debugPrintLn("----methane");

  String strMethane;
  int AO_Out;
  AO_Out = analogRead(AO_Pin);
  strMethane = String(AO_Out);

  debugPrintLn("Methane Concentration: ");
  debugPrintLn(AO_Out);//prints the methane value
  
  return strMethane;
}

void writeData (String dataString){
  //write the data string to the SD card
  //open file, write to file, close the file
  //https://learn.adafruit.com/adafruit-adalogger-featherwing/using-the-sd-card
  
  File dataFile = SD.open(fileName, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();

    // print to the serial port too:
    debugPrintLn(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    debugPrintLn("error opening datalog.txt");
  }
}

void debugPrintLn(String s){
  //if the debug constant is true then print to the serial port
  if(isDebug){
    Serial.println(s);
  }
}

void toggleOff(){
  //toggle DONE so TPL knows to cut power!
  //https://learn.adafruit.com/adafruit-tpl5110-power-timer-breakout/overview?view=all#usage

  while (1) {
    digitalWrite(DONE_Pin, HIGH);
    delay(1);
    digitalWrite(DONE_Pin, LOW);
    delay(1);
  }
}