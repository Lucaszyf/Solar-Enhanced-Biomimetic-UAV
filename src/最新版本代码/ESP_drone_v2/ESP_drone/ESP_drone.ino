#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "bsp_gpio.h"
#include "bsp_servo.h"
#include "bsp_attachment.h"
#include "bsp_usart.h"
#include "bsp_solarpanel.h"
#include "bsp_SW.h"
#include "LobotSerialServoControl.h" // 导入库文件
#include <ESP32Servo.h>

// ---------- 舵机对象 ----------
Servo servoB;
Servo servoC;

// ===== WIFI及NTP配置 =====
const char *ssid = "Zhang's iPhone";        // WIFI账户
const char *password = "aaaaaaaa";      // WIFI密码

#define NTP1 "ntp1.aliyun.com"
#define NTP2 "ntp2.aliyun.com"
#define NTP3 "ntp3.aliyun.com"

// ===== 舵机引脚定义 =====
#define sp_left_spin_pin  12    // 太阳能转动电机左通道，寻找光强的舵机
#define sp_right_spin_pin 13    // 太阳能转动电机右通道
#define BUS_SERVO_PIN 14        // 总线舵机假设接在GPIO14（此处为伪代码）

// ===== 总线舵机串口配置 =====
#define SERVO_SERIAL_RX   35
#define SERVO_SERIAL_TX   5

// ===== 命令数据结构 =====
// 每组命令包含：指定的时间（时和分）、舵机B/C平行转动角度、总线舵机角度
#define MAX_COMMANDS 10
struct Command {
  int hour;       // 命令执行的小时（0～23）
  int minute;     // 命令执行的分钟（0～59）
  int pairAngle;  // 舵机B/C平行转动角度（0～90）
  int busAngle;   // 总线舵机角度（范围按实际要求设定）
};

Command commands[MAX_COMMANDS];
int commandCount = 0;    // 当前有效命令数量
int currentCommand = 0;  // 当前待执行的命令索引

// ===== 串口命令数据缓存 =====
String serialData = "";

// ===== 辅助函数 =====

// 判断当前网络时间是否已达到或超过命令规定的时分
bool isTimeReached(const Command &cmd) {
  struct tm timeInfo;
  if (!getLocalTime(&timeInfo)) {
    Serial.println("未能获取当前网络时间");
    return false;
  }
  int currentMinutes = timeInfo.tm_hour * 60 + timeInfo.tm_min;
  int cmdMinutes = cmd.hour * 60 + cmd.minute;
  return currentMinutes >= cmdMinutes;
}

// 解析命令字符串，例如："#1030,040,060,1250,060,080;"
void processSerialCommand(const String &cmdStr) {
  String cmd = cmdStr;
  cmd.trim(); // 去除多余空白和换行符
  Serial.print("收到命令字符串：");
  Serial.println(cmd);

  // 检查命令是否以 '#' 开头且以 ';' 结束
  if (!cmd.startsWith("#") || !cmd.endsWith(";")) {
    Serial.println("错误：命令格式不正确，应以 '#' 开始，以 ';' 结束");
    return;
  }
  
  // 去除起始符 '#' 和结束符 ';'
  String content = cmd.substring(1, cmd.length() - 1);
  content.trim();
  
  // 用逗号分割，理论上每3个数字构成一组命令
  int tokenCount = 0;
  int tokens[MAX_COMMANDS * 3]; // 临时存储解析出的数字
  int startIndex = 0;
  while (startIndex < content.length()) {
    int commaIndex = content.indexOf(',', startIndex);
    String token;
    if (commaIndex == -1) {
      token = content.substring(startIndex);
    } else {
      token = content.substring(startIndex, commaIndex);
    }
    token.trim();
    if (token.length() > 0) {
      int val = token.toInt();
      tokens[tokenCount++] = val;
    }
    if (commaIndex == -1) break;
    startIndex = commaIndex + 1;
    if (tokenCount >= MAX_COMMANDS * 3) break; // 超过最大命令数则退出
  }
  if (tokenCount % 3 != 0) {
    Serial.println("错误：命令数据不完整，每组应包含3个数值");
    return;
  }
  commandCount = tokenCount / 3;
  for (int i = 0; i < commandCount; i++) {
    int timeVal   = tokens[i * 3];   // 如 1030 表示 10:30
    int pairAngle = tokens[i * 3 + 1]; // 舵机B/C平行转动角度
    int busAngle  = tokens[i * 3 + 2]; // 总线舵机角度

    int hour = timeVal / 100;
    int minute = timeVal % 100;
    if (pairAngle < 0 || pairAngle > 90) {
      Serial.print("错误：第");
      Serial.print(i + 1);
      Serial.println("组数据，平行舵机角度需在0-90之间");
      continue;
    }
    // 根据需要可增加对 busAngle 的检查

    commands[i].hour = hour;
    commands[i].minute = minute;
    commands[i].pairAngle = pairAngle;
    commands[i].busAngle = busAngle;
  }
  Serial.print("解析到 ");
  Serial.print(commandCount);
  Serial.println(" 组命令，等待执行。");
  currentCommand = 0; // 重置命令索引，从第一组命令开始执行
}

