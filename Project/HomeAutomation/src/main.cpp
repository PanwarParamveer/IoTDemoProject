
#include <Arduino.h>
// #define LED D0            // Led in NodeMCU at pin GPIO16 (D0).
// void setup() {
// pinMode(LED, OUTPUT);    // LED pin as output.
// }
// void loop() {
// digitalWrite(LED, HIGH);// turn the LED off.(Note that LOW is the voltage level but actually
//                         //the LED is on; this is because it is acive low on the ESP8266.
// delay(200);            // wait for 1 second.
// digitalWrite(LED, LOW); // turn the LED on.
// delay(200); // wait for 1 second.
// }

// const int tmp = A0;
// void setup() {
//   Serial.begin(9600);
//   // put your setup code here, to run once:
// }

// void loop() {
// int analogValue = analogRead(tmp);
// float millivolts = (analogValue/1024.0) * 3300; //3300 is the voltage provided by NodeMCU
// float celsius = millivolts/10;
// Serial.print("in DegreeC=   ");
// Serial.println(celsius);

// //---------- Here is the calculation for Fahrenheit ----------//

// float fahrenheit = ((celsius * 9)/5 + 32);
// Serial.print(" in Farenheit=   ");
// Serial.println(fahrenheit);
// delay(1000);
// }

#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif

// //Provide the token generation process info.
#include "addons/TokenHelper.h"
// //Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

/* 1. Define the WiFi credentials */
#define WIFI_SSID "Param"
#define WIFI_PASSWORD "panwar@2021"

/* 2. Define the API Key */
#define API_KEY "AIzaSyB0POpgvkCMQIgGptZAF2RCll9R5gzFpWw"

/* 3. Define the RTDB URL */
#define DATABASE_URL "https://params-iot-device.firebaseio.com/" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "iot@test.com"
#define USER_PASSWORD "123456"

//Define Firebase Data object
FirebaseData stream;
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

int count = 0;

void streamCallback(StreamData data)
{
  // Serial.printf("sream path, %s\nevent path, %s\ndata type, %s\nevent type, %s\n\n",
  //               data.streamPath().c_str(),
  //               data.dataPath().c_str(),
  //               data.dataType().c_str(),
  //               data.eventType().c_str());
  printResult(data); //see addons/RTDBHelper.h
  Serial.println(data.intData());
  if (data.intData() == 1)
  {
     Serial.print("Command: ");
     Serial.println(data.intData());
    digitalWrite(D0, HIGH);
  }
  else if (data.intData() == 0)
  { Serial.print("Command: ");
     Serial.println(data.intData());
    digitalWrite(D0, LOW);
  }
  else
  {
    Serial.println("Unknown Command...\n");
  }
}

void streamTimeoutCallback(bool timeout)
{
  if (timeout)
    Serial.println("stream timeout, resuming...\n");
}

void setup()
{

  Serial.begin(9600);
  
  pinMode(D0, OUTPUT);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);

  //Or use legacy authenticate method
  //Firebase.begin(DATABASE_URL, "<database secret>");

  Firebase.reconnectWiFi(true);

  //The data under the node being stream (parent path) should keep small
  //Large stream payload leads to the parsing error due to memory allocation.

  if (!Firebase.beginStream(stream, "/State"))
    Serial.printf("sream begin error, %s\n\n", stream.errorReason().c_str());

  Firebase.setStreamCallback(stream, streamCallback, streamTimeoutCallback);
}

void loop()
{

  if (Firebase.ready() && (millis() - sendDataPrevMillis > 30000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();
    count++;
    FirebaseJson json;
    json.add("Value", count);
    Serial.printf("Set json... %s\n\n", Firebase.setJSON(fbdo, "/Temperature", json) ? "ok" : fbdo.errorReason().c_str());
  }
}