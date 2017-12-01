/*

A crocwalker!

 */
#include <Servo.h>

Servo myservo1;  // create servo object to control a servo
Servo myservo2;  
Servo myservo3;  


int pos1 = 0;    // variable to store the servo position
int pos2 = 0;    
int pos3 = 0;    

void setup() {
  myservo1.attach(9); 
  myservo2.attach(8); 
  myservo3.attach(7); 

//close both "feet" crocs. 
//mine were reversed - might need a bit of tweaking to get yours right

  for (pos2 = 80; pos2 <= 130; pos2 += 1) { 
    myservo2.write(pos2);
    delay(20);           
  }

  for (pos3 = 80; pos3 <= 130; pos3 += 1) { 
    myservo3.write(pos3);
    delay(20);           
  }


}

void loop() {

//open the first leg croc

  for (pos2 = 130; pos2 >= 80; pos2 -= 1) { 
      myservo2.write(pos2);              
      delay(10);  
  } 
 
//open the leg servo
  
  for (pos1 = 80; pos1 <= 130; pos1 += 1) { 
    myservo1.write(pos1);
    delay(10);           
  }


// close the first croc
  for (pos2 = 80; pos2 <= 130; pos2 += 1) { 
    myservo2.write(pos2);
    delay(10);           
  }


//open the second croc

  for (pos3 = 130; pos3 >= 80; pos3 -= 1) { 
      myservo3.write(pos3);              
      delay(10);  
  } 

//close the leg  

  for (pos1 = 130; pos1 >= 80; pos1 -= 1) { 
      myservo1.write(pos1);              
      delay(10);  
  }  

//close the second croc

  for (pos3 = 80; pos3 <= 130; pos3 += 1) { 
    myservo3.write(pos3);
    delay(10);           
  }

}


