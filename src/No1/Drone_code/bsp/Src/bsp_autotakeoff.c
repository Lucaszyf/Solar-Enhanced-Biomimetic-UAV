#include "bsp_autotakeoff.h"
#include "bsp_usart.h"

u8 user_cmd_data[7];

void MY_Take_Off_Land()
{
    user_cmd_data[0] = 0x00; // 保留字节，可忽略
    user_cmd_data[1] = 0x00; // 保留字节，可忽略
    user_cmd_data[2] = 0x00; // 指令字节，0x01表示起飞
    user_cmd_data[3] = 0x00; // 目标高度高字节
    user_cmd_data[4] = 0x00; // 目标高度低字节
    user_cmd_data[5] = 0x00; // 速度高字节
    user_cmd_data[6] = 0x00; // 速度低字节

    if(send_data[2] == 2 && rec_buf==1)  //气泵
    {
        //自动起飞
        user_cmd_data[2] = 0x01;
    }
    else if(send_data[3] == 1 && rec_buf==0) { //爪
        //自动起飞
        user_cmd_data[2] = 0x01;
    }
//	else if(send_data[2] == 1)
//	{
//		//自动降落
//		user_cmd_data[2] = 0x02;
//	}
    else
    {
        user_cmd_data[2] = 0x00;
    }

    FlyCtrlDataAnl(user_cmd_data);

}
