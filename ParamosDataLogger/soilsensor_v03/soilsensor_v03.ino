
#include "RTClib.h"
#include <Adafruit_AHTX0.h>
#include "Adafruit_seesaw.h"


RTC_PCF8523 rtc;
Adafruit_AHTX0 aht;
Adafruit_seesaw ss;


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


  //INITIALIZE AHT20
  //check air temp humid sensor
  if (! aht.begin()) {
    Serial.println("Could not find AHT");
    while (1) delay(10);
  }
  //
  
  
  //INITIALIZE seesaw Soil Sensor");
  if (!ss.begin(0x36)) {
    Serial.println("ERROR! seesaw not found");
    while(1) delay(1);
  } else {
    Serial.print("seesaw started! version: ");
    Serial.println(ss.getVersion(), HEX);
  }
  //

  
  pinMode(0, OUTPUT);
}

void loop () {
  digitalWrite(0, HIGH);

  getTime();

  getAir();
  getTime();
  getSoil();

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
