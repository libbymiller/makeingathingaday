#include <Adafruit_Sensor.h>
#include <Adafruit_LIS3DH.h>

#include <Wire.h>
#include <SPI.h>
#define THRESHOLD 10


// I2C
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
   #define Serial SerialUSB
#endif

int ledPin = 9; 

void setup(void) {
#ifndef ESP8266
  while (!Serial);     // will pause Zero, Leonardo, etc until serial console opens
#endif

  Serial.begin(9600);
  Serial.println("LIS3DH test!");
  
  if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
    while (1);
  }
  Serial.println("LIS3DH found!");
  
  lis.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!
  
  Serial.print("Range = "); Serial.print(2 << lis.getRange());  
  Serial.println("G");
}

void loop() {
  lis.read();      // get X Y and Z data at once

  sensors_event_t event; 
  lis.getEvent(&event);
  
  float vibe = ((abs(event.acceleration.x) + abs(event.acceleration.y) + abs(event.acceleration.y))/2)*8.5;
  Serial.println(vibe);
  if(vibe>255.0){
     vibe = 255.0;
  }
  analogWrite(ledPin, vibe);

  delay(200); 
}
