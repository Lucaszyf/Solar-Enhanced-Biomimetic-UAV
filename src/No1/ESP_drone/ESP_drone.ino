#include <esp_now.h>
#include <WiFi.h>
#include "bsp_gpio.h"
#include "bsp_servo.h"
#include "bsp_attachment.h"
#include "bsp_usart.h"
#include "bsp_solarpanel.h"
#include "bsp_SW.h"

//变量区
// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xC4, 0xDE, 0xE2, 0x9D, 0x56, 0xB0};  //接收端代码MAC地址

// Structure example to send data
typedef struct struct_message {
  char a[32];
  int b;
  float c;
  bool d;
} struct_message;

// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  Serial2.begin(115200);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;   
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  //IO模式初始化
  pinMode(SERVO_PIN, OUTPUT);

  pinMode(pump_pin, OUTPUT);
  pinMode(e_valve, OUTPUT);

  pinMode(claw_pressure_pin, INPUT);  //模拟输入
  pinMode(claw1_pin, OUTPUT);
  pinMode(claw2_pin, OUTPUT);
  pinMode(claw_sw_pin, INPUT);
  pinMode(claw_open_button_pin, INPUT);

  pinMode(sp_pin1, OUTPUT);
  pinMode(sp_pin2, OUTPUT);
  pinMode(sp_left_spin_pin, OUTPUT);
  pinMode(sp_right_spin_pin, OUTPUT);

  pinMode(camera_sw, OUTPUT);

  pinMode(light_sensor, INPUT);

  pinMode(air_sensor_pin, INPUT);

  pinMode(arm_mode_sw, INPUT);

  //模块初始化
  servo_init();

  setServoAngle(135,50);
}
 
void loop() {
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  usart_rec();
  arm_mode=digitalRead(arm_mode_sw);
  interpret_SW();
  Serial2.print(arm_mode);
}
