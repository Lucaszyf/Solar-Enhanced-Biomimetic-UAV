/*
    本模块提供串口接收和解析接口
    -USART1 (魔改拓空者GPS串口一)
        -my_Usart1_Init()
        -my_Usart1_IRQ()
        -my_Usart1_Send()
        -Usart1_Byte_Get()
        -Usart1_Data_Analysis()
*/

#include "my_Usart.h"
#include "Ano_OF.h"
#include "Ano_Math.h"
#include "my_Jetson_Ctrl.h"
#include "my_ProgramCtrl.h"
#include "my_protocol.h"
#include "Ano_Filter.h"

// 匿名GPS串口(USART1) --> 换成自己的串口
//====uart1
void my_Usart1_Init(u32 br_num)
{
    USART_InitTypeDef USART_InitStructure;
    USART_ClockInitTypeDef USART_ClockInitStruct;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); // 开启USART2时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    // 串口中断优先级
    NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_UART1_P;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = NVIC_UART1_S;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

    // 配置PD5作为USART2　Tx
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    // 配置PD6作为USART2　Rx
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置USART2
    // 中断被屏蔽了
    USART_InitStructure.USART_BaudRate            = br_num;                         // 波特率
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;            // 8位数据
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;               // 在帧结尾传输1个停止位
    USART_InitStructure.USART_Parity              = USART_Parity_No;                // 禁用奇偶校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 硬件流控制失能
    USART_InitStructure.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;  // 发送、接收使能
    // 配置USART2时钟
    USART_ClockInitStruct.USART_Clock   = USART_Clock_Disable;   // 时钟低电平活动
    USART_ClockInitStruct.USART_CPOL    = USART_CPOL_Low;        // SLCK引脚上时钟输出的极性->低电平
    USART_ClockInitStruct.USART_CPHA    = USART_CPHA_2Edge;      // 时钟第二个边沿进行数据捕获
    USART_ClockInitStruct.USART_LastBit = USART_LastBit_Disable; // 最后一位数据的时钟脉冲不从SCLK输出

    USART_Init(USART1, &USART_InitStructure);
    USART_ClockInit(USART1, &USART_ClockInitStruct);

    // 使能USART2接收中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    // 使能USART2
    USART_Cmd(USART1, ENABLE);
}

u8 my_TxBuffer[256];
u8 my_TxCounter = 0;
u8 my_count     = 0;

void my_Usart1_IRQ(void)
{
    u8 com_data;

    if (USART1->SR & USART_SR_ORE) // ORE中断 //数据溢出标志位
    {
        com_data = USART1->DR;
    }

    // 接收中断
    if (USART_GetITStatus(USART1, USART_IT_RXNE)) {
        USART_ClearITPendingBit(USART1, USART_IT_RXNE); // 清除中断标志

        com_data = USART1->DR;
        //Usart1_Byte_Get(com_data); # 色块追踪
				Jetson_Receive(com_data);
        // AnoDTRxOneByteUart ( com_data );
    }
    // 发送（进入移位）中断  数据发送出去，成功之后关闭发生中断
    if (USART_GetITStatus(USART1, USART_IT_TXE)) {

        USART1->DR = my_TxBuffer[my_TxCounter++]; // 写DR清除中断标志
        if (my_TxCounter == my_count) {
            USART1->CR1 &= ~USART_CR1_TXEIE; // 关闭TXE（发送中断）中断
        }

        // USART_ClearITPendingBit(USART2,USART_IT_TXE);
    }
}

void my_Usart1_Send(unsigned char *DataToSend, u8 data_num)
{
    u8 i;
    for (i = 0; i < data_num; i++) {
        my_TxBuffer[my_count++] = *(DataToSend + i);
    }
		//判断发送中断是否被禁用
    if (!(USART1->CR1 & USART_CR1_TXEIE)) {
        USART_ITConfig(USART1, USART_IT_TXE, ENABLE); // 打开发送中断
    }
}

/**********************************************************************************************************
 *函 数 名: Usart1_Byte_Get
 *功能说明: Usart1字节数据获取
 *参    数: 字节数据
 *返 回 值: 无
 *格    式：0xaa,0x29,0x05,0x41,len,xx(len个数据),0xbb ---> 长度len+6
 **********************************************************************************************************/
