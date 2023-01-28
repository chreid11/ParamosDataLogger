/*
v01 rtc
v02 +aht
v03 +seesaw
v04 +sd card detect
v05 +mq-4
v06 return string to write
v07 +milliseconds from power on to output (millis())
    if power management succeeds this will reset every time readings are taken
v08 +TPL5110 - call pin 15 for DONE to turn off power
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
const int DONE_Pin = 15;
const int warmUpTime = 3000;
const char* fileName = "datalog.csv";
const bool isDebug = true;

void setup () {

  delay(warmUpTime);

  Serial.begin(57600);

  #ifndef ESP8266
    while (!Serial); // wait for serial port to connect. Needed for native USB
  #endif
  myPrintLine("SETUP...");


  //INITIALIZE REAL TIME CLOCK
  if (! rtc.begin()) {
    myPrintLine("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  } else {
    myPrintLine("Found RTC");
  }

  if (! rtc.initialized() || rtc.lostPower()) {
    //myPrintLine("RTC is NOT initialized, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  rtc.start();
  //


  //INITIALIZE AHT20
  //check air temp humid sensor
  if (! aht.begin()) {
    myPrintLine("Could not find AHT");
    while (1) delay(10);
  } else {
    myPrintLine("Found AHT");
  }
  //
  
  
  //INITIALIZE seesaw Soil Sensor");
  if (!ss.begin(0x36)) {
    myPrintLine("seesaw not found");
    while(1) delay(10);
  } else {
    myPrintLine("seesaw Soil Sensor started, version: ");
    myPrintLine(ss.getVersion(), HEX);
  }
  //
  

  //INITIALIZE SD card setup");
  if (!SD.begin(chipSelect)) {
    myPrintLine("SD Card failed, or not present");
    while(1) delay(10);
  } else {
    myPrintLine("SD card initialized");
  }
  //


  pinMode(0, OUTPUT); //led
  pinMode(DONE_Pin, OUTPUT);


  myPrintLine("...COMPLETE SETUP");
  myPrintLine();
  if (isDebug) {delay(3000)};
}


void loop () {
  digitalWrite(0, LOW);

  String strData = "";
  strData += getTime();
  strData += "," + getAir();
  strData += "," + getSoil();
  strData +=  "," + getMethane();
  //myMillis = millis();
  strData +=  "," + String(millis());
  writeData(strData);

  myPrintLine(strData);
  myPrintLine();
  if (isDebug) {delay(3000)};

  digitalWrite(0, HIGH);
  toggleOff();
}

String getTime () {
  //get the date and time from teh Real Time Clock (RTC)
  String strTime;
  DateTime now = rtc.now();

  char buf1[] = "YYYY-MM-DD hh:mm:ss";
  strTime = now.toString(buf1);
  myPrintLine("time: ");
  myPrintLine(strTime);
  return strTime;
}

String getAir () {
  //read the air temperature and humidity from the AHT20 sensor
  String strAir;
  myPrintLine("----air");
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data

  strAir = String(temp.temperature) + "," + String(humidity.relative_humidity);
  myPrintLine("Air Temp: "); 
  myPrintLine(temp.temperature); 
  myPrintLine(" degrees C");
  myPrintLine("Humidity: "); 
  myPrintLine(humidity.relative_humidity); 
  myPrintLine("% rH");
  return strAir;
}

String getSoil () {
  //get the soil temp and capacitance from teh soil moisture sensor (seesaw)
  String strSoil;
  myPrintLine("----soil");
  float tempC = ss.getTemp();
  uint16_t capread = ss.touchRead(0);
  strSoil = String(tempC) + "," + String(capread);

  myPrintLine("Soil Temp: "); 
  myPrintLine(tempC); 
  myPrintLine("*C");
  myPrintLine("Capacitive: "); 
  myPrintLine(capread);

  return strSoil;
}

String getMethane () {
  //get the methane reading from the MQ-4 sensor
  String strMethane;
  int AO_Out;
  //while(i<10) {
    AO_Out = analogRead(AO_Pin);
    strMethane = String(AO_Out);

    myPrintLine("Methane Concentration: ");
    myPrintLine(AO_Out);//prints the methane value
  //  i++;
  return strMethane;
  //}  
}

void writeData (String dataString){
  //write the data string to the SD card
  //open file, write to file, close the file
  File dataFile = SD.open(fileName, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    myPrintLine(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    myPrintLine("error opening datalog.txt");
  }
}

void myPrintLine(String s){
  //if the debug constant is true then print to the serial port
  if(isDebug){
    Serial.println(s);
  }
}

void toggleOff(){
  //https://learn.adafruit.com/adafruit-tpl5110-power-timer-breakout/overview?view=all#usage
  // toggle DONE so TPL knows to cut power!
  while (1) {
    digitalWrite(DONE_Pin, HIGH);
    delay(1);
    digitalWrite(DONE_Pin, LOW);
    delay(1);
  }
}