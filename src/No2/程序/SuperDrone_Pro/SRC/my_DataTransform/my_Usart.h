#ifndef MY_USART_H
#define MY_USART_H

#include "stm32f4xx.h"
#include "Ano_Filter.h" //滤波器
#include "my_OfflineCheck.h"

// 魔改串口1
void my_Usart1_Init(u32 br_num);
void my_Usart1_IRQ(void);
void my_Usart1_Send(unsigned char *DataToSend, u8 data_num);


// 串口接收函数
void Usart1_Byte_Get(u8 bytedata);
void Jetson_Receive(u8 data); // （Jetson）:T265s数据

// static 数据解析函数
static void Usart1_Data_Analysis(u8 *buf_data, u8 len);

extern s16 UAV_Positon[3]; // 无人机位置数组
extern s16 UAV_Speed[3];   // 无人机速度数组
extern s16 UAV_Rpy[3];     // 无人机角度数组
extern s16 normal_speed[3];
extern s16 raw_rpy[3];
extern s8 Green_Flag;

#endif
