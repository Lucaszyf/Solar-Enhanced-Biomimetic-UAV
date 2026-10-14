#ifndef MY_Jetson_CTRL
#define MY_Jetson_CTRL

#include "stm32f4xx.h"

extern float camera_center_x;
extern float camera_center_y;

// Jetson 状态结构体
typedef struct
{   
  u8 offline; // 是否离线 1-离线 0-在线
  u8 mode_sta; // 模式状态
//   u16 offline_check_time; //超时容忍时间

} _my_Jetson_status;


//声明
extern _my_Jetson_status my_cb_Jetson_status; //用于色块追踪的Jetson状态结构体

// 色块检测相关 Jetson控制
void Jetson_CB_Offline_Check(u8 dT_ms);
void Jetson_CB_Check_Reset(void);



#endif



