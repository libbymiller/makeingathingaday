#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <Adafruit_NeoPixel.h>

//based on https://learn.adafruit.com/chameleon-scarf/overview?view=all#code
// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)

const int numPixels = 50;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(numPixels, 2, NEO_RGB + NEO_KHZ800);


// our RGB -> eye-recognized gamma color
byte gammatable[256];

const int buttonPin = 5;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin

int buttonState = 0;         // variable for reading the pushbutton status

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

void setup() {
  Serial.begin(115200);
  Serial.println("Color View Test!");
  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }
  
  // thanks PhilB for this gamma table!
  // it helps convert RGB colors to what humans see
  for (int i=0; i<256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;
      
    gammatable[i] = x;      
    //Serial.println(gammatable[i]);
  }

  Serial.println();
  Serial.println("Running...");

}
   
uint16_t clear, red, green, blue;

const int max_length = numPixels;
int recorded[max_length][3];
int recordedLength = 0;
int lastButton = HIGH;

void loop()
{

  buttonState = digitalRead(buttonPin);
  Serial.println("buttonstate "+buttonState);
  if (buttonState == HIGH) {
      if(lastButton != HIGH){
        //flash to show we're recording
        colorWipe(strip.Color(0,0,0), 0);
        strip.show(); // Initialize all pixels to 'off'
        for (int i=0; i<3; i++){ //this sequence flashes the first pixel three times as a countdown to the color reading.
          strip.setPixelColor (0, strip.Color(188, 188, 188)); //white, but dimmer-- 255 for all three values makes it blinding!
          strip.show();
          delay(1000);
          strip.setPixelColor (0, strip.Color(0, 0, 0));
          strip.show();
          delay(500);
        }
        delay(2000);
        recordedLength = 0;
      }
      if(recordedLength < 50){ //don't continue recording if we've run out pixels
        Serial.println("RECORDING "+recordedLength);
        digitalWrite(ledPin, HIGH);
      
        tcs.setInterrupt(false);      // turn on LED
        delay(60);  // takes 50ms to read 
  
        tcs.getRawData(&red, &green, &blue, &clear);
        tcs.setInterrupt(true);  // turn off LED
    
        Serial.print("C:\t"); Serial.print(clear);
        Serial.print("\tR:\t"); Serial.print(red);
        Serial.print("\tG:\t"); Serial.print(green);
        Serial.print("\tB:\t"); Serial.print(blue);

        // Figure out some basic hex code for visualization
        uint32_t sum = red;
        sum += green;
        sum += blue;
        //sum += clear; // clear contains RGB already so no need to re-add it
  
        float r, g, b;
        r = red; r /= sum;
        g = green; g /= sum;
        b = blue; b /= sum;
        r *= 256; g *= 256; b *= 256;
        Serial.print("\t");
        Serial.print((int)r, HEX); Serial.print((int)g, HEX); Serial.print((int)b, HEX);
        Serial.println();
        recorded[recordedLength][0] = (int)r;
        recorded[recordedLength][1] = (int)g;
        recorded[recordedLength][2] = (int)b;
        //Serial.print((int)r ); Serial.print(" "); Serial.print((int)g);Serial.print(" ");  Serial.println((int)b );
        strip.setPixelColor(recordedLength, strip.Color(gammatable[(int)r], gammatable[(int)g], gammatable[(int)b]));
        strip.show();
        if(recordedLength < 50){
           recordedLength++;
        }else{ // do nothing
          Serial.println("Pixel memory is full");
        }
      }
      delay(2000);
  }else{
      Serial.println("PLAYBACK");
      Serial.print("recorded length");
      Serial.println(recordedLength);
      digitalWrite(ledPin, LOW);
      for(int i = 0; i < recordedLength; i++){
        if(recorded[i]!=NULL && (recorded[i][0]!=0 && recorded[i][1]!=0 && recorded[i][2]!=0)){
          int rr = recorded[i][0];
          int gg = recorded[i][1];
          int bb = recorded[i][2];
          Serial.print("\t");
          Serial.print((int)rr, HEX); Serial.print((int)gg, HEX); Serial.print((int)bb, HEX);
          Serial.println();
          colorWipe(strip.Color(gammatable[(int)rr], gammatable[(int)gg], gammatable[(int)bb]), 0);
          delay(2000);
        }
      }
      
  }
  lastButton = buttonState;
  

}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

