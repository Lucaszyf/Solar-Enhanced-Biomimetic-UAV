#ifndef BSP_SOLARPANEL_H
#define BSP_SOLARPANEL_H
#include "Arduino.h"
#include "bsp_gpio.h"
#include "bsp_servo.h"
#include "bsp_usart.h"

extern int best_angle; //最佳角度

void sp_close();
void sp_open();
void sp_angle_cal();

#endif