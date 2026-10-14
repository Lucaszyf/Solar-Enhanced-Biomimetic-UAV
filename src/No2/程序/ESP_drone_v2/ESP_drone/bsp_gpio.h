//所有引脚定义
//RX2 TX2 对应io16、17  用作与串口2的发送与接收  与飞控连接
#define servo_pin  27  //舵机 PWM 
#define camera_sw  21  //图传开关 数字
// #define arm_mode_sw 36 //切换爪子或吸盘

#define pump_pin  15 //气泵  数字
#define e_valve 2 //电磁阀 数字

#define air_sensor_pin  39  //气压传感器数据  模拟

#define claw_pressure_pin  36  //夹爪压力传感器   模拟
#define claw1_pin  4  //爪子电机通道1  数字
#define claw2_pin  5  //爪子电机通道2  数字
#define claw_sw_pin  26  //爪子限位开关  数字
#define claw_open_button_pin  25 //停止爪子开按键 数字

#define sp_pin1  18  //太阳能伸缩电机通道1 数字
#define sp_pin2  19  //太阳能伸缩电机通道2 数字
#define sp_left_spin_pin  13  //太阳能转动电机左通道1 数字   寻找光强的舵机
#define sp_right_spin_pin 12  //太阳能转动电机右通道1 数字  寻找光强的舵机

#define sp_light_sensor_up  34  //光敏输入上  模拟
#define sp_light_sensor_down  32  //光敏输入下  模拟


