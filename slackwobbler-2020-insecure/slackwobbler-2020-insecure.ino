/**
   ESP8266/Arduino Slack DoorBot

   Based on https://github.com/urish/arduino-slack-bot by Uri Shaked.

   This project is brought to you by open software and open hardware. Without it, we wouldn't have nice things. "Something, something, shoulders of giants."

   This version based on https://github.com/jeremy21212121/slack-doorbot-esp8266

   Edited by @libbymiller to add a servo.

   Licensed under the MIT License
*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

// this is https://github.com/Links2004/arduinoWebSockets - Marcus Sadler
#include <WebSocketsClient.h>
#include <WebSockets.h>
#include <ArduinoJson.h>

#include <WiFiClientSecure.h>

#define WORD_SEPERATORS "., \"'()[]<>;:-+&?!\n\t"

#include "Secrets.h"
 /**
  
 ---------------------------------------
 *  Secrets.h contains the following:  *
 ---------------------------------------
 
#define SLACK_BOT_TOKEN "slackbottokenhere" // Get token by creating new bot integration at https://my.slack.com/services/new/bot 
#define WIFI_SSID       "wifinetworkname"
#define WIFI_PASSWORD   "wifipassword"
 
 */

// set to true for dev or debugging
#define DEBUG_SERIAL_PRINT true

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

unsigned long nextCmdId = 1UL; // signed longs overflow to a large negative value, which isn't what we want. Even though it would probably take 13 years :P
bool connected = false;

//servo stuff
#include <Servo.h>

Servo myservo;  // create servo object to control a servo

int pos = 0;    // variable to store the servo position


/**
  Sends a ping message to Slack. Call this function immediately after establishing
  the WebSocket connection, and then every 5 seconds to keep the connection alive.
*/
void sendPing() {
  char nxtCmd[10]; // unsigned long is max ten char in base 10 representation (4,294,967,295)
  ultoa(nextCmdId, nxtCmd, 10); // converts unsigned long into chars. last param is "base", as in decimal (base 10) number, binary (base 2), hex (base 16), etc
  nextCmdId++;
  String json = "{\"type\":\"ping\",\"id\":" + String(nxtCmd) + "}"; // hand writing serialized json because it was easier than creating an object, adding the properties and values, then serializing it
  webSocket.sendTXT(json);
}

/**
  Sends a text message to a particular slackChannel
 */
void respond(const char* slackChannel, String txtMsg) {
    char nxtCmd[10]; // nextCmdId is an "unsigned long", max value 4,294,967,295 which is 10 chars
    ultoa(nextCmdId, nxtCmd, 10); // convert from unsigned long to chars
    String json = "{\"type\":\"message\",\"id\":" + String(nxtCmd) + ",\"channel\":\"" + String(slackChannel) + "\",\"text\":\"" + txtMsg + "\"}"; // hand write serialized json, even tho we have a json lib
    webSocket.sendTXT(json);
    nextCmdId++;
}

/**
  Deserialize response and get the channel, type, and text
  If text starts and ends with the magic words, do stuff
*/
void processSlackMessage(char *payload) {
  
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, payload);
  const char* slackChannel = doc["channel"];
  const char* type = doc["type"];
  const char* message = doc["text"];
  String tt = String( type );
  String chanString = String(slackChannel);
  
  bool publicChan = chanString.startsWith("C");
  // https://stackoverflow.com/questions/41111227/how-can-a-slack-bot-detect-a-direct-message-vs-a-message-in-a-channel

  if(tt == "message") {
    Serial.println("ok");
    char *nextWord = NULL;
    for (nextWord = strtok((char *)  message, WORD_SEPERATORS); nextWord; nextWord = strtok(NULL, WORD_SEPERATORS)) {
        //Serial.println(nextWord);
        if (strcasecmp(nextWord, "iain") == 0 ||
            strcasecmp(nextWord, "@here") == 0) {
        Serial.println("match!!!");
        myservo.write(0);
        delay(200);
        myservo.write(180);
        delay(200);
        myservo.write(0);
        delay(200);
        myservo.write(180);
        delay(200);
        myservo.write(0);
      }
    }
  }

