#ifndef __BSP_AUTOTAKEOFF_H
#define __BSP_AUTOTAKEOFF_H

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

void MY_Take_Off_Land(void);

#endif
