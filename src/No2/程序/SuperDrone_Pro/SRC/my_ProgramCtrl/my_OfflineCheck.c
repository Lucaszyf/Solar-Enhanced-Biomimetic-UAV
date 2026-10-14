#include "my_OfflineCheck.h"


//--------------------------   K210 (0x42) 断线检测   -------------------------------------
u8 K210_42_offline_flag        = 1;
u16 K210_42_offline_check_time = 0; // 用于色块追踪的k210超时时间
#define K210_42_OFFLINE_TIME_MS 200 // 毫秒

void K210_42_Offline_Check(u8 dT_ms)
{
    if (K210_42_offline_check_time < K210_42_OFFLINE_TIME_MS) {
        K210_42_offline_check_time += dT_ms;
    } else // 超时 认为掉线
    {
        K210_42_offline_flag = 1;
    }
}

void K210_42_Check_Reset(void)
{
    K210_42_offline_check_time = 0;
    K210_42_offline_flag       = 0;
}
//---------------------------------------------------------------------------------------------------

