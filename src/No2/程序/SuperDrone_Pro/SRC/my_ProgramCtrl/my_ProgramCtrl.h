#include "include.h"
#include "stm32f4xx.h"

extern s16 SK_X,SK_Y;
extern s8 Jetson_flag;
extern float UAV_speed_x,UAV_speed_y,UAV_speed_z;
extern s16 UAV_speed_yaw;


void User_task(u8 ms);     // 用户任务
void Test_task(u8 ms); //测试任务
void Test_show_task(u8 ms);
