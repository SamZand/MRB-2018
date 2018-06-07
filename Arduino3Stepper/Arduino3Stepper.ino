// Include the Servo library 
#include <Servo.h> 
// Declare the Servo pin 
int servoPin = 3; 
int servoPin2 = 2;
int servoPin3 = 4;
//Lowest position = 135 degrees;
//Highest position = 120 degrees;
Servo Servo1; 
Servo Servo2;
Servo Servo3;
String servo1, servo2, servo3, readString;
void setup() { 
   Serial.begin(9600);
   Servo1.attach(servoPin); 
   Servo2.attach(servoPin2);
   Servo3.attach(servoPin3);
}
void loop(){ 
 while (Serial.available()) {
    delay(3);  //delay to allow buffer to fill 
    if (Serial.available() >0) {
      char c = Serial.read();  //gets one byte from serial buffer
      readString += c; //makes the string readString
    } 
  }

  if (readString.length() >5) {
      Serial.println(readString); //see what was received
          
      servo1 = readString.substring(0,2);
      servo2 = readString.substring(2,4);
      servo3 = readString.substring(4,6);
      readString = "";
      Serial.println(servo1);  //print to serial monitor to see parsed results
      Serial.println(servo2);
      Serial.println(servo3);
      Servo1.write(servo1.toInt()+120); 
      Servo2.write(servo2.toInt()+120);
      Servo3.write(servo3.toInt()+120);
  }
}
