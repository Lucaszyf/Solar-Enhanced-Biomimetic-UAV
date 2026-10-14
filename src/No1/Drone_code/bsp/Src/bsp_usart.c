#include "bsp_usart.h"

int data_ch_i;
s16 rc_ch_data[6];
uint8_t send_data[8];

// 串口句柄
uint8_t rxBuffer[FRAME_LENGTH];
volatile uint8_t rxIndex = 0;
FrameTypeDef frame;

uint8_t rec_buf;

void ch_data(void) {
    //将遥控器发过来的原始数字使用rc_ch_data进行接收；原始有10个通道值，我们只需要其中6个保存到rc_ch_data数组里面
    for(data_ch_i=4; data_ch_i<10; data_ch_i++)rc_ch_data[data_ch_i-4]=rc_in.rc_ch.st_data.ch_[data_ch_i];
    //rc_ch_data[]中下标0-5里面存储的是SWA SWB SWC SWD VRA VRB原始数据 而原始数据比较大，通过串口2发送数据只能发送uint8_t类型
    //只能发送0-255之间的数值，需要做转换在发送出去
    if(rc_ch_data[0]<=1000)send_data[0]=0;
    else if(rc_ch_data[0]>=1990)send_data[0]=1;

    if(rc_ch_data[1]<=1000)send_data[1]=0;
    else if(rc_ch_data[1]>=1490&&rc_ch_data[1]<=1510)send_data[1]=1;
    else if(rc_ch_data[1]>=1990)send_data[1]=2;

    if(rc_ch_data[2]<=1000)send_data[2]=0;
    else if(rc_ch_data[2]>=1490&&rc_ch_data[2]<=1510)send_data[2]=1;
    else if(rc_ch_data[2]>=1990)send_data[2]=2;

    if(rc_ch_data[3]<=1000)send_data[3]=0;
    else if(rc_ch_data[3]>=1990)send_data[3]=1;

    send_data[4]=rc_ch_data[4]/10;
    send_data[5]=rc_ch_data[5]/10;

    send_data[6]=frame.data[6];
    send_data[7]=frame.data[7];

    Usart2_Send(send_data,sizeof(send_data)/sizeof(send_data[0]));
}


void laserdata(uint8_t bytedata) {
    // 存储接收到的字节到缓冲区
    if (rxIndex < FRAME_LENGTH) {
        rxBuffer[rxIndex++] = bytedata;
    }
    // 检查是否接收到完整的数据帧
    if (rxIndex == FRAME_LENGTH) {
        // 校验包头和包尾
        if (rxBuffer[0] == HEADER_1 && rxBuffer[1] == HEADER_2 && rxBuffer[13] == TAIL) {
            // 复制数据到frame结构体
            frame.header[0] = rxBuffer[0];
            frame.header[1] = rxBuffer[1];
            for (int i = 0; i < 11; i++) {
                frame.data[i] = rxBuffer[2 + i];
            }
            frame.tail = rxBuffer[13];
            // 处理完整的数据帧
        }
        // 重置索引，准备接收下一个数据帧
        rxIndex = 0;
    }
}

void rec_data(uint8_t rec_buf) {
    rec_buf=rec_buf;
}

