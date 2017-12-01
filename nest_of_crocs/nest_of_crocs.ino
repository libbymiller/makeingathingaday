/*
 * based on and very similar to
 http://www.arduino.cc/en/Tutorial/Sweep
 * 
 */
#include <Servo.h>

Servo myservo1;  // create servo object to control a servo
Servo myservo2;  // create servo object to control a servo
Servo myservo3;  // create servo object to control a servo


int pos1 = 0;    // variable to store the servo position
int pos2 = 0;    // variable to store the servo position
int pos3 = 0;    // variable to store the servo position



void setup() {
  myservo1.attach(7); 
  myservo2.attach(8);
  myservo3.attach(9); 
}

void loop() {
  for (pos1 = 70; pos1 <= 110; pos1 += 1) { 
    myservo1.write(pos1);
    delay(10);           
  }
  for (pos1 = 110; pos1 >= 70; pos1 -= 1) { 
      myservo1.write(pos1);              
      delay(10);  
  }                     

  for (pos2 = 70; pos2 <= 110; pos2 += 1) { 
    myservo2.write(pos2);
    delay(20);           
  }
  for (pos2 = 110; pos2 >= 70; pos2 -= 1) { 
      myservo2.write(pos2);              
      delay(20);  
  }

  for (pos3 = 70; pos3 <= 110; pos3 += 1) { 
    myservo3.write(pos3);
    delay(30);           
  }
  for (pos3 = 110; pos3 >= 70; pos3 -= 1) { 
      myservo3.write(pos3);              
      delay(30);  
  }

}


