// Date and time functions using a PCF8523 RTC connected via I2C and Wire lib
#include "RTClib.h"

RTC_PCF8523 rtc;

void setup () {
  Serial.begin(57600);

  #ifndef ESP8266
    while (!Serial); // wait for serial port to connect. Needed for native USB
  #endif
  delay(3000);
  Serial.println("starting");


  //INITIALIZE REAL TIME CLOCK
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (! rtc.initialized() || rtc.lostPower()) {
    //Serial.println("RTC is NOT initialized, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  rtc.start();
  //


  
  pinMode(0, OUTPUT);
}

void loop () {
  Serial.println("----time");
  digitalWrite(0, HIGH);

  gettime();

  Serial.println();
  delay(1000);
  digitalWrite(0, LOW);
  delay(1000);
}

void gettime () {
  Serial.println("----time");
  DateTime now = rtc.now();

  char buf1[] = "YYYY-MM-DD hh:mm:ss";
  String t = now.toString(buf1);
  Serial.print("time: ");
  Serial.println(t);
}
