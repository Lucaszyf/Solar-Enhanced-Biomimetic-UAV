#ifndef __BSP_ATTACHMENT_H
#define __BSP_ATTACHMENT_H

#include "Arduino.h"
#include "bsp_gpio.h"
#include "bsp_usart.h"

extern int arm_mode;
extern char claw_close_state;
extern bool air_state;
void attach_to_object();
void interpret_attach();
void auto_takeoff();

void open_arm();
void close_arm();
void stop_arm();
void attach_to_object();
#endif

