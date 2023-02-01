/*test gpio voltages
*/
const int warmUpTime = 5000;

void setup () {

    delay(warmUpTime);
    //measure here
    //----

    //----
    //set output pins
    pinMode(2, OUTPUT);
    pinMode(0, OUTPUT);
    pinMode(13, OUTPUT);
    pinMode(12, OUTPUT);
    pinMode(14, OUTPUT);
    pinMode(16, OUTPUT);
    //----


    delay(warmUpTime);
    //measure here

}


void loop () {
  
    digitalWrite(2, LOW);
    digitalWrite(0, LOW);
    digitalWrite(13, LOW);
    digitalWrite(12, LOW);
    digitalWrite(14, LOW);
    digitalWrite(16, LOW);
    delay(warmUpTime);
    //measure here

    digitalWrite(2, HIGH);
    digitalWrite(0, HIGH);
    digitalWrite(13, HIGH);
    digitalWrite(12, HIGH);
    digitalWrite(14, HIGH);
    digitalWrite(16, HIGH);
    delay(warmUpTime);
    //measure here
}
