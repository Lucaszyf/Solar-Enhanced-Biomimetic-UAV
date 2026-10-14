//所有引脚定义
//RX2 TX2 对应io16、17  用作与串口2的发送与接收  与飞控连接
#define SERVO_PIN  32  //舵机 PWM 
#define camera_sw  33  //图传开关 数字
#define arm_mode_sw 36 //切换爪子或吸盘


#define pump_pin  25 //气泵  数字
#define e_valve 2 //电磁阀 数字

// #define laser_sensor_sda  0  //激光传感器数据  I2C
// #define laser_sensor_scl  4  //激光传感器时钟  I2C


#define air_sensor_pin  13  //压力传感器数据  模拟

#define claw_pressure_pin  35  //夹爪压力传感器   模拟
#define claw1_pin  18  //爪子电机通道1  数字
#define claw2_pin  5  //爪子电机通道2  数字
#define claw_sw_pin  26  //爪子限位开关  数字
#define claw_open_button_pin  39 //停止爪子开按键 数字

#define sp_pin1  23  //太阳能伸缩电机通道1 数字
#define sp_pin2  19  //太阳能伸缩电机通道2 数字
#define sp_left_spin_pin  22  //太阳能转动电机左通道1 数字
#define sp_right_spin_pin  14  //太阳能转动电机右通道1 数字
// #define sp_button_pin 15  //停止太阳能转动的按键

#define light_sensor  34  //光照强度输入  模拟


