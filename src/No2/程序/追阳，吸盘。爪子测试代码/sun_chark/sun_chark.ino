#include <Arduino.h>
#include <ESP32Servo.h>

// 定义舵机引脚
const int servoPin = 12;
const int servoPin_Host = 27;

// 创建一个舵机对象
Servo myServo;
Servo myServo_B;
// 定义ADC引脚
const int adcPin1 = 32; // 上方光敏电阻
const int adcPin2 = 34; // 下方光敏电阻

// 定义舵机角度范围
int currentAngle = 90; // 初始角度为90度（中间位置）
const int minAngle = 0; // 最小角度
const int maxAngle = 180; // 最大角度
//触碰开关
const int pin_up = 25;
const int pin_down = 26;

void setup() {
  // 初始化串口通讯
  Serial.begin(115200);
  Serial2.begin(115200);

  // 附加舵机到指定的引脚
  myServo.attach(servoPin);
  myServo.write(currentAngle); // 将舵机移动到初始位置
  //无人机机载舵机
  myServo_B.attach(servoPin_Host);
  myServo_B.write(100); // 将舵机移动到初始位置
  // 设置ADC引脚为输入模式
  pinMode(adcPin1, INPUT);
  pinMode(adcPin2, INPUT);
  pinMode(39, INPUT);

  pinMode(18,OUTPUT);
  pinMode(19,OUTPUT);
  //气泵
  pinMode(2,OUTPUT);
  pinMode(15,OUTPUT);
  pinMode(21,OUTPUT);
  //太阳能
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  //夹爪
  pinMode(25,INPUT_PULLUP);
  pinMode(26,INPUT_PULLUP);

  digitalWrite(18,HIGH);
  digitalWrite(19,HIGH);

  digitalWrite(4,HIGH);
  digitalWrite(5,HIGH);

  digitalWrite(2,LOW);
  digitalWrite(15,LOW);
  digitalWrite(21,LOW);
}
int adcValue1;
int adcValue2;
int adcValue3;
int adcValue4;
char zhua_close = 0;
void loop() {
  // 读取ADC值
  adcValue1 = analogRead(adcPin1) - 1140;
  adcValue2 = analogRead(adcPin2);
  adcValue3 = analogRead(36);
  adcValue4 = analogRead(39);

  // 将ADC值输出到串口监视器
  // Serial.print("ADC Value at D33: ");
  // Serial.print(adcValue1);
  // Serial.print("\tADC Value at D32: ");
  // Serial.println(adcValue2);
  // Serial.print("\tADC Value at D36: ");
  // Serial.println(adcValue3);
  Serial.print("\tADC Value at D32: ");
  Serial.println(adcValue1);
  Serial.print("\tADC Value at D34: ");
  Serial.println(adcValue2);
  zhuazi_Get();
  // 比较两个光敏电阻的值，调整舵机角度
  if (adcValue1 - adcValue2> 30 && currentAngle < maxAngle) {
    // 如果上方光照弱，且当前角度小于最大角度，则增加角度
    currentAngle += 2;
    //myServo.write(currentAngle);
  } else if (adcValue2 - adcValue1 > 30 && currentAngle > minAngle) {
    // 如果下方光照弱，且当前角度大于最小角度，则减少角度
    currentAngle -= 2;
    //myServo.write(currentAngle);
  }
  if (Serial.available() > 0) {
    char receivedData = Serial.read();
    if(receivedData == 'A')
    {
      digitalWrite(4,HIGH);
      digitalWrite(5,LOW);
    }else if(receivedData == 'B')
    {
      digitalWrite(4,LOW);
      digitalWrite(5,HIGH);
    }else if(receivedData == 'C')
    {
      digitalWrite(4,HIGH);
      digitalWrite(5,HIGH);
    }else if(receivedData == 'D')
    {
      digitalWrite(2,LOW);
    }else if(receivedData == 'E')
    {
      digitalWrite(2,HIGH);
    }else if(receivedData == '1')
    {
      digitalWrite(4,HIGH);
      digitalWrite(5,LOW);
    }else if(receivedData == '2')
    {
      digitalWrite(4,LOW);
      digitalWrite(5,HIGH);
    }else if(receivedData == '3')
    {
      digitalWrite(4,HIGH);
      digitalWrite(5,HIGH);
      zhua_close = 0;
    }
  }
  if (Serial2.available() > 0) {
    char receivedData = Serial2.read();
    if(receivedData == 'A')
    {
      digitalWrite(4,HIGH);
      digitalWrite(5,LOW);
    }else if(receivedData == 'B')
    {
      digitalWrite(4,LOW);
      digitalWrite(5,HIGH);
    }else if(receivedData == 'C')
    {
      digitalWrite(4,HIGH);
      digitalWrite(5,HIGH);
    }else if(receivedData == 'D')
    {
      digitalWrite(2,LOW);
    }else if(receivedData == 'E')
    {
      digitalWrite(2,HIGH);
    }else if(receivedData == '1')
    {
      digitalWrite(4,HIGH);
      digitalWrite(5,LOW);
    }else if(receivedData == '2')
    {
      digitalWrite(4,LOW);
      digitalWrite(5,HIGH);
    }else if(receivedData == '3')
    {
      digitalWrite(4,HIGH);
      digitalWrite(5,HIGH);
      zhua_close = 0;
    }
  }
  // 延时100毫秒，避免调整过快
  delay(50);
}

void zhuazi_Get()
{
  if(digitalRead(25) == 0 && zhua_close == 0)
  {
    digitalWrite(4,LOW);
    digitalWrite(5,HIGH);
  }
  if(digitalRead(26) == 0)
  {
    digitalWrite(4,HIGH);
    digitalWrite(5,HIGH);
    while(digitalRead(26) == 0)
    {
      digitalWrite(4,LOW);
      digitalWrite(5,HIGH);
    }
    digitalWrite(4,HIGH);
    digitalWrite(5,HIGH);
  }
  if(adcValue3 > 3000)
  {
    digitalWrite(4,HIGH);
    digitalWrite(5,HIGH);
    zhua_close = 1;
  }
  if(adcValue4 > 1200)
  {
    digitalWrite(15,HIGH);
  }else if(adcValue4 < 800)
  {
    digitalWrite(15,LOW);
  }
}
