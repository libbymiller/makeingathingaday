/*
 * WebSocketClientSocketIO.ino
 *
 *  Created on: 06.06.2016
 *
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <WebSocketsClient.h>

#include <Hash.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

#include <Adafruit_Sensor.h>

#include <Adafruit_LIS3DH.h>

Adafruit_LIS3DH lis = Adafruit_LIS3DH();

#define USE_SERIAL Serial

#define MESSAGE_INTERVAL 30000
#define HEARTBEAT_INTERVAL 25000

uint64_t messageTimestamp = 0;
uint64_t heartbeatTimestamp = 0;
bool isConnected = false;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {


    switch(type) {
        case WStype_DISCONNECTED:
            USE_SERIAL.printf("[WSc] Disconnected!\n");
            isConnected = false;
            break;
        case WStype_CONNECTED:
            {
                USE_SERIAL.printf("[WSc] Connected to url: %s\n",  payload);
                isConnected = true;

          // send message to server when Connected
                // socket.io upgrade confirmation message (required)
        webSocket.sendTXT("5");
            }
            break;
        case WStype_TEXT:
            USE_SERIAL.printf("[WSc] get text: %s\n", payload);

      // send message to server
      // webSocket.sendTXT("message here");
            break;
        case WStype_BIN:
            USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
            hexdump(payload, length);

            // send data to server
            // webSocket.sendBIN(payload, length);
            break;
    }

}

void setup() {
    // USE_SERIAL.begin(921600);
    USE_SERIAL.begin(115200);

    //Serial.setDebugOutput(true);
    USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

      for(uint8_t t = 4; t > 0; t--) {
          USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
          USE_SERIAL.flush();
          delay(1000);
      }

    WiFiMulti.addAP("", "");

    //WiFi.disconnect();
    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
    }

    webSocket.beginSocketIO("192.168.1.8", 3000);
    webSocket.onEvent(webSocketEvent);

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
    webSocket.loop();

    if(isConnected) {

        sensors_event_t event; 
        lis.getEvent(&event);
        Serial.print("\t\tX: "); Serial.print(event.acceleration.x);
        Serial.print(" \tY: "); Serial.print(event.acceleration.y); 
        Serial.print(" \tZ: "); Serial.print(event.acceleration.z); 
        Serial.println(" m/s^2 ");

        Serial.println();
        webSocket.sendTXT("42[\"news\",{\"X\":"+String(event.acceleration.x)+",\"Y\":"+String(event.acceleration.y)+",\"Z\":"+String(event.acceleration.z)+" }]");
        delay(200);
    }
}
