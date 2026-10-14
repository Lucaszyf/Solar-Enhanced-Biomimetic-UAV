#include "my_PidCtrl.h"

PID_DATA loc_xyz[4];   // 基本PID控制器
PID_DATA PID_cb[2];    // 对准色块

#define Yaw 3

void my_PID_Init(void)
{
		// 初始化xy-PID控制器:XY速度
    loc_xyz[X].Kp           = 0.45;
    loc_xyz[X].Ki           = 0.015;
    loc_xyz[X].Kd           = 0.00;
    loc_xyz[X].err          = 0.00;
    loc_xyz[X].err_last     = 0.00;
    loc_xyz[X].err_integral = 0.00;
    loc_xyz[Y]              = loc_xyz[X]; // xy参数相同

		// 初始化z-PID控制器:高度
    loc_xyz[Z].Kp           = 0.40;
    loc_xyz[Z].Ki           = 0.001;
    loc_xyz[Z].Kd           = 0;
    loc_xyz[Z].err          = 0.00;
    loc_xyz[Z].err_last     = 0.00;
    loc_xyz[Z].err_integral = 0.00;

    // ————————————————————————————————————————————

    // 对准色块
    PID_cb[X].Kp           = 0.15;
    PID_cb[X].Ki           = 0.001;
    PID_cb[X].Kd           = 0.00;
    PID_cb[X].err          = 0.00;
    PID_cb[X].err_last     = 0.00;
		
    PID_cb[X].err_integral = 0.00;
    PID_cb[Y]              = PID_cb[X]; //xy参数相同

}

/**
 * @brief  根据实际,获得期望  PID算法实现
 * @param  pid:参数结构体指针
 * @param  actual_val:实际值
 * @param  integral_limit:积分限幅（正值） 0表示不限幅
 * @param  out_limit:输出限幅（正值） 0表示不限幅
 * @note   通用 PID 控制器计算函数
 * @retval 通过PID计算后的输出
 */
void Pid_calculation(
    float expect,         // 期望值
    float actual,         // 实际值（输入）
    PID_DATA *pid,        // 参数结构体指针
    float dT,             // 时间间隔
    float err_limit,      // 误差限幅（正值）
    float integral_limit, // 积分限幅（正值）
    float out_limit       // 输出限幅（正值）
)
{
    static float hz;

    /*计算频率*/
    hz = safe_div(1.0, dT, 0);

    /*计算目标值与实际值的误差*/
    pid->err = expect - actual;

    /*误差积分*/
    if (err_limit != 0) {
        pid->err_integral += pid->Ki * LIMIT(pid->err, -err_limit, err_limit) * dT;
    }

    /*误差积分限幅*/
    if (integral_limit != 0) {
        pid->err_integral = LIMIT(pid->err_integral, -integral_limit, integral_limit);
    }

    /*PID计算           P                    I                            D*/
    pid->out = pid->Kp * pid->err + pid->err_integral + pid->Kd * (pid->err - pid->err_last);

    /*误差传递*/
    pid->err_last = pid->err;

    /*输出值限幅*/
    if (out_limit != 0) {
        pid->out = LIMIT(pid->out, -out_limit, out_limit);
    }
}
