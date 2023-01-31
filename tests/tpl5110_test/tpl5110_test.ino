/*test toggle
*/
#include "RTClib.h"
#include <SPI.h> //sd
#include <SD.h> //sd


RTC_PCF8523 rtc;

const int chipSelect = 15;
const int AO_Pin = 17;
const int DONE_Pin = 0; //16; //was 15 but used for SD?
const int warmUpTime = 2500;
const char* fileName = "datalog.csv";
const bool isDebug = true;
const char* cVer = "v09";

void setup () {

  //----
  //Sensor warm up time?
  delay(warmUpTime);
  //----


  String errMsg = "";
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
    errMsg += "No RTC;"; //debugPrintLn("No RTC");
    Serial.flush();
    //while (1) delay(10);
  } else {
    debugPrintLn("Found RTC");
  }

  if (! rtc.initialized() || rtc.lostPower()) {
    //debugPrintLn("RTC is NOT initialized, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  rtc.start();
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
  //check for errors, log to card if available, do nothing
  if (errMsg != ""){
    errMsg += cVer;
    writeData(errMsg);
    while(1) delay(10);
  }
  //----

  //----
  //set output pins
  pinMode(2, OUTPUT); //led
  pinMode(DONE_Pin, OUTPUT);
  //digitalWrite(DONE_Pin, LOW);
  //----


  debugPrintLn("...COMPLETE SETUP");
  debugPrintLn("");
}


void loop () {
  digitalWrite(2, LOW); //red led on

  for (int i=0; i<20; i++) {
    delay(500);
    digitalWrite(2, HIGH);    //red led off
    delay(500);
    digitalWrite(2, LOW);
    //----
    //build output string from sensors and write to SD
    String strData = "pin0,";
    strData += getTime();
    strData +=  "," + String(millis());
    writeData(strData);
    //----
  }

  debugPrintLn("");
  if (isDebug) {delay(3000);};

  digitalWrite(2, HIGH);    //red led off

  //----
  //tell TPL to turn power off
  debugPrintLn("toggling...");
  toggleOff();
  //----
}


String getTime () {
  //get the date and time from the Real Time Clock (RTC)
  //https://learn.adafruit.com/adafruit-adalogger-featherwing/using-the-real-time-clock

  debugPrintLn("----time");

  String strTime;
  DateTime now = rtc.now();
  char buf1[] = "YYYY-MM-DD hh:mm:ss";
  strTime = now.toString(buf1);

  //debugPrintLn("time,");
  //debugPrintLn(strTime);
  
  return strTime;
}


void writeData (String dataString){
  //write the data string to the SD card: open file, write to file, close the file
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
    debugPrintLn("error opening datalog.csv");
  }
}

void debugPrintLn(String s){
  //if the debug constant is true then print to the serial port
  if(isDebug){
    Serial.println(s);
  }
}

void toggleOff(){
  //toggle DONE so TPL knows to cut power
  //if toggle for 20 sec and it's still on, at least go to deepsleep
  //https://learn.adafruit.com/adafruit-tpl5110-power-timer-breakout/overview?view=all#usage

  for (int i=0; i<1000; i++) {
    debugPrintLn("high...");
    digitalWrite(DONE_Pin, HIGH);
    delay(10);
    debugPrintLn("low...");
    digitalWrite(DONE_Pin, LOW);
    delay(10);
  }
  writeData("toggled, but still on - deepsleep 50min");
  ESP.deepSleep(30000e6); // 20e6 is 20 seconds //eventually go to sleep
}