// 检查串口（USB）是否有新命令数据，并处理一整行命令
void checkSerialInput() {
  if (Serial.available() > 0) {
    // 读取直到遇到分号';'
    String inputStr = Serial.readStringUntil(';');
    // 由于 readStringUntil() 不会包含结束符，因此手动添加
    inputStr += ";";
    // 如果输入中包含 '#'，认为是一条完整命令
    if (inputStr.indexOf('#') != -1) {
      processSerialCommand(inputStr);
    } else {
      Serial.print("接收到无效命令：");
      Serial.println(inputStr);
    }
  }
}

// ---------- 外设及GPIO初始化 ----------
void GPIO_INIT(){
  // IO模式初始化
  // 以下引脚定义参考 bsp_gpio.h 等头文件中定义的变量，请根据实际硬件修改
  // 爪子：气泵、电磁阀、气压传感器
  pinMode(pump_pin, OUTPUT);
  pinMode(e_valve, OUTPUT);
  pinMode(air_sensor_pin, INPUT);
  // 爪子：n20，压力，内触碰开关，外触碰开关
  pinMode(claw1_pin, OUTPUT);
  pinMode(claw2_pin, OUTPUT);
  pinMode(claw_pressure_pin, INPUT);  // 模拟输入
  pinMode(claw_open_button_pin, INPUT_PULLUP);
  pinMode(claw_sw_pin, INPUT_PULLUP);
  // 太阳能板：n20电机、舵机、光照传感器
  pinMode(sp_pin1, OUTPUT);
  pinMode(sp_pin2, OUTPUT);
  pinMode(sp_left_spin_pin, OUTPUT);
  pinMode(sp_right_spin_pin, OUTPUT);
  pinMode(sp_light_sensor_up, INPUT);
  pinMode(sp_light_sensor_down, INPUT);
  // 摄像头图传开关
  pinMode(camera_sw, OUTPUT);
}

// ---------- 全局串口及伺服控制对象 ----------
// 使用默认硬件串口（UART0）用于调试
// 使用另一个 HardwareSerial 对象与总线舵机通信
HardwareSerial MyServoSerial(0); // 注意：ESP32 的 UART0 一般用于 USB 调试，请根据实际需求调整
LobotSerialServoControl BusServo(MyServoSerial);

