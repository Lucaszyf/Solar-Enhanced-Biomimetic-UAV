#ifndef __BSP_SERVO_H
#define __BSP_SERVO_H

#include "Arduino.h"
#include "bsp_gpio.h"
#include <ESP32Servo.h>

// 定义LEDC通道和定时器
#define LEDC_CHANNEL_0 0
#define LEDC_CHANNEL_1 1
#define LEDC_CHANNEL_2 2
#define LEDC_TIMER_13_BIT 13
#define LEDC_BASE_FREQ 50  // 舵机通常使用50Hz的PWM信号

// 舵机最小和最大占空比对应的值（取决于舵机和PWM频率）
#define SERVO_MIN_PULSEWIDTH 500  // 对应0度的脉宽（微秒）
#define SERVO_MAX_PULSEWIDTH 2500 // 对应270度的脉宽（微秒）

void servo_init();
void setServoAngle(int angle, int speed);
void setServoAngle_sp(int angle, int speed);
void setServoAngle_back(int angle, int speed);
#endif