u8 usart1_buf[20]; // 接收数组 最大缓存20个字节
void Usart1_Byte_Get(u8 bytedata)
{
    static u8 len = 0, rec_sta;
    //u8 check_val  = 0;

    //
    usart1_buf[rec_sta] = bytedata;
    //
    if (rec_sta == 0) {
        if (bytedata == 0xaa) // 1帧 0xaa
        {
            rec_sta++;
        } else {
            rec_sta = 0;
        }
    } else if (rec_sta == 1) {
        if (bytedata == 0x29) // 2帧 0x29
        {
            rec_sta++;
        } else {
            rec_sta = 0;
        }
    } else if (rec_sta == 2) // 3帧 0x05
    {
        if (bytedata == 0x05) {
            rec_sta++;
        } else {
            rec_sta = 0;
        }
    } else if (rec_sta == 3) // 4帧 0x41/0x42
    {
        if (bytedata == 0x41) {
            rec_sta++;
        } else {
            rec_sta = 0;
        }
    } else if (rec_sta == 4) // 5帧(长度信息) len
    {
        // 长度数据
        len = bytedata;
        if (len < 20) // 长度小于20才算正确帧，否则丢掉
        {
            rec_sta++;
        } else {
            rec_sta = 0;
        }
    }else if (rec_sta == (len + 5)) // “数据”长度为len字节
    {
        if (bytedata == 0xbb) // 最后一帧是 0xbb
        {
            // 解析
            Usart1_Data_Analysis(usart1_buf, len + 6); // 数组一共len+6长度
            //
            rec_sta = 0;
        } else {
            rec_sta = 0;
        }

    }else {
        //为我所需要的数据存储服务
        rec_sta++;
    }
}
/**********************************************************************************************************
 *函 数 名: Usart1_Data_Analysis
 *功能说明: 接收数据,判断是否断线
 *参    数: 字节数据、字节长度
 *返 回 值: 无
 *格    式：0xaa,0x29,0x05,0x41,len,xx(len个数据),0xbb ---> 长度len+6
 **********************************************************************************************************/
static void Usart1_Data_Analysis(u8 *buf_data, u8 len)
{
	if (*(buf_data + 3) == 0x41)
	{
		Jetson_flag = buf_data[5];
		SK_Y = buf_data[6] * 240 / 100;//实际图像为横向：（无人机坐标轴）相当于Y方向的速度
		SK_X = buf_data[7] * 320 / 100;//实际图像为纵向：（无人机坐标轴）相当于X方向的速度
		//ANO_SendF1(Jetson_flag,SK_Y,SK_X);
		Jetson_CB_Check_Reset();       //只要收到了Jetson传来的数据，就把cb_offline_check_time置0，认为用于色块追踪在线
	}
}

//--------------------------   下面是 T265 相关   -------------------------------------
s16 DataRecieve[19]; // jetson nano 发来的数据
/***   T265数据   ****/
/*速度数据*/
s16 raw_speed[3];  // 原始数据
s16 normal_speed[3];
s16 LPF_speed[3];   // 低通数据
/*位置数据*/
s16 raw_position[3]; // 原始数据
s16 normal_position[3];
s16 LPF_position[3]; // 低通数据
/*rpy数据*/
s16 raw_rpy[3];
s16 LPF_rpy[3]; // 低通数据

s16 UAV_Positon[3]; // 无人机位置数组
s16 UAV_Speed[3];   // 无人机速度数组
s16 UAV_Rpy[3];     // 无人机角度数组
/*绿色识别*/
s8 Green_Flag = 0;
/**********************************************************************************************************
 *函 数 名: Jetson_Receive
 *功能说明: 接收数据,判断是否断线
 *参    数: 字节数据、字节长度
 *返 回 值: 无
 *格    式：0x13,0x14,数据,0x52
 **********************************************************************************************************/
//数据处理函数
void T265_Data_Handle(void); // 声明
/**
 * @brief Jetson nano数据接收函数
 * @param {u8} data
 * @return {*}
 * @note
 */
