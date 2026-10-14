#include <esp_now.h>
//#include <WiFi.h>
#include "bsp_gpio.h"
#include "bsp_servo.h"
#include "bsp_attachment.h"
#include "bsp_usart.h"
#include "bsp_solarpanel.h"
#include "bsp_SW.h"
// #include "LobotSerialServoControl.h" // 导入库文件
#include "FashionStar_UartServoProtocol.h" // 串口总线舵机通信协议
#include "FashionStar_UartServo.h" // Fashion Star串口总线舵机的依赖

// 串口总线舵机配置
#define USERVO_BAUDRATE (uint32_t)115200 // 波特率
// 串口1舵机管理器
FSUS_Protocol protocol_ch1(&Serial, USERVO_BAUDRATE);
FSUS_Servo uservo_0(0, &protocol_ch1); // 创建舵机
FSUS_Servo uservo_1(1, &protocol_ch1); // 创建舵机
// #define SERVO_SERIAL_RX   35
// #define SERVO_SERIAL_TX   5
// HardwareSerial HardwareSerial(0);
// LobotSerialServoControl BusServo(HardwareSerial);
//变量区
// 定义串口任务句柄
TaskHandle_t SerialTaskHandle;

void GPIO_INIT(){
  //IO模式初始化
  //爪子：气泵电磁阀气压计
  pinMode(pump_pin, OUTPUT);
  pinMode(e_valve, OUTPUT);
  pinMode(air_sensor_pin, INPUT);
  //爪子：n20，压力，内触碰开关，外触碰开关
  pinMode(claw1_pin, OUTPUT);
  pinMode(claw2_pin, OUTPUT);
  pinMode(claw_pressure_pin, INPUT);  //模拟输入
  pinMode(claw_open_button_pin, INPUT_PULLUP);
  pinMode(claw_sw_pin, INPUT_PULLUP);
  //太阳能板：n20电机，舵机，光照传感器
  pinMode(sp_pin1, OUTPUT);
  pinMode(sp_pin2, OUTPUT);
  pinMode(sp_left_spin_pin, OUTPUT);
  pinMode(sp_right_spin_pin, OUTPUT);
  pinMode(mid_servo_pin, OUTPUT);
  pinMode(sp_light_sensor_up, INPUT);
  pinMode(sp_light_sensor_down, INPUT);
  //摄像头图传开关
  pinMode(camera_sw, OUTPUT);
}
 