// ---------- 串口任务（用于处理来自其他串口，如 Serial2 的数据）----------
TaskHandle_t SerialTaskHandle;
void serialEventTask(void *pvParameters) {
  for (;;) {
    if (Serial2.available() > 0) {
      char receivedData = Serial2.read();
      Serial.print("Serial2接收: ");
      Serial.println(receivedData);
      // 根据接收到的数据控制其他外设（以下为示例，请根据实际需求调整）
      if (receivedData == 'A') {       // 爪开
        open_arm();
        claw_close_state = 0;
      } else if (receivedData == 'B') { // 爪关
        close_arm();
        claw_close_state = 0;
      } else if (receivedData == 'C') { // 爪停
        stop_arm();
        claw_close_state = 0;
      } else if (receivedData == 'D') { // 开关泵
        digitalWrite(e_valve, !digitalRead(e_valve));
        air_state = 0;
      } else if (receivedData == 'E') { // 开关阀
        digitalWrite(pump_pin, !digitalRead(pump_pin));
        air_state = 0;
      } else if (receivedData == 'O') { // 自动追阳模式
        sun_state_num = !sun_state_num;
      } else if (receivedData == 'L') { // 放出太阳能
        sun_state_input = !sun_state_input;
      } else if (receivedData == 'M') { // 收回太阳能
        sun_state_output = !sun_state_output;
      } else if (receivedData == 'F') { // 关闭图传
        digitalWrite(camera_sw, LOW);
      } else if (receivedData == 'G') { // 开启图传
        digitalWrite(camera_sw, HIGH);
      } else if (receivedData == '1') { // 切换爪子和吸盘
        arm_mode = !arm_mode;
        digitalWrite(e_valve, HIGH);
        stop_arm();
        air_state = 0;
      } else if (receivedData == '2') { // 开启/关闭自动吸附
        air_state = !air_state;
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS); // 避免任务占用过多CPU资源
  }
}

void setup() {
  // 初始化USB串口用于调试
  Serial.begin(115200);
  delay(1000); // 稍作等待，确保串口稳定
  Serial.println();
  
  // 初始化 Serial2（如用于其他设备通信，根据需要配置）
  Serial2.begin(115200);
  
  // ----- WiFi连接及NTP时间初始化 -----
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("正在连接WiFi");
  //while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  //}
  Serial.println("\nWiFi connected!");
  // 使用东八区（UTC+8）时间，连接三个阿里云NTP服务器
  configTime(8 * 3600, 0, NTP1, NTP2, NTP3);

  // ----- 舵机初始化 -----
  servoB.attach(sp_left_spin_pin);
  servoC.attach(sp_right_spin_pin);
  // 初始位置：设定 servoB 为180度，servoC 为0度
  servoB.write(180);
  servoC.write(0);

  // ----- 总线舵机初始化 -----
  // 注意：若 UART0 用于USB调试，可考虑换用其他硬件串口
  MyServoSerial.begin(115200, SERIAL_8N1, SERVO_SERIAL_RX, SERVO_SERIAL_TX);
  BusServo.OnInit();
  // 示例：控制1号舵机运行到500脉宽位置，运行时间1000毫秒
  BusServo.LobotSerialServoMove(1, 500, 1000);
  delay(1000);

  // ----- IO及其他外设初始化 -----
  GPIO_INIT();
  servo_init();

  // 创建用于处理 Serial2 数据的任务（如不需要，可注释掉）
  xTaskCreatePinnedToCore(
      serialEventTask,   // 任务函数
      "SerialEventTask", // 任务名称
      8192,              // 任务堆栈大小
      NULL,              // 任务参数
      1,                 // 任务优先级
      &SerialTaskHandle, // 任务句柄
      1                  // 任务运行在核心1上
  );

  Serial.println("等待指令，格式例如：");
  Serial.println("  #1030,040,060,1250,060,080;");
}

void loop() {
  // --- 1. 检查并处理 USB 串口命令 ---
  checkSerialInput();

  // --- 2. 检查并执行预定命令 ---
  if (commandCount > 0 && currentCommand < commandCount) {
    if (isTimeReached(commands[currentCommand])) {
      // 计算平行舵机的目标角度：
      // 例如：设定 servoB = 180 - pairAngle, servoC = pairAngle
      int targetPairAngle = commands[currentCommand].pairAngle;
      int targetServoB = 180 - targetPairAngle;
      int targetServoC = targetPairAngle;
      servoB.write(targetServoB);
      servoC.write(targetServoC);
      
      // 伪代码：设置总线舵机角度
      // 例如：BusServo.write(commands[currentCommand].busAngle);
      
      Serial.print("执行命令 ");
      Serial.print(currentCommand + 1);
      Serial.print(" （计划时间 ");
      Serial.print(commands[currentCommand].hour);
      Serial.print(":");
      Serial.print(commands[currentCommand].minute);
      Serial.print("）：舵机B = ");
      Serial.print(targetServoB);
      Serial.print("°, 舵机C = ");
      Serial.print(targetServoC);
      Serial.print("°, 总线舵机 = ");
      Serial.print(commands[currentCommand].busAngle);
      Serial.println("°");
      
      currentCommand++; // 执行完毕后指向下一条命令
    }
  }

  // --- 3. 其他周期性任务（例如太阳能板控制、附件操作等） ---
  // 这里保留了原有的调用，实际使用时请根据需要调整
  if(sun_state_num == 1) {
    sun_chark();
  }
  if(sun_state_input == 1) {
    sp_open(55, 1800);
    sun_state_input = 0;
  }
  if(sun_state_output == 1) {
    sp_close(55, 1800);
    sun_state_output = 0;
  }
  attach_to_object();

  // 每次循环延时 1 秒（可根据实际需求调整时间间隔）
  delay(1000);
}
