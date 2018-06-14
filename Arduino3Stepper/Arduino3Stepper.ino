// Include the Servo library 
#include <Servo.h> 
// Declare the Servo pin 
int servoPin = 9; 
int servoPin2 = 10;
int servoPin3 = 11;
//Lowest position = 135 degrees;
//Highest position = 120 degrees;
Servo Servo1; 
Servo Servo2;
Servo Servo3;
String servo1, servo2, servo3, readString;
void setup() { 
   Serial.begin(115200);
   Servo1.attach(servoPin); 
   Servo2.attach(servoPin2);
   Servo3.attach(servoPin3);
}
void loop(){ 
  if (Serial.available() >0) {
    char c = Serial.read();
    if( c == 'B'){
        delay(2);
        while(Serial.available()){
        c = Serial.read();  //gets one byte from serial buffer
        if( c == 'E')
        {
          Serial.println(readString); //see what was received 
          if(readString.length() == 6)
          {
            servo1 = readString.substring(0,2);
            servo2 = readString.substring(2,4);
            servo3 = readString.substring(4,6);
            Servo1.write(servo1.toInt()+100); 
            Servo2.write(servo2.toInt()+100);
            Servo3.write(servo3.toInt()+100);
          }
          readString = "";
          Serial.begin(115200);
          break;
        }
        readString += c; //makes the string readString
        }
    }
  } 
}
