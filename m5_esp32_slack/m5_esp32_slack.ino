
/**
   ESP8266/Arduino Slack DoorBot

   Based on https://github.com/urish/arduino-slack-bot by Uri Shaked.

   This project is brought to you by open software and open hardware. Without it, we wouldn't have nice things. "Something, something, shoulders of giants."

   THis version based on https://github.com/jeremy21212121/slack-doorbot-esp8266

   Edited by @libbymiller for esp32 + servo.

   Licensed under the MIT License
*/

// Include the ESP32 Arduino Servo Library instead of the original Arduino Servo Library
#include <ESP32Servo.h> 

// M5stickC library
#include <M5StickC.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// this is https://github.com/Links2004/arduinoWebSockets - Marcus Sadler
#include <WebSocketsClient.h>
#include <WebSockets.h>

#include <ArduinoJson.h>

const char* rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIElDCCA3ygAwIBAgIQAf2j627KdciIQ4tyS8+8kTANBgkqhkiG9w0BAQsFADBh\n" \
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n" \
"QTAeFw0xMzAzMDgxMjAwMDBaFw0yMzAzMDgxMjAwMDBaME0xCzAJBgNVBAYTAlVT\n" \
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxJzAlBgNVBAMTHkRpZ2lDZXJ0IFNIQTIg\n" \
"U2VjdXJlIFNlcnZlciBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n" \
"ANyuWJBNwcQwFZA1W248ghX1LFy949v/cUP6ZCWA1O4Yok3wZtAKc24RmDYXZK83\n" \
"nf36QYSvx6+M/hpzTc8zl5CilodTgyu5pnVILR1WN3vaMTIa16yrBvSqXUu3R0bd\n" \
"KpPDkC55gIDvEwRqFDu1m5K+wgdlTvza/P96rtxcflUxDOg5B6TXvi/TC2rSsd9f\n" \
"/ld0Uzs1gN2ujkSYs58O09rg1/RrKatEp0tYhG2SS4HD2nOLEpdIkARFdRrdNzGX\n" \
"kujNVA075ME/OV4uuPNcfhCOhkEAjUVmR7ChZc6gqikJTvOX6+guqw9ypzAO+sf0\n" \
"/RR3w6RbKFfCs/mC/bdFWJsCAwEAAaOCAVowggFWMBIGA1UdEwEB/wQIMAYBAf8C\n" \
"AQAwDgYDVR0PAQH/BAQDAgGGMDQGCCsGAQUFBwEBBCgwJjAkBggrBgEFBQcwAYYY\n" \
"aHR0cDovL29jc3AuZGlnaWNlcnQuY29tMHsGA1UdHwR0MHIwN6A1oDOGMWh0dHA6\n" \
"Ly9jcmwzLmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydEdsb2JhbFJvb3RDQS5jcmwwN6A1\n" \
"oDOGMWh0dHA6Ly9jcmw0LmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydEdsb2JhbFJvb3RD\n" \
"QS5jcmwwPQYDVR0gBDYwNDAyBgRVHSAAMCowKAYIKwYBBQUHAgEWHGh0dHBzOi8v\n" \
"d3d3LmRpZ2ljZXJ0LmNvbS9DUFMwHQYDVR0OBBYEFA+AYRyCMWHVLyjnjUY4tCzh\n" \
"xtniMB8GA1UdIwQYMBaAFAPeUDVW0Uy7ZvCj4hsbw5eyPdFVMA0GCSqGSIb3DQEB\n" \
"CwUAA4IBAQAjPt9L0jFCpbZ+QlwaRMxp0Wi0XUvgBCFsS+JtzLHgl4+mUwnNqipl\n" \
"5TlPHoOlblyYoiQm5vuh7ZPHLgLGTUq/sELfeNqzqPlt/yGFUzZgTHbO7Djc1lGA\n" \
"8MXW5dRNJ2Srm8c+cftIl7gzbckTB+6WohsYFfZcTEDts8Ls/3HB40f/1LkAtDdC\n" \
"2iDJ6m6K7hQGrn2iWZiIqBtvLfTyyRRfJs8sjX7tN8Cp1Tm5gr8ZDOo0rwAhaPit\n" \
"c+LJMto4JQtV05od8GiG7S5BNO98pVAdvzr508EIDObtHopYJeS4d60tbvVS3bR0\n" \
"j6tJLp07kzQoH3jOlOrHvdPJbRzeXDLz\n" \
"-----END CERTIFICATE-----\n";

 /** 
  *  If Slack changes their SSL cert, you will need to update this ^
  *  This cert is currently set to expire Feb 12, 2021, but could change sooner.
  *  
  *  The following command can be used to get the current ssl certificate:
  *  
  *  openssl s_client -connect api.slack.com:443
  *
  */

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
#define WORD_SEPERATORS "., \"'()[]<>;:-+&?!\n\t"

WiFiMulti WiFiMulti;

// create servo object to control a servo
Servo myservo;  

// Possible PWM GPIO pins on the ESP32: 0(used by on-board button),2,4,5(used by on-board LED),12-19,21-23,25-27,32-33 
int servoPin = 32;      // GPIO pin used to connect the servo control, on M5 it's on the Grove connector

WebSocketsClient webSocket;

unsigned long nextCmdId = 1UL; // signed longs overflow to a large negative value, which isn't what we want. Even though it would probably take 13 years :P
bool connected = false;


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
        if (strcasecmp(nextWord, "someword") == 0) {
          Serial.println("match!");
          myservo.write(180);
          Serial.println("up");
          delay(4000);
          myservo.write(0);
          Serial.println("down");
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
  Serial.println(type);
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
  WiFiClientSecure httpsClient;
  //httpsClient.setFingerprint(SLACK_SSL_FINGERPRINT);
  httpsClient.setCACert(rootCACertificate);
  httpsClient.setTimeout(15000); // 15 Seconds

  HTTPClient http;
  http.begin(httpsClient,"https://slack.com/api/rtm.connect?token=" SLACK_BOT_TOKEN");
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
  webSocket.beginSSL(host, 443, path);
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


  Serial.begin(115200);
  Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  // wait for WiFi connection
  Serial.print("Waiting for WiFi to connect...");
  while ((WiFiMulti.run() != WL_CONNECTED)) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" connected");

  setClock(); 
  //configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  Serial.println("connected");
  

  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);// Standard 50hz servo
  myservo.attach(servoPin, 500, 2400);   // attaches the servo on pin 18 to the servo object
                                         // using SG90 servo min/max of 500us and 2400us
                                         // for MG995 large servo, use 1000us and 2000us,
                                         // which are the defaults, so this line could be
                                         // "myservo.attach(servoPin);"


  M5.begin();

  delay(500);

  Serial.printf("STATUS(System) Ready \n\n");

}

// Not sure if WiFiClientSecure checks the validity date of the certificate. 
// Setting clock just to be sure...
void setClock() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}

unsigned long lastPing = 0;

void loop() {

  webSocket.loop();
  lastPing = connectAndPing( lastPing );

}