/*
  if ( !publicChan && String(type) == "message" ) {
    // its a direct message
    if ( txt.startsWith("open") && txt.endsWith("sesame") ) {
        respond(slackChannel, SUCCESS_RESP_MSG);
        toggleRelay();
    } else {
        respond(slackChannel, FAIL_RESP_MSG);
    }
  }
*/  
}

/**
  Called on each web socket event. Handles disconnection, and also
  incoming messages from slack.
*/
void webSocketEvent(WStype_t type, uint8_t *payload, size_t len) {
  switch (type) {
    case WStype_DISCONNECTED:
      if (DEBUG_SERIAL_PRINT) {
        Serial.printf("[WebSocket] Disconnected :-( \n");
      }
      connected = false;
      break;

    case WStype_CONNECTED:
      if (DEBUG_SERIAL_PRINT) {
        Serial.printf("[WebSocket] Connected to: %s\n", payload);
      }
      sendPing();
      break;

    case WStype_TEXT:
      if (DEBUG_SERIAL_PRINT) {
        Serial.printf("[WebSocket] Message: %s\n", payload);
      }
      processSlackMessage((char*)payload);
      break;
  }
}

/**
  Establishes a bot connection to Slack:
  1. Performs a REST call to get the WebSocket URL
  2. Conencts the WebSocket
  Returns true if the connection was established successfully.
*/
bool connectToSlack() {
  
  // Step 1: Find WebSocket address via RTM API (https://api.slack.com/methods/rtm.connect)
  //BearSSL::WiFiClientSecure httpsClient;
  WiFiClientSecure httpsClient;

  httpsClient.setTimeout(15000); // 15 Seconds
  httpsClient.setInsecure();

  HTTPClient http;
  http.begin(httpsClient,"https://slack.com/api/rtm.connect?token=" SLACK_BOT_TOKEN);
  int httpCode = http.GET();

  if (httpCode != HTTP_CODE_OK) {
    if (DEBUG_SERIAL_PRINT) {
      Serial.printf("HTTP GET failed with code %d\n", httpCode);
    }
    if (httpCode == 429) {
      /**
        the RTM API can only be called once per minute! So if it didn't work the first time, we'll just wait 60 seconds.
        It would be better to parse the "Retry-After" header, as it is probably somewhat < 60 seconds
       */
      if (DEBUG_SERIAL_PRINT) {
        Serial.printf("[Warn] Rate limited, waiting the max (60 secs) rather than parsing the header");
      }
      delay(60000);
    }
    return false;
  }
  WiFiClient *client = http.getStreamPtr();
  
  client->find("wss:\\/\\/");
  
  String host = client->readStringUntil('\\');
  String path = client->readStringUntil('"');

  path.replace("\\/", "/");

  // Step 2: Open WebSocket connection and register event handler
  if (DEBUG_SERIAL_PRINT) {
    Serial.println("WebSocket Host=" + host + " Path=" + path);
  }
  //webSocket.beginSSL(host.c_str(), 443, path.c_str());
  webSocket.beginSSL(host.c_str(), 443, path.c_str());

  webSocket.onEvent(webSocketEvent);
  return true;
  
}

unsigned long connectAndPing( unsigned long lastPing ) {
  
  /**
    Establishes a connection to slack and then pings every 5 seconds to keep the connection active
    Returns unsigned long lastPing
   */
  
  if (connected) {
    // Send ping every 5 seconds, to keep the connection alive
    if (millis() - lastPing > 5000) {
      sendPing();
      lastPing = millis();
    }
  } else {
    // Try to connect / reconnect to slack
    connected = connectToSlack();
    if (!connected) {
      delay(500);
    }
  }
  return lastPing;

}

void setup() {

  if (DEBUG_SERIAL_PRINT) {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
  }

  //wemos D1 pinout: https://escapequotes.net/esp8266-wemos-d1-mini-pins-and-diagram/
  // we are using IO5 -> D1
  myservo.attach(5);

  WiFiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
  }
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  
}

unsigned long lastPing = 0;

void loop() {
  
  webSocket.loop();
  lastPing = connectAndPing( lastPing );

}
