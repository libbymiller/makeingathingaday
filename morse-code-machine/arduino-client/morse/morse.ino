/*
 * based on WebSocketClientSocketIO.ino
 *
 *  Created on: 06.06.2016
 *
 */


#include <Arduino.h>

#include <ArduinoJson.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <WebSocketsClient.h>

#include <Hash.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;


#include <Servo.h>  // Include the Servo library

int servoPin1 = 5;  // Declare the Servo pin

int servoPin2 = 3;  // Declare the Servo pin
int pos1 = 0;

Servo Servo1;  // Create a servo object
Servo Servo2;  // Create a servo object


#define USE_SERIAL Serial

#define MESSAGE_INTERVAL 30000
#define HEARTBEAT_INTERVAL 25000

uint64_t messageTimestamp = 0;
uint64_t heartbeatTimestamp = 0;
bool isConnected = false;

void dit(){

  Servo1.write(60);
  for (pos1 = 60; pos1 <= 90; pos1 += 1) { 
    Servo1.write(pos1);
    delay(10);           
  }
  Servo1.write(90);
  delay(100);
  Servo1.write(60);
  delay(100);

}

void dah(){

  Servo1.write(60);
  for (pos1 = 60; pos1 <= 90; pos1 += 1) { 
    Servo1.write(pos1);
    delay(10);           
  }
  Servo1.write(90);
  delay(400);
  Servo1.write(60);
  delay(100);

}

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
            {
              USE_SERIAL.printf("[WSc] get text: %s\n", payload);

              // payload won't load into json as is
              // create character buffer with ending null terminator (string)
              char message_buff[100];
              int i = 0;
              for(i=0; i<length; i++) {
                 message_buff[i] = payload[i];
              }

              message_buff[i] = '\0'; 
              String msgString = String(message_buff);
              int bracketIndex = msgString.indexOf('{'); // get the part we want
              int dataIndex = msgString.indexOf("data"); //check it's a data message
              String thing = msgString.substring(bracketIndex);

              if(bracketIndex!=-1 && dataIndex!=-1){ // without this it will crash on the wrong kind of message

                DynamicJsonBuffer jsonBuffer;
                //Serial.println(msgString);
                JsonObject& json = jsonBuffer.parseObject(thing);
                //json.printTo(Serial);
                if (json.success()) {
                  USE_SERIAL.printf("ok");
                  //Serial.print(json["data"]);
                  const char* dd = json["data"]; 

                  Servo2.write(80); // second servo starts moving

                  //loop through each character

                  for(char j = 0; j<strlen(dd); j++){
                     Serial.print(dd[j]);

                     if(dd[j] == '_'){
                        Serial.println("dah");
                        dah();
                     }
                     if(dd[j] == '.'){
                        Serial.println("dit");
                        dit();
                     }
                     if(dd[j] == ' '){
                        Serial.println("space");
                        delay(400);
                     }
   
                  }
                  Servo2.write(90); // stop servo
                }else{
                  USE_SERIAL.printf("nope");
                }
              }
      

            }
            break;
        case WStype_BIN:
            USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
            hexdump(payload, length);

            // send data to server
            // webSocket.sendBIN(payload, length);
            break;
        default:
            ///hexdump(type, length);
            hexdump(payload, length);
            break;

    }

}

void setup() {
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

    WiFiMulti.addAP("YourAP", "passwd");

    //WiFi.disconnect();
    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
    }

    // hardcoded ip address running on my server

    webSocket.beginSocketIO("192.168.1.8", 3000);
    webSocket.onEvent(webSocketEvent);

    //servo stuff

    delay(200);
    
    Servo1.attach(servoPin1); // We need to attach the servo to the pin number
    Servo2.attach(servoPin2);  

    delay(200);
    Servo1.write(60);
    Servo2.write(90);

}

void loop() {
    webSocket.loop();

    if(isConnected) {

        delay(200);
    }
}
