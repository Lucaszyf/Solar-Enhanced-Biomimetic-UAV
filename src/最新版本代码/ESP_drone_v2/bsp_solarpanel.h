#ifndef BSP_SOLARPANEL_H
#define BSP_SOLARPANEL_H
#include "Arduino.h"
#include "bsp_gpio.h"
#include "bsp_servo.h"
#include "bsp_usart.h"

extern int best_angle; //最佳角度

void sp_close(int sp_speed,int run_time);  //控制开关速度
void sp_open(int sp_speed,int run_time);
void sp_angle_cal();
void sun_chark(void);

#endif