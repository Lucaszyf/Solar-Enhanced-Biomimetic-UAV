#include "my_Jetson_Ctrl.h"

// 基本参数 设定
#define Jetson_OFFLINE_TIME_MS 200 // 毫秒
// 摄像头中心xy坐标
float camera_center_x = 160;
float camera_center_y = 120;

_my_Jetson_status my_cb_Jetson_status; // 用于色块追踪的Jetson状态结构体
u16 cb_offline_check_time = 0; // 用于色块追踪的Jetson超时时间

/**********************************************************************************************************
 *函 数 名: Jetson_CB_Offline_Check
 *功能说明: 用于色块追踪的Jetson掉线检测，用来检测硬件是否在线
 *参    数: 时间（毫秒）
 *返 回 值: 无
 **********************************************************************************************************/
void Jetson_CB_Offline_Check(u8 dT_ms)  //时间一直在执行，Jetson一直给单片机发送数据！！！（cb_offline_check_time清零）//超过时间证明断线
{
    if (cb_offline_check_time < Jetson_OFFLINE_TIME_MS) {
        cb_offline_check_time += dT_ms;
    } else // 超时 认为掉线
    {
        my_cb_Jetson_status.offline = 1;  
    }
}

/**********************************************************************************************************
 *函 数 名: Jetson_CB_Check_Reset
 *功能说明: 用于色块追踪的Jetson掉线检测复位，证明没有掉线
 *参    数: 无
 *返 回 值: 无
 **********************************************************************************************************/
void Jetson_CB_Check_Reset(void)  //数据接收成功说明Jetson在线
{
    cb_offline_check_time = 0;
    my_cb_Jetson_status.offline = 0;
}