void setup() {
  //Init Serial Monitor
  Serial.begin(115200,SERIAL_8N1,-1,26);
  Serial2.begin(115200,SERIAL_8N1,16,17);
 
  // Set device as a Wi-Fi Station
  // WiFi.mode(WIFI_STA);
  // if (esp_now_init() != ESP_OK) {
  //   Serial.println("Error initializing ESP-NOW");
  //   return;
  // }
  // esp_now_register_send_cb(OnDataSent);
  // memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  // peerInfo.channel = 0;  
  // peerInfo.encrypt = false;   
  // if (esp_now_add_peer(&peerInfo) != ESP_OK){
  //   Serial.println("Failed to add peer");
  //   return;
  // }
  // BusServo.OnInit();
  // HardwareSerial.begin(115200,SERIAL_8N1,SERVO_SERIAL_RX,SERVO_SERIAL_TX);
  // // BusServo.LobotSerialServoMove(1,90,1000); // 设置1号舵机运行到500脉宽位置，运行时间为1000毫秒
  // BusServo.LobotSerialServoMove(1,500,1000); // 设置1号舵机运行到500脉宽位置，运行时间为1000毫秒
  // delay(1000);
  uservo_0.init(); 
  uservo_1.init();
  uservo_0.setSpeed(5);
  uservo_1.setSpeed(5);
  delay(500);
  uservo_0.setRawAngle(0);  // 设置舵机的角度
  delay(500);
  uservo_1.setRawAngle(-90);  // 设置舵机的角度
  delay(500);
  GPIO_INIT();
  //模块初始化
  servo_init();
  // 创建串口任务
  xTaskCreatePinnedToCore(
      serialEventTask,   // 任务函数
      "SerialEventTask", // 任务名称
      8192,              // 任务堆栈大小
      NULL,              // 任务参数
      1,                 // 任务优先级
      &SerialTaskHandle, // 任务句柄
      1                  // 任务运行在核心1上
  );
  // digitalWrite(claw1_pin, LOW);   //闭合
  // digitalWrite(claw2_pin, HIGH);
  // delay(1000);
  // sp_open(65,1800);  //太阳能板以50bg速度开
  // sp_close(65,1800);//太阳能板以50速度关,延时1800ms

}
int ce_data = 0;
void loop() {
  // Send message via ESP-NOW
  // esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  //usart_rec1();
  if(sun_state_num == 1)sun_chark();
  // sp_open(65,1800);
  // sp_close(65,1800);
  if(sun_state_input == 1)
  {
    sp_open(55,1800);
    sun_state_input = 0;
  }
  if(sun_state_output == 1)
  {
    sp_close(55,1800);
    sun_state_output = 0;
  }
  if(ce_data == 1)
  {
    servo_auto();
    ce_data = 0;
  }
  attach_to_object();
  // usart_rec();
  // arm_mode=digitalRead(arm_mode_sw);
  // interpret_SW();
  // Serial2.print(arm_mode);
}
void servo_auto()
{
  for(int a = 0 ; a<180; a += 1){
    int c = 180 - a;
    myServo_B.write(a);
    myServo_C.write(c);
    uservo_0.setRawAngle(a);
    delay(150);
  }
  for(int a = 0 ; a<180; a += 1){
    int c = 180 - a;
    myServo_B.write(a);
    myServo_C.write(c);
    uservo_0.setRawAngle(a);
    delay(150);
  }
  for(int a = 180 ; a>=0; a -= 1){
    int c = 180 - a;
    myServo_B.write(a);
    myServo_C.write(c);
    uservo_0.setRawAngle(a);
    delay(150);
  }
  myServo_C.write(0);
  myServo_B.write(0);
}
void serialEventTask(void *pvParameters) {
  while (1) {
    // if (Serial.available() > 0) {
    //   char receivedData = Serial.read();
    //   if (receivedData == 'A') {//爪开
    //     open_arm();
    //     claw_close_state = 0;
    //   } else if (receivedData == 'B') {//爪关
    //     close_arm();
    //     claw_close_state = 0;
    //   } else if (receivedData == 'C') {//爪停
    //     stop_arm();
    //     claw_close_state = 0;
    //   } else if (receivedData == 'D') {//开关泵
    //     digitalWrite(e_valve, !digitalRead(e_valve));
    //     air_state = 0;
    //   } else if (receivedData == 'E') {//开关阀
    //     digitalWrite(pump_pin, !digitalRead(pump_pin));
    //     air_state = 0;
    //   } else if (receivedData == 'O') { // 自动追阳模式
    //     sun_state_num = !sun_state_num;
    //   } else if (receivedData == 'L') { // 放出太阳能
    //     sun_state_input = !sun_state_input;
    //   } else if (receivedData == 'M') { // 收回太阳能
    //     sun_state_output = !sun_state_output;
    //   } else if (receivedData == 'F') {//关闭图传
    //     digitalWrite(camera_sw, LOW);
    //   } else if (receivedData == 'G') {//开启图传
    //     digitalWrite(camera_sw, HIGH);
    //   }else if (receivedData == '1') {//切换爪子和吸盘
    //     arm_mode = !arm_mode;
    //     digitalWrite(e_valve, HIGH);
    //     stop_arm();
    //     air_state = 0;
    //   }else if (receivedData == '2') {//开启关闭自动吸附
    //     air_state = !air_state;
    //   }
    //   receivedData = 0;
    // }
    // if (Serial2.available() > 0) {
    //   char receivedData = Serial2.read();
    //   Serial.println(receivedData);
    //   if (receivedData == 'A') {//爪开
    //     open_arm();
    //     claw_close_state = 0;
    //   } else if (receivedData == 'B') {//爪关
    //     close_arm();
    //     claw_close_state = 0;
    //   } else if (receivedData == 'C') {//爪停
    //     stop_arm();
    //     claw_close_state = 0;
    //   } else if (receivedData == 'D') {//开关泵
    //     digitalWrite(e_valve, !digitalRead(e_valve));
    //     air_state = 0;
    //   } else if (receivedData == 'E') {//开关阀
    //     digitalWrite(pump_pin, !digitalRead(pump_pin));
    //     air_state = 0;
    //   } else if (receivedData == 'O') { // 自动追阳模式
    //     sun_state_num = !sun_state_num;
    //   } else if (receivedData == 'L') { // 放出太阳能
    //     sun_state_input = !sun_state_input;
    //   } else if (receivedData == 'M') { // 收回太阳能
    //     sun_state_output = !sun_state_output;
    //   } else if (receivedData == 'F') {//关闭图传
    //     digitalWrite(camera_sw, LOW);
    //   } else if (receivedData == 'G') {//开启图传
    //     digitalWrite(camera_sw, HIGH);
    //   }else if (receivedData == '1') {//切换爪子和吸盘
    //     arm_mode = !arm_mode;
    //     digitalWrite(e_valve, HIGH);
    //     stop_arm();
    //     air_state = 0;
    //   }else if (receivedData == '2') {//开启关闭自动吸附
    //     air_state = !air_state;
    //   }
    //   receivedData = 0;
    // }
    if (Serial2.available() > 0) {
      char receivedData = Serial2.read();
      //Serial.println(receivedData);
      if (receivedData == 'A') {//爪开
        //open_arm();
        ce_data = 1;
        claw_close_state = 0;
      } else if (receivedData == 'B') {//爪关
        close_arm();
        claw_close_state = 0;
      } else if (receivedData == 'C') {//爪停
        stop_arm();
        claw_close_state = 0;
      } else if (receivedData == 'L') {//开关泵
        digitalWrite(e_valve, !digitalRead(e_valve));
        air_state = 0;
      } else if (receivedData == 'M') {//开关阀
        digitalWrite(pump_pin, !digitalRead(pump_pin));
        air_state = 0;
      } else if (receivedData == 'D') { // 放出太阳能
        sun_state_input = !sun_state_input;
      } else if (receivedData == 'E') { // 收回太阳能
        sun_state_output = !sun_state_output;
      } else if (receivedData == '1') {//切换爪子和吸盘
        arm_mode = !arm_mode;
        digitalWrite(e_valve, HIGH);
        stop_arm();
        air_state = 0;
      }else if (receivedData == '2') {//开启关闭自动吸附
        air_state = !air_state;
      }
      receivedData = 0;
    }
    vTaskDelay(10 / portTICK_PERIOD_MS); // 避免任务过度占用CPU时间
  }
}