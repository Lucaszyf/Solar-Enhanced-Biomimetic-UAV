#include <Arduino.h>
#include <WiFi.h>
#include <ESP32Servo.h>

// ===== WIFI及NTP配置 =====
const char *ssid = "QS_OFFICE";        // WIFI账户
const char *password = "fablab108";      // WIFI密码

#define NTP1 "ntp1.aliyun.com"
#define NTP2 "ntp2.aliyun.com"
#define NTP3 "ntp3.aliyun.com"

// ===== 舵机引脚定义 =====
#define SERVO_B_PIN 12  // 舵机B连接到GPIO12
#define SERVO_C_PIN 13  // 舵机C连接到GPIO13
#define BUS_SERVO_PIN 14 // 总线舵机假设接在GPIO14（此处为伪代码）

Servo servoB;
Servo servoC;
// 伪代码：总线舵机
// Servo busServo;  

// ===== 命令数据结构 =====
// 每组命令包含：指定的时间（时和分）、舵机B/C平行转动角度（0～90）、总线舵机角度
#define MAX_COMMANDS 10
struct Command {
  int hour;       // 命令执行的小时（0～23）
  int minute;     // 命令执行的分钟（0～59）
  int pairAngle;  // 舵机B/C平行转动角度（0～90）
  int busAngle;   // 总线舵机角度（伪代码，范围按实际要求设定）
};

Command commands[MAX_COMMANDS];
int commandCount = 0;  // 当前有效命令数量
int currentCommand = 0; // 当前待执行的命令索引

// ===== 串口接收数据 =====
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

// 解析串口输入的命令字符串，例如：
//   "#1030,040,060,1250,060,080;"
void processSerialCommand(const String &cmd) {
  Serial.print("收到命令字符串：");
  Serial.println(cmd);

  // 检查命令是否以 '#' 开头
  if (!cmd.startsWith("#")) {
    Serial.println("错误：命令不以 '#' 开头");
    return;
  }
  // 去除起始符 '#' 和末尾的 ';'
  String content = cmd.substring(1, cmd.length() - 1); // 去除首尾特殊字符

  // 按逗号分隔，理论上每3个数值构成一组命令
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
    int timeVal   = tokens[i * 3];     // 如 1030 表示 10:30
    int pairAngle = tokens[i * 3 + 1];   // 舵机B/C平行转动角度
    int busAngle  = tokens[i * 3 + 2];   // 总线舵机角度

    int hour = timeVal / 100;
    int minute = timeVal % 100;
    if (pairAngle < 0 || pairAngle > 90) {
      Serial.print("错误：第");
      Serial.print(i + 1);
      Serial.println("组数据，平行舵机角度需在0-90之间");
      continue;
    }
    // 根据需要可以增加对busAngle的检查

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

// ===== setup =====
void setup() {
  Serial.begin(115200);
  Serial.println();

  // ----- WiFi连接及NTP时间初始化 -----
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("正在连接WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  // 使用东八区（UTC+8）时间，连接三个阿里云NTP服务器
  configTime(8 * 3600, 0, NTP1, NTP2, NTP3);

  // ----- 舵机初始化 -----
  servoB.attach(SERVO_B_PIN);
  servoC.attach(SERVO_C_PIN);
  servoB.write(180); // 初始位置：对应角度0°（servoB=180，servoC=0）
  servoC.write(0);

  // 伪代码：总线舵机初始化
  // busServo.attach(BUS_SERVO_PIN);
  // busServo.write(0);  // 初始位置

  Serial.println("等待指令，格式例如：");
  Serial.println("  #1030,040,060,1250,060,080;");
}

// ===== loop =====
void loop() {
  // --- 接收并解析串口数据 ---
  while (Serial.available() > 0) {
    char c = Serial.read();
    // 当收到起始符 '#' 时，清空之前的数据
    if (c == '#') {
      serialData = "";
    }
    serialData += c;
    // 当收到结束符 ';' 时，认为命令接收完毕，开始解析
    if (c == ';') {
      processSerialCommand(serialData);
      serialData = "";
    }
  }

  // --- 检查并执行命令 ---
  if (commandCount > 0 && currentCommand < commandCount) {
    // 检查当前网络时间是否已达到或超过当前命令的时分
    if (isTimeReached(commands[currentCommand])) {
      // 计算平行舵机的目标角度：servoB=180 - pairAngle, servoC=pairAngle
      int targetPairAngle = commands[currentCommand].pairAngle;
      int targetServoB = 180 - targetPairAngle;
      int targetServoC = targetPairAngle;
      servoB.write(targetServoB);
      servoC.write(targetServoC);
      
      // 伪代码：设置总线舵机角度
      // busServo.write(commands[currentCommand].busAngle);
      
      Serial.print("执行命令 ");
      Serial.print(currentCommand + 1);
      Serial.print("（计划执行时间 ");
      Serial.print(commands[currentCommand].hour);
      Serial.print(":");
      Serial.print(commands[currentCommand].minute);
      Serial.print("）：舵机B设置为 ");
      Serial.print(targetServoB);
      Serial.print("°，舵机C设置为 ");
      Serial.print(targetServoC);
      Serial.print("°，总线舵机设置为 ");
      Serial.print(commands[currentCommand].busAngle);
      Serial.println("°");
      
      currentCommand++; // 指向下一组命令
    }
  }

  // 每隔1秒检查一次网络时间
  delay(1000);
}
