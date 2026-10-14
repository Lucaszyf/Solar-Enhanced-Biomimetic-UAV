#ifndef MY_PID
#define MY_PID

#include "Ano_math.h"
#include "Ano_FcData.h"

/* pid控制器 参数结构体 */
typedef struct
{
    float Kp, Ki, Kd;   // 定义比例、积分、微分系数（P I D）
    float err;          // 定义偏差值
    float err_last;     // 定义上一个偏差值
    float err_integral; // 定义误差积分值
    float out;          // 输出值
} PID_DATA;

extern PID_DATA loc_xyz[4];
extern PID_DATA PID_cb[2];    // 对准色块

///////////////////////////////////////////////////////////////////

void my_PID_Init(void);

void Pid_calculation(
    float expect,         // 期望值
    float actual,         // 实际值（输入）
    PID_DATA *pid,        // 参数结构体指针
    float dT,             // 时间间隔
    float err_limit,      // 误差限幅（正值）
    float integral_limit, // 积分限幅（正值）
    float out_limit       // 输出限幅（正值）
);

#endif
