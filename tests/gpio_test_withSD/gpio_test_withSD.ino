/*test gpio voltages
#include "RTClib.h"
#include <SPI.h> //sd
#include <SD.h> //sd

RTC_PCF8523 rtc;
*/

const int warmUpTime = 5000;
const int chipSelect = 15;
const int blueLED = 2;
const int redLED = 2;
const int DONE_Pin = 16;
      //12, 13, 14 do not work - needed for SD....
      //16 won't boot right
      //MAYBE 15 works???? no can't open file - also goes high??
      //0, 2 has no conflict
      //2 won't let the board turn on - goes high before code goes low
      //0 is same as 2
      //??? 15 ???, would need to solder and rewire the datalogger
      //try 15 with SD commented out
      //trying to change SDA and SCL pins to 5,2 instead of 4, 5 using 4 as DONE pin
      //  ^^ above works
const char* fileName = "testgpioandSD.txt";

const bool lWriteToSD = false;


void setup () {

    //----
    //set output pins
    pinMode(DONE_Pin, OUTPUT);
    digitalWrite(DONE_Pin, LOW);
    pinMode(redLED, OUTPUT);
    digitalWrite(redLED, LOW);
    //pinMode(blueLED, OUTPUT);
    //digitalWrite(blueLED, LOW);
    //----


  Serial.begin(115200);
  #ifndef ESP8266
    while (!Serial); // wait for serial port to connect. Needed for native USB
  #endif
    Serial.println("setup start");



  //----
  //INITIALIZE REAL TIME CLOCK
  /*
  if (! rtc.begin()) {
    Serial.println("No RTC;"); //debugPrintLn("No RTC");
    Serial.flush();
    //while (1) delay(10);
  } else {
    Serial.println("Found RTC");
  }

  if (! rtc.initialized() || rtc.lostPower()) {
    //debugPrintLn("RTC is NOT initialized, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  rtc.start();
  */
  //----
  /*
  if (lWriteToSD){
    //----
    //INITIALIZE SD card setup");
    if (!SD.begin(chipSelect)) {
      Serial.println("SD Card failed, or not present");
      blinkBlue(200);
      while(1) delay(10);
    } else {
      Serial.println("SD card initialized");
    }
    //----
  }
    */

    Serial.print("warming up ...");
    //delay(warmUpTime);
    //measure here
    Serial.println("setup end");

}


void loop () {
  
  //----
  //Sensor warm up time?
  delay(warmUpTime);
  //----
  digitalWrite(redLED, LOW); //red led on

    writeData("----");
    writeData("starting loop - low ");
    String t = getTime();
    writeData(t);
    writeData("DONE_Pin: ");
    String d = String(DONE_Pin);
    writeData(d);

    Serial.print("____low____ ");
    Serial.println(DONE_Pin);
    digitalWrite(DONE_Pin, LOW);
    delay(warmUpTime);
    Serial.println("warmup ");
    delay(500);
    digitalWrite(redLED, HIGH); //red led off
    delay(500);
    digitalWrite(redLED, LOW); //red led on
    delay(500);

    Serial.println("----HIGH----");

    writeData("millis: ");
    String m = String(millis());
    writeData(m);
    writeData("toggling off...");
    blinkLED(redLED, 100, 10);

    toggleOff();
    delay(warmUpTime);
    //measure here
}

String getTime () {
  //get the date and time from the Real Time Clock (RTC)
  //https://learn.adafruit.com/adafruit-adalogger-featherwing/using-the-real-time-clock

/*
  String strTime;
  DateTime now = rtc.now();
  char buf1[] = "YYYY-MM-DD hh:mm:ss";
  strTime = now.toString(buf1);

  //debugPrintLn("time,");
  //debugPrintLn(strTime);
  
  return strTime;
  */
  return "--";
}

void writeData (String dataString){
  //write the data string to the SD card: open file, write to file, close the file
  //https://learn.adafruit.com/adafruit-adalogger-featherwing/using-the-sd-card
  
  if (lWriteToSD){ 
    //File dataFile = SD.open(fileName, FILE_WRITE);

    // if the file is available, write to it:
    //if (dataFile) {
    //  dataFile.println(dataString);
    //  dataFile.close();

      // print to the serial port too:
      Serial.println(dataString);
    //}
    // if the file isn't open, pop up an error:
    //else {
    //  Serial.print("====> ERROR opening file: ");
    //  Serial.println(fileName);
    //}
  }
}

void toggleOff(){
  //toggle DONE so TPL knows to cut power
  //if toggle for 20 sec and it's still on, at least go to deepsleep
  //https://learn.adafruit.com/adafruit-tpl5110-power-timer-breakout/overview?view=all#usage

  for (int i=0; i<10000; i++) {
    digitalWrite(DONE_Pin, HIGH);
    delay(1);
    digitalWrite(DONE_Pin, LOW);
    delay(1);
  }
}

void blinkBlue(int pulseTime){
  blinkLED(blueLED, pulseTime, 500);
}

void blinkRed(int pulseTime){
  blinkLED(redLED, pulseTime, 500);
}

void blinkLED(int pinNum, int pulseTime, int iterations){
  for (int i =0; i < iterations; i++) {    
    digitalWrite(pinNum, HIGH);
    delay(pulseTime);
    digitalWrite(pinNum, LOW);
    delay(pulseTime);
  }
}

