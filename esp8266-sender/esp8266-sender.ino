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

// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  int b;
} struct_message;

// Create a struct_message called myData
struct_message myData;

//set your wifi network's channel. This is to facilitate the receiving agents to connect to internet
//I have tried using the method available here [https://github.com/m1cr0lab-esp32/esp-now-network-and-wifi-gateway/blob/47f324a9302d8111b85ceb99601f4458825f5bb1/src/sender.cpp#L18], 
//but doesn't seem to work for my home's hidden wifi network
unsigned int WIFI_CHANNEL = 5;

const int pirPin = 13;
int pirState = 0;
int lastPirState = 0;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(9600);

  pinMode(pirPin, INPUT);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, WIFI_CHANNEL, NULL, 0);
}
 
void loop() {
  pirState = digitalRead(pirPin);

  if (pirState != lastPirState){
    if (pirState == HIGH){
      Serial.println("Motion Detected.");
      myData.b = 1;
    }
    else {
      Serial.println("No Motion Detected.");
      myData.b = 0;
    }
    // Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  }

  lastPirState = pirState;
}