void Jetson_Receive(u8 data)
{
    static u8 my_flag  = 0;
    static u8 _datalen = 0; // 数据长度
    switch (my_flag){
        case 0:{
            if (data == 0x13){
               my_flag++;
            }else{
               _datalen = 0;
            }

        } break;
        case 1:{
            if (data == 0x14)
                my_flag = 2; // 识别T265
            else
                my_flag = 0;

        }break;
        /*==  T265识别  ==*/
        case 2:{
            if (_datalen < 19) // 增加或减少协议数据注意要改这里,还有改它该数组本身的数据长度 数值大于有效数据的长度
                DataRecieve[_datalen++] = data;
            else {
                if (data == 0x52) {
                    // t265原始数据获取
                    raw_position[0] = DataRecieve[0] | DataRecieve[1] << 8;
                    raw_position[1] = DataRecieve[2] | DataRecieve[3] << 8;
                    raw_position[2] = DataRecieve[4] | DataRecieve[5] << 8;
                    raw_speed[0]    = DataRecieve[6] | DataRecieve[7] << 8;
                    raw_speed[1]    = DataRecieve[8] | DataRecieve[9] << 8;
                    raw_speed[2]    = DataRecieve[10] | DataRecieve[11] << 8;
                    raw_rpy[0]      = DataRecieve[12] | DataRecieve[13] << 8;
                    raw_rpy[1]      = DataRecieve[14] | DataRecieve[15] << 8;
                    raw_rpy[2]      = DataRecieve[16] | DataRecieve[17] << 8;
										Green_Flag      = DataRecieve[18];
                    T265_Data_Handle(); // 数据处理

                    Jetson_CB_Check_Reset(); // 在线复位
                } else {
                    my_flag = 0;
                }

                my_flag  = 0;
                _datalen = 0;
            }
        }break;
        default:
            break;
    }
}

s16 speed_change_threshold = 50;    // 速度变化阈值
float LPF_speed_param      = 0.35f; // 速度低通滤波系数
s16 speed_limit            = 150;   // t265速度限幅

s16 position_change_threshold = 50;   // 位置变化阈值
float LPF_position_param      = 0.3f; // 位置低通滤波系数
void T265_Data_Handle()
{
    for (u8 i = 0; i < 3; i++) {
        // 速度数据处理
        raw_speed[i]    = LIMIT(raw_speed[i], -speed_limit, speed_limit); // 速度限幅
        normal_speed[i] = raw_speed[i];
        LPF_speed[i] += LPF_speed_param * (normal_speed[i] - LPF_speed[i]); // 速度 低通滤波
        // if (ABS(LPF_speed[i]) <= 2) { //限制不过调
        //     LPF_speed[i] = 0;
        // } else {
        //     //
        // }

        // 位置数据处理
        normal_position[i] = raw_position[i];
        // LPF_position[i] += LPF_position_param * (normal_position[i] - LPF_position[i]); // 位置 低通滤波
        LPF_position[i] = normal_position[i]; // 位置 不滤波

        // rpy数据处理
        //S_LPF_1(0.30f, raw_rpy[i], LPF_rpy[i]); // yaw 卡尔曼滤波
			  LPF_rpy[i] = raw_rpy[i];
    }

    /*坐标转换+误差补偿 获取最终数据*/
    UAV_Positon[X] = LPF_position[X];
    UAV_Positon[Y] = LPF_position[Y];
    UAV_Positon[Z] = LPF_position[Z];
    UAV_Rpy[ROL]   = LPF_rpy[ROL];
    UAV_Rpy[PIT]   = LPF_rpy[PIT];
    UAV_Rpy[YAW]   = LPF_rpy[YAW];
    UAV_Speed[X]   = LPF_speed[X];
    UAV_Speed[Y]   = LPF_speed[Y];
    UAV_Speed[Z]   = LPF_speed[Z];
		
		ANO_SendF2(UAV_Positon[X],UAV_Positon[Y],UAV_Positon[Z],UAV_Speed[X],UAV_Speed[Y],UAV_Speed[Z],UAV_Rpy[ROL],UAV_Rpy[PIT],UAV_Rpy[YAW],Green_Flag);
}
