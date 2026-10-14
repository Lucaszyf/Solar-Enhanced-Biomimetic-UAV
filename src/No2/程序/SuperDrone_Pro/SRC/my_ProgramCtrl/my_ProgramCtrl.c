/*
    相关说明：
		
    · User_task(u8 ms)为主任务函数。

    · 地图位置坐标系为飞控坐标
                (x)
                 ^
                 |
                 |
    (y) <————————|



    · 串口1(GPS串口) --> Jetson nano

*/
#include "my_ProgramCtrl.h"
#include "my_Protocol.h"
#include "Ano_FlightCtrl.h"
#include "Ano_ProgramCtrl_User.h"
#include "my_Jetson_Ctrl.h"
#include "my_PidCtrl.h"
#include "Ano_OF.h"
#include "my_Usart.h"
#include "Drv_OpenMV.h"


#define ALT_OF (ano_of.of_alt_cm) // 光流高度数据

// xyz速度
float UAV_speed_x;
float UAV_speed_y;
float UAV_speed_z;
s16 UAV_speed_yaw;
// 色块位置信息
s16 SK_X,SK_Y;
s8 Jetson_flag = 0;
//路径安排											    	// 0       1         2          3          4           5          6           7           8   
const s16 farm_coordinate[19][2] = {{0, 0}, {80, 0}, {80, -80}, {80, -160}, {160, 0}, {160, -80}, {160, -160}, {240, 0}, {240, -80},
																	//	 9           10           11          12          13         14          15          16
																	 {240, -160}, {320, 0}, {320, -80}, {320, -160}, {400, 0}, {400, -80}, {400, -160}, {480, 0}, 
																	 // 17          18          19        20           21
																	 {480, -80}, {480, -160}};//, {560, 0}, {560, -80}, {560, -160}};
//播撒顺序
//const s16 farm_order[21]= {1,4,7,10,13,16,19,20,17,14,11,8,5,2,3,6,9,12,15,18,21};
const s16 farm_order[18]= {1,4,7,10,13,16,17,14,11,8,5,2,3,6,9,12,15,18};
//目标高度
s8 target_height = 80;
// 发送给STM32的控制信号	
u8 ESP8266_Control[5] = {0xaa, 0x29, 0x05, 0x01, 0xbb};

#define LIGHT_SHINE()                \
{                                    \
		ESP8266_Control[3] = 0x01;       \
		Usart2_Send(ESP8266_Control, 5); \
} // 激光笔闪烁

