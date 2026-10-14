#include "FashionStar_UartServoProtocol.h" // 串口总线舵机通信协议
#include "FashionStar_UartServo.h" // Fashion Star串口总线舵机的依赖

#define USERVO_BAUDRATE (uint32_t)115200 // 波特率
#define SERVO_SERIAL_RX   35
#define SERVO_SERIAL_TX   5
HardwareSerial HardwareSerial(0);


FSUS_Protocol protocol_ch1(&HardwareSerial, USERVO_BAUDRATE);

FSUS_Servo uservo_0(0, &protocol_ch1); // 创建舵机

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(115200);
  HardwareSerial.begin(115200,SERIAL_8N1,SERVO_SERIAL_RX,SERVO_SERIAL_TX);
  protocol_ch1.init();
  uservo_0.init(); 
}

void loop() {
  // put your main code here, to run repeatedly:
  protocol_ch1.sendSetAngle(1,90,0,0);
}
