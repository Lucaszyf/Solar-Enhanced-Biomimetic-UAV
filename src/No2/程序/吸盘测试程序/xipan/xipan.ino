#include <ESP32Servo.h>
#include "BluetoothSerial.h"
#include "LobotSerialServoControl.h" // 导入库文件

BluetoothSerial SerialBT;
#define SERVO_SERIAL_RX   35
#define SERVO_SERIAL_TX   5
HardwareSerial HardwareSerial(0);
LobotSerialServoControl BusServo(HardwareSerial);

// const int servoPin_Host = 27;
// Servo myServo;

void setup() {
  // put your setup code here, to run once:
  // 初始化串口通讯
  Serial.begin(115200);
  Serial2.begin(115200);
  SerialBT.begin("Lucas"); 

  pinMode(2,OUTPUT);
  pinMode(15,OUTPUT);
  pinMode(39, INPUT);

  digitalWrite(2,LOW);
  digitalWrite(15,LOW);
  // 附加舵机到指定的引脚
  // myServo.attach(servoPin_Host);
  // myServo.write(40); // 将舵机移动到初始位置
  BusServo.OnInit();
  HardwareSerial.begin(115200,SERIAL_8N1,SERVO_SERIAL_RX,SERVO_SERIAL_TX);
  BusServo.LobotSerialServoMove(1,90,1000); // 设置1号舵机运行到500脉宽位置，运行时间为1000毫秒
  delay(1000);
}
int adcValue1,con_num = 0;
void loop() {
  // put your main code here, to run repeatedly:
  if (SerialBT.available() > 0) {
    char receivedData = SerialBT.read();
    if(receivedData == 'A')
    {
      con_num = !con_num;
      digitalWrite(15,LOW);
    }else if(receivedData == 'B')
    {
      digitalWrite(2,LOW);
    }else if(receivedData == 'C')
    {
      digitalWrite(2,HIGH);
    }
  }
  if (Serial2.available() > 0) {
    char receivedData = Serial2.read();
    if(receivedData == 'A')
    {
      con_num = !con_num;
      digitalWrite(15,LOW);
    }else if(receivedData == 'B')
    {
      digitalWrite(2,LOW);
    }else if(receivedData == 'C')
    {
      digitalWrite(2,HIGH);
    }
  }
  if (Serial.available() > 0) {
    char receivedData = Serial.read();
    Serial.print(receivedData);
    if(receivedData == 'A')
    {
      con_num = !con_num;
      digitalWrite(15,LOW);
    }else if(receivedData == 'B')
    {
      digitalWrite(2,LOW);
    }else if(receivedData == 'C')
    {
      digitalWrite(2,HIGH);
    }
  }
  if(con_num == 1)
  {
    adcValue1 = analogRead(39);
    Serial.println(adcValue1);
    if(adcValue1 > 1250)
    {
      digitalWrite(15,HIGH);
    }else if(adcValue1 < 900)
    {
      digitalWrite(15,LOW);
    }
  }
}
