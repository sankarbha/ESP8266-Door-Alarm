/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-esp8266-nodemcu-arduino-ide/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <ESP8266WiFi.h>
#include <espnow.h>

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    int b;
} struct_message;

// Create a struct_message called myData
struct_message myData;

const unsigned int buzzerPin = 14;
bool isDetected = false;
const unsigned int beepCount = 5;

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Motion Detected: ");
  Serial.println(myData.b);
  Serial.println();
  
  isDetected = myData.b == 1;
}
 
void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);
  
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);

  pinMode(buzzerPin, OUTPUT);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packet info
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

void blinkBeep() {
  //https://www.arduino.cc/reference/en/language/functions/advanced-io/tone/
  tone(buzzerPin, 1000, 50);
  digitalWrite(BUILTIN_LED, LOW);
  delay(500);
  
  noTone(buzzerPin);
  digitalWrite(BUILTIN_LED, HIGH);
  delay(500);
}

void loop() {
  if (isDetected) {
    for (int i = 0; i < beepCount; i++) {
      blinkBeep();
    }

    isDetected = false;
  }
}