#include <ESP8266Webhook.h> //https://github.com/Rupakpoddar/ESP8266Webhook
#include <ESP8266WiFi.h>
#include <espnow.h>

#define _SSID "YOUR-WIFI-SSID"      // Your WiFi SSID
#define _PASSWORD "YOUR-WIFI-PASSWORD"  // Your WiFi Password
//Refer https://randomnerdtutorials.com/door-status-monitor-using-the-esp8266/ for how to trigger email via IFTTT Web Hooks
#define KEY "IFTTT-KEY"        // Webhooks Key
#define EVENT "IFTTT-EVENT-NAME"      // Webhooks Event Name

Webhook webhook(KEY, EVENT);    // Create an object.

typedef struct struct_message {
    int b;
} struct_message;

struct_message myData;

bool isDetected = false;

void SendEMail() {
  int response = webhook.trigger();
  if(response == 200)
    Serial.println("Email triggered.");
  else
    Serial.println("Email failed.");    
}

void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Motion Detected: ");
  Serial.println(myData.b);
  Serial.println();

  isDetected = myData.b == 1;
}

bool EspNowInit() {
  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return false;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);  

  Serial.println("Initialized ESP-NOW");
  return true;
}

void ConnectToWiFiNetwork() {
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(_SSID);
  WiFi.begin(_SSID, _PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("-");
  }
  Serial.println("");
  Serial.println("WiFi Connected");
}

void setup() {
  Serial.begin(9600);
  //both AP and Station mode
  WiFi.mode(WIFI_AP_STA);
  WiFi.disconnect();
  delay(1000);

  ConnectToWiFiNetwork();

  EspNowInit();
}

void loop() {
  if (isDetected) {
    delay(1000);

    SendEMail();

    isDetected = false;
  }
}