/*测试任务*/
u16 Case1_time;
void Test_task(u8 ms)
{
	static u8 mission_flag = 0;
	if (rc_in.rc_ch.st_data.ch_[ch_7_aux3] >= 1950)
	{
		mission_flag = 1;
	   //ANO_SendF1(500,200,300,400,500);
	}else
	{
		mission_flag = 0;
	}
	
	if(mission_flag)
	{
		one_key_take_off();
		if(rc_in.rc_ch.st_data.ch_[ch_8_aux4] >= 1950)
		{
			Case1_time += ms;
			if(Case1_time < 2000){Program_Ctrl_User_Set_HXYcmps(40,0);}
			else if(Case1_time > 2000 && Case1_time < 4000)
			{
				Program_Ctrl_User_Set_HXYcmps(0,0);
				Program_Ctrl_User_Set_Zcmps(20);
			}else
			{
				Program_Ctrl_User_Set_HXYcmps(0,0);
				Program_Ctrl_User_Set_Zcmps(0);
			}
		}
	}else
	{
		Case1_time = 0;
		one_key_land();
	}

}
//////////  测试任务  /////////////
//s8 process_text = 0;
//s16 time_fly_start_text = 0;
//void Test_show_task(u8 ms)
//{
//	static s8 mission_flag = 0;
//	if (rc_in.rc_ch.st_data.ch_[ch_7_aux3] >= 1950)
//	{
//		mission_flag = 1;
//	   //ANO_SendF1(500,200,300,400,500);
//	}else
//	{
//		mission_flag = 0;
//	}
//	UAV_speed_x = UAV_speed_y = UAV_speed_z = UAV_speed_yaw = 0;
//	if(mission_flag)
//	{
//		switch(process_text)
//		{
//			case 0:{
//				time_fly_start_text += ms;
//				if(time_fly_start_text <= 3000)
//				{
//					if(time_fly_start_text == 1000)
//					{
//					  flag.unlock_cmd = 1; // 解锁
//					}
//					if(time_fly_start_text >= 1500)
//					{
//						one_key_take_off();
//					}
//				}else
//				{
//					time_fly_start_text = 0;
//					process_text=1;
//				}
//			}break;
//			case 1:{
//				Pid_calculation(
//					target_height, // 期望值
//					ALT_OF,        // 实际值（输入）
//					&loc_xyz[Z],   // 参数结构体指针
//					20 * 1e-3f,    // 时间间隔
//					10,            // 误差限幅（正值）
//					10,            // 积分限幅（正值）
//					100);          // 输出限幅（正值）
//				UAV_speed_z = loc_xyz[Z].out;
//				if(my_cb_Jetson_status.offline == 0)
//				//if(1)
//				{
//					if(Jetson_flag)
//					//if(1)
//					{
//						Pid_calculation(
//						160,              // 期望值
//						SK_X, // 实际值（输入）(Jetson nano)读取
//						&PID_cb[X],    // 参数结构体指针
//						20 * 1e-3f,     // 时间间隔
//						10,              // 误差限幅（正值）
//						10,              // 积分限幅（正值）
//						30);              // 输出限幅（正值）
//						Pid_calculation(
//						120,              // 期望值
//						SK_Y, // 实际值（输入）(Jetson nano)读取
//						&PID_cb[Y],    // 参数结构体指针
//						20 * 1e-3f,     // 时间间隔
//						10,              // 误差限幅（正值）
//						10,              // 积分限幅（正值）
//						30);            // 输出限幅（正值）
//						UAV_speed_x = PID_cb[X].out;
//						UAV_speed_y = PID_cb[Y].out;
//					}else
//					{
//						UAV_speed_x = 0;
//						UAV_speed_y = 0;
//					}
//				}else
//				{
//						UAV_speed_x = 0;
//						UAV_speed_y = 0;
//				}
//			}break;
//		}
//	}else
//	{
//		process_text = 0;
//		one_key_land();
//	}
//}
//    哪个任务         消抖确认       播撒点变量
s8 process_text=0,center_ok=0,test_sub_process=0;
s16 time_fly_start_text = 0;
void Test_show_task(u8 ms)
{
	//                          播撒相关变量
	static s8 mission_flag = 0,shine_flag = 0;
	
	if (rc_in.rc_ch.st_data.ch_[ch_7_aux3] >= 1950)
	{
		mission_flag = 1;
	}else
	{
		mission_flag = 0;
	}
	if(mission_flag && my_cb_Jetson_status.offline == 0)
	{
		UAV_speed_x = UAV_speed_y = UAV_speed_z = UAV_speed_yaw = 0;
		switch(process_text)
		{
			case 0:{
				time_fly_start_text += ms;
				if(time_fly_start_text <= 3000)
				{
					if(time_fly_start_text == 1000)
					{
						flag.unlock_cmd = 1; // 解锁
					}
					if(time_fly_start_text >= 1500)
					{
						one_key_take_off();
					}
				}else
				{
					time_fly_start_text = 0;
					process_text++;
				}
			}break;
			case 1:{
				if(ABS(target_height - ALT_OF) > 5)
				{
					Pid_calculation(
						target_height, // 期望值
						ALT_OF,        // 实际值（输入）
						&loc_xyz[Z],   // 参数结构体指针
						20 * 1e-3f,    // 时间间隔
						10,            // 误差限幅（正值）
						10,            // 积分限幅（正值）
						100);          // 输出限幅（正值）
					Pid_calculation(
						0,              // 期望值
						UAV_Positon[X], // 实际值（输入）(Jetson nano)读取
						&loc_xyz[X],    // 参数结构体指针
						20 * 1e-3f,     // 时间间隔
						5,              // 误差限幅（正值）
						5,              // 积分限幅（正值）
						70);              // 输出限幅（正值）
					Pid_calculation(
						0,              // 期望值
						UAV_Positon[Y], // 实际值（输入）(Jetson nano)读取
						&loc_xyz[Y],    // 参数结构体指针
						20 * 1e-3f,     // 时间间隔
						5,              // 误差限幅（正值）
						5,              // 积分限幅（正值）
						70);            // 输出限幅（正值）
					UAV_speed_x = loc_xyz[X].out;
					UAV_speed_y = loc_xyz[Y].out;
					UAV_speed_z = loc_xyz[Z].out;
					center_ok=0;
				}else
				{
					center_ok++;
					if(center_ok == 10){
						center_ok=0;
						process_text++;
						loc_xyz[X].err_integral = loc_xyz[Y].err_integral = loc_xyz[Z].err_integral = 0; // 误差累积清0
					}
				}
			}break;
			case 2:{
				Pid_calculation(
					target_height,  // 期望值
					ALT_OF,      // 实际值（输入）
					&loc_xyz[Z], // 参数结构体指针
					20 * 1e-3f,  // 时间间隔
					10,          // 误差限幅（正值）
					10,          // 积分限幅（正值）
					100          // 输出限幅（正值）
					);
				UAV_speed_z = loc_xyz[Z].out; // 不掉高度
				//------- 播撒 --------
				if ((ABS(UAV_Positon[X] - farm_coordinate[farm_order[test_sub_process]][0])) > 5 || (ABS(UAV_Positon[Y] - (farm_coordinate[farm_order[test_sub_process]][1]))) > 5) {
				Pid_calculation(
					farm_coordinate[farm_order[test_sub_process]][0], // 期望值
					UAV_Positon[X],                              // 实际值（输入）
					&loc_xyz[X],                                 // 参数结构体指针
					20 * 1e-3f,                                  // 时间间隔
					5,                                           // 误差限幅（正值）
					5,                                           // 积分限幅（正值）
					30                                           // 输出限幅（正值）
					);
				Pid_calculation(
					farm_coordinate[farm_order[test_sub_process]][1], // 期望值  运行的时候发现都会往左偏！！！
					UAV_Positon[Y],                                   // 实际值（输入）
					&loc_xyz[Y],                                      // 参数结构体指针
					20 * 1e-3f,                                       // 时间间隔
					5,                                                // 误差限幅（正值）
					5,                                                // 积分限幅（正值）
					30                                                // 输出限幅（正值）
					);
				UAV_speed_x = loc_xyz[X].out;
				UAV_speed_y = loc_xyz[Y].out;

				center_ok = 0;
				} else {
						center_ok++;
						if (center_ok == 10) {
								center_ok = 0;
								process_text += 1;
						}
				}
			}break;
			case 3:{
				Pid_calculation(
					target_height,  // 期望值
					ALT_OF,      // 实际值（输入）
					&loc_xyz[Z], // 参数结构体指针
					20 * 1e-3f,  // 时间间隔
					10,          // 误差限幅（正值）
					10,          // 积分限幅（正值）
					100          // 输出限幅（正值）
				);
				UAV_speed_z = loc_xyz[Z].out; // 不掉高度

				Pid_calculation(
					farm_coordinate[farm_order[test_sub_process]][0], // 期望值
					UAV_Positon[X],                              // 实际值（输入）
					&loc_xyz[X],                                 // 参数结构体指针
					20 * 1e-3f,                                  // 时间间隔
					10,                                          // 误差限幅（正值）
					10,                                          // 积分限幅（正值）
					30                                           // 输出限幅（正值）
				);
				Pid_calculation(
					farm_coordinate[farm_order[test_sub_process]][1], // 期望值
					UAV_Positon[Y],                                   // 实际值（输入）
					&loc_xyz[Y],                                      // 参数结构体指针
					20 * 1e-3f,                                       // 时间间隔
					10,                                               // 误差限幅（正值）
					10,                                               // 积分限幅（正值）
					30                                                // 输出限幅（正值）
				);
				UAV_speed_x = loc_xyz[X].out;
				UAV_speed_y = loc_xyz[Y].out; // 闭环
			 if(shine_flag == 0 && Green_Flag == 0){
					LIGHT_SHINE();// 激光笔闪烁
				  LIGHT_SHINE();
					shine_flag = 1;
			 }
	
				time_fly_start_text += ms;

				if (time_fly_start_text >= 600) // 计时
				{
					time_fly_start_text = 0;
					shine_flag          = 0;
					process_text        = 2; // 继续定位到下一点

					//--- 到达播撒方块的操作 ---
					test_sub_process += 1;      // 准备去下一个点
					if (test_sub_process == 18) // 21个点播撒完成
					{
							process_text = 4; // 回起点
					}
					//------------------------

					loc_xyz[X].err_integral = loc_xyz[Y].err_integral = loc_xyz[Z].err_integral = 0; // 误差累积清0
				}
			}break;
			case 4:{
				Pid_calculation(
					target_height,  // 期望值
					ALT_OF,      // 实际值（输入）
					&loc_xyz[Z], // 参数结构体指针
					20 * 1e-3f,  // 时间间隔
					10,          // 误差限幅（正值）
					10,          // 积分限幅（正值）
					70           // 输出限幅（正值）
					);
				UAV_speed_z = loc_xyz[Z].out; // 不掉高度
				if ((ABS(UAV_Positon[X] - 0)) > 3 || (ABS(UAV_Positon[Y] - (0))) > 3) {
					Pid_calculation(
						0,              // 期望值
						UAV_Positon[X], // 实际值（输入）
						&loc_xyz[X],    // 参数结构体指针
						20 * 1e-3f,     // 时间间隔
						5,              // 误差限幅（正值）
						5,              // 积分限幅（正值）
						30              // 输出限幅（正值）
						);
					Pid_calculation(
						0,              // 期望值
						UAV_Positon[Y], // 实际值（输入）
						&loc_xyz[Y],    // 参数结构体指针
						20 * 1e-3f,     // 时间间隔
						5,              // 误差限幅（正值）
						5,              // 积分限幅（正值）
						30              // 输出限幅（正值）
						);
					UAV_speed_x = loc_xyz[X].out;
					UAV_speed_y = loc_xyz[Y].out;

					center_ok = 0;
				} else {
						center_ok++;
						if (center_ok == 10) {
								center_ok = 0;
								process_text += 1;
								loc_xyz[X].err_integral = loc_xyz[Y].err_integral = loc_xyz[Z].err_integral = 0; // 误差累积清0
						}
				}
			}break;
			case 5:{
				Pid_calculation(
					0,              // 期望值
					UAV_Positon[X], // 实际值（输入）
					&loc_xyz[X],    // 参数结构体指针
					20 * 1e-3f,     // 时间间隔
					5,              // 误差限幅（正值）
					5,              // 积分限幅（正值）
					30              // 输出限幅（正值）
					);
				Pid_calculation(
					0,              // 期望值
					UAV_Positon[Y], // 实际值（输入）
					&loc_xyz[Y],    // 参数结构体指针
					20 * 1e-3f,     // 时间间隔
					5,              // 误差限幅（正值）
					5,              // 积分限幅（正值）
					30              // 输出限幅（正值）
					);
				UAV_speed_x = loc_xyz[X].out; // 对准起点
				UAV_speed_y = loc_xyz[Y].out;

				if (ABS(ALT_OF - 10) >= 5) {
					Pid_calculation(
							10,          // 期望值
							ALT_OF,      // 实际值（输入）
							&loc_xyz[Z], // 参数结构体指针
							20 * 1e-3f,  // 时间间隔
							10,          // 误差限幅（正值）
							10,          // 积分限幅（正值）
							30           // 输出限幅（正值）
					);
					UAV_speed_z = loc_xyz[Z].out; // 下降至指定高度
				} else {
					process_text = 100;
				}
			}break;
			case 100:{
				one_key_land();
			}break;
			default:break;
		}
	}else
	{
		UAV_speed_x = UAV_speed_y = UAV_speed_z = UAV_speed_yaw = 0;
		process_text = 0;
		center_ok = 0;
		test_sub_process = 0;
		//one_key_land();
	}
}
////////// 用户主任务 /////////////
s8 process = 0;
s16 time_fly_start = 0;
void User_task(u8 ms)
{
	static s8 mission_flag = 0;
	if (rc_in.rc_ch.st_data.ch_[ch_7_aux3] >= 1950)
	{
		mission_flag = 1;
	}else
	{
		mission_flag = 0;
	}
	
	if(mission_flag && my_cb_Jetson_status.offline == 0)
	{
		UAV_speed_x = UAV_speed_y = UAV_speed_z = 0;
		switch(process)
		{
			case 0:{
				time_fly_start += ms;
				if(time_fly_start <= 3000)
				{
					if(time_fly_start == 500)
					{
					  flag.unlock_cmd = 1; // 解锁
					}
					if(time_fly_start >= 1000)
					{
						one_key_take_off();
					}
				}else
				{
					time_fly_start = 0;
					process++;
				}
			}break;
			case 1:{
				Pid_calculation(
						target_height, // 期望值
						ALT_OF,        // 实际值（输入）
						&loc_xyz[Z],   // 参数结构体指针
						20 * 1e-3f,    // 时间间隔
						10,            // 误差限幅（正值）
						10,            // 积分限幅（正值）
						20);          // 输出限幅（正值）
				Pid_calculation(
						0,              // 期望值
						-ty / ALT_OF, // 实际值（输入）(Jetson nano)读取
						&loc_xyz[X],    // 参数结构体指针
						20 * 1e-3f,     // 时间间隔
						30,              // 误差限幅（正值）
						10,              // 积分限幅（正值）
						30);              // 输出限幅（正值）
				Pid_calculation(
						0,              // 期望值
						tx / ALT_OF, // 实际值（输入）(Jetson nano)读取
						&loc_xyz[Y],    // 参数结构体指针
						20 * 1e-3f,     // 时间间隔
						30,              // 误差限幅（正值）
						10,              // 积分限幅（正值）
						30);            // 输出限幅（正值）
				UAV_speed_x = loc_xyz[X].out;
				UAV_speed_y = loc_xyz[Y].out;
				UAV_speed_z = loc_xyz[Z].out;
				if (ABS(tx) <= 70 && ABS(ty <= 70)) {
					process = 2;
				}
				//Apriltag_yaw_angle();
			}break;
			case 2:
			{
				Pid_calculation(
					20, // 期望值
					ALT_OF,        // 实际值（输入）
					&loc_xyz[Z],   // 参数结构体指针
					20 * 1e-3f,    // 时间间隔
					10,            // 误差限幅（正值）
					10,            // 积分限幅（正值）
					20);          // 输出限幅（正值）
					UAV_speed_z = loc_xyz[Z].out;
				
				if (ABS(ALT_OF - 20) <= 2) {
					process = 100;
				}
			}break;
			case 100:{
				//one_key_land();
				flag.unlock_cmd = 0;
			}break;
			default:break;
		}
	}else
	{
		process = 0;
		one_key_land();
	}
}
////////// 用户主任务 /////////////
