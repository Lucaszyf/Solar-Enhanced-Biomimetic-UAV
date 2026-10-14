#ifndef __BSP_USART_H
#define __BSP_USART_H

#include "Ano_FlightCtrl.h"
#include "Ano_Imu.h"
#include "Drv_icm20602.h"
#include "Ano_MagProcess.h"
#include "Drv_spl06.h"
#include "Ano_MotionCal.h"
#include "Ano_AttCtrl.h"
#include "Ano_LocCtrl.h"
#include "Ano_AltCtrl.h"
#include "Ano_MotorCtrl.h"
#include "Drv_led.h"
#include "Ano_RC.h"
#include "Drv_laser.h"
#include "Ano_OF.h"
#include "Ano_OF_DecoFusion.h"
#include "Ano_FlyCtrl.h"
#include "Ano_UWB.h"
#include "Ano_Sensor_Basic.h"
#include "Ano_Imu_Calibration.h"
//#include "Ano_DT.h"
//#include "Ano_LED.h"
#include "Ano_ProgramCtrl_User.h"
#include "Drv_OpenMV.h"
#include "Ano_FlyCtrl.h"

#define FRAME_LENGTH 14

#define HEADER_1 0xFE
#define HEADER_2 0x0A
#define TAIL 0x55

typedef struct {
    uint8_t header[2];  // 包头
    uint8_t data[11];   // 数据
    uint8_t tail;       // 包尾
} FrameTypeDef;

extern uint8_t send_data[];
extern uint8_t rec_buf;

void ch_data(void);
void laserdata(uint8_t bytedata);

#endif
