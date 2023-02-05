/*test gpio voltages
*/
const int warmUpTime = 5000;

void setup () {

    //----
    //set output pins
    pinMode(5, OUTPUT);
    digitalWrite(5, LOW);
    pinMode(0, OUTPUT);
    digitalWrite(0, LOW);
    pinMode(4, OUTPUT);
    digitalWrite(4, LOW);
    pinMode(2, OUTPUT);
    pinMode(13, OUTPUT);
    pinMode(12, OUTPUT);
    pinMode(14, OUTPUT);
    //pinMode(16, OUTPUT);  //resets?
    //----


    //delay(warmUpTime);
    //measure here
    //----

  Serial.begin(57600);
  #ifndef ESP8266
    while (!Serial); // wait for serial port to connect. Needed for native USB
  #endif
    Serial.println("setup start");

    delay(warmUpTime);
    delay(warmUpTime);
    //measure here
    Serial.println("setup end");

}


void loop () {
  
    Serial.println("____low____");
    digitalWrite(5, LOW);
    digitalWrite(4, LOW);
    digitalWrite(2, LOW);
    digitalWrite(0, LOW);
    digitalWrite(13, LOW);
    digitalWrite(12, LOW);
    digitalWrite(14, LOW);
    //digitalWrite(16, LOW);
    delay(warmUpTime);
    //measure here

    Serial.println("----HIGH----");
    digitalWrite(5, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(2, HIGH);
    digitalWrite(0, HIGH);
    digitalWrite(13, HIGH);
    digitalWrite(12, HIGH);
    digitalWrite(14, HIGH);
    //digitalWrite(16, HIGH);
    delay(warmUpTime);
    //measure here
}
