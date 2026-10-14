#ifndef BSP_USART_H
#define BSP_USART_H
#include "Arduino.h"
#include "bsp_solarpanel.h"

extern unsigned int lasercombined;  
extern uint8_t buffer[];
extern uint8_t sun_state_num,sun_state_input,sun_state_output;

void usart_rec1(void);
void usart_rec2(void);


#endif