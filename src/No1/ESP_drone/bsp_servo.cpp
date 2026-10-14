#include "bsp_servo.h"

// 将角度转换为脉宽值
int angleToPulseWidth(int angle) {
  int pulseWidth = map(angle, 0, 270, SERVO_MIN_PULSEWIDTH, SERVO_MAX_PULSEWIDTH);
  return pulseWidth;
}

// 设置舵机角度并控制转速
void setServoAngle(int angle, int speed) {
  if(angle<=45){
    angle=45;
  }
  else if(angle>=225){
    angle=225;
  }
  int pulseWidth = angleToPulseWidth(angle);
  int dutyCycle = (8191 * pulseWidth) / (1000000 / LEDC_BASE_FREQ);  // 转换为13位的占空比值
  ledcWrite(LEDC_CHANNEL_0, dutyCycle);
  delay(30+speed);  // 控制转速，通过延迟时间实现
}

// 将角度转换为脉宽值
int angleToPulseWidth_sp(int angle) {
  int pulseWidth = map(angle, 0, 180, SERVO_MIN_PULSEWIDTH, SERVO_MAX_PULSEWIDTH);
  return pulseWidth;
}

// 设置舵机角度并控制转速
void setServoAngle_sp(int angle, int speed) {
  if(angle<=0){
    angle=0;
  }
  else if(angle>=180){
    angle=180;
  }
  int pulseWidth = angleToPulseWidth(angle);
  int dutyCycle = (8191 * pulseWidth) / (1000000 / LEDC_BASE_FREQ);  // 转换为13位的占空比值
  ledcWrite(LEDC_CHANNEL_1, dutyCycle);
  delay(speed);  // 控制转速，通过延迟时间实现
}

void servo_init(){
   // 配置LEDC定时器
  ledcSetup(LEDC_CHANNEL_0|LEDC_CHANNEL_1|LEDC_CHANNEL_2, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  // 将LEDC通道附加到GPIO引脚
  ledcAttachPin(SERVO_PIN, LEDC_CHANNEL_0);
  
  // ledcSetup(LEDC_CHANNEL_1, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  // 将LEDC通道附加到GPIO引脚
  ledcAttachPin(sp_left_spin_pin, LEDC_CHANNEL_1);
  // ledcSetup(LEDC_CHANNEL_2, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);  
  ledcAttachPin(sp_right_spin_pin, LEDC_CHANNEL_2);
}

