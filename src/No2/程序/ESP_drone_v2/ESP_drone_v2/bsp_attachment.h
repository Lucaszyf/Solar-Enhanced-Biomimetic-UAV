#ifndef __BSP_ATTACHMENT_H
#define __BSP_ATTACHMENT_H

#include "Arduino.h"
#include "bsp_gpio.h"
#include "bsp_usart.h"

extern int arm_mode;

void attach_to_object();
void interpret_attach();
void auto_takeoff();

#endif

