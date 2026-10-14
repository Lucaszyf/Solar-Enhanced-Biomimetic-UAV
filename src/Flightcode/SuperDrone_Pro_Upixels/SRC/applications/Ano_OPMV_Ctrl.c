//默认引用
#include "Ano_OPMV_LineTracking_Ctrl.h"
#include "Ano_OPMV_CBTracking_Ctrl.h"
#include "Drv_OpenMV.h"
#include "Ano_OPMV_Ctrl.h"
#include "Ano_Math.h"
#include "Ano_Filter.h"
#include "Ano_ProgramCtrl_User.h"
//
//数据接口定义：
//=========mapping===============
//需要引用的文件：
#include "Ano_FlightCtrl.h"

//需要调用引用的外部变量：
#define RELATIVE_HEIGHT_CM           (jsdata.valid_of_alt_cm)  //相对高度


//需要操作赋值的外部变量：


//===============================
//全局变量：
_opmv_ct_sta_st opmv_ct_sta;
//参数设定：


/**********************************************************************************************************
*函 数 名: ANO_OPMV_Ctrl_Task
*功能说明: OPMV控制任务
*参    数: 周期时间(ms)
*返 回 值: 无
**********************************************************************************************************/
void ANO_OPMV_Ctrl_Task(u8 dT_ms)
{
	//判断高度标记的条件
	if( RELATIVE_HEIGHT_CM >40)
	{
		//起飞大于40厘米，标记置位
		opmv_ct_sta.height_flag =1;
	}
	//复位高度标记的条件
	if(flag.unlock_sta ==0)
	{
		//上锁后标记复位
		opmv_ct_sta.height_flag =0;
	}
	
	//判断开启条件（可自行设计）
	if
	(
		switchs.of_flow_on                   //光流有效
		&& switchs.opmv_on                   //mv有效
		&& opmv_ct_sta.height_flag !=0       //高度标记不为0
		&& flag.flight_mode2 == 1            //AUX2通道模式值为1，参考FlightCtrl.c内程序
	)
	{
		opmv_ct_sta.en = 1;
	}
	else
	{
		opmv_ct_sta.en = 0;
	}
	
	
	//不同模式执行不同功能
	if(opmv.mode_sta==1)
	{
		//
		opmv_ct_sta.reset_flag = 0;
		//
		ANO_CBTracking_Ctrl(&dT_ms,opmv_ct_sta.en);
		//调用用户程控函数赋值控制量
		Program_Ctrl_User_Set_HXYcmps(ano_opmv_cbt_ctrl.exp_velocity_h_cmps[0],ano_opmv_cbt_ctrl.exp_velocity_h_cmps[1]);
	}
	else if(opmv.mode_sta == 2)
	{
		//
		opmv_ct_sta.reset_flag = 0;
		//
		ANO_LTracking_Ctrl(&dT_ms,opmv_ct_sta.en);
		//调用用户程控函数赋值控制量
		Program_Ctrl_User_Set_HXYcmps(ano_opmv_lt_ctrl.exp_velocity_h_cmps[0],ano_opmv_lt_ctrl.exp_velocity_h_cmps[1]);		
		Program_Ctrl_User_Set_YAWdps(ano_opmv_lt_ctrl.exp_yaw_pal_dps);
	}
	else
	{
		//reset
		if(opmv_ct_sta.reset_flag==0)
		{
			opmv_ct_sta.reset_flag = 1;
			Program_Ctrl_User_Set_HXYcmps(0,0);		
			Program_Ctrl_User_Set_YAWdps(0);			
		}
	}
	
}
