#include "Ano_Parameter.h"
#include "data_process.h"
#include "Ano_Imu_Data.h"
#include "Drv_time.h"
#include "stdio.h"
#include "Ano_OF.h"

//优象原始数据
u8 flow_buf[20];
flow_data_analysis flow_data;
void Flow_data_get(u8 bytedata)
{	
	static u8 rec_sta=0;
	
	flow_buf[rec_sta] = bytedata;
	//
	if(rec_sta==0)
	{
		if(bytedata==0xFE)
		{
			rec_sta++;
		}
		else
		{
			rec_sta=0;
		}
	}
	else if(rec_sta==1)
	{
		if(bytedata==0x0A)
		{
			rec_sta++;
		}	
		else
		{
			rec_sta=0;
		}		
	}
	else if(rec_sta==13)
	{
		if(bytedata==0x55)
		{
			//解析成功
			Flow_Data_Analysis(flow_buf);
			//
			rec_sta=0;
		}
		else
		{
			rec_sta=0;
		}		
	}else
	{
		rec_sta++;
	}
}
u32 Y_time=0;
s32 Y_num_min[2]={0,0};
static void Flow_Data_Analysis(u8 *buf_data)
{
			flow_data.valid=*(buf_data+10); //光流数据可用0xF5
			flow_data.belief=*(buf_data+11);//激光数据可用0x64
			if(flow_data.valid == 0xF5 && flow_data.belief == 0x64)
			{
				flow_data.pos_x=(s16)(*(buf_data+2)|*(buf_data+3)<<8);
				flow_data.pos_y=(s16)(*(buf_data+4)|*(buf_data+5)<<8);
				flow_data.dT_ms=(s16)(*(buf_data+6)|*(buf_data+7)<<8);
				flow_data.dis_h=(s16)(*(buf_data+8)|*(buf_data+9)<<8);
				flow_data.speed_x = (short)(flow_data.pos_x*flow_data.dis_h*0.5/2000.0);//(short)((flow_data.pos_x/10000.0*flow_data.dis_h*0.5)*50);
				flow_data.speed_y = (short)(flow_data.pos_y*flow_data.dis_h*0.5/2000.0);//(short)((flow_data.pos_y/10000.0*flow_data.dis_h*0.5)*50);

				ano_of.of2_dx_fix = (short)(flow_data.speed_x);//修正后的X、Y轴的移动速度，适用于积分计算（地面速度,单位：厘米/秒）
				ano_of.of2_dy_fix = (short)(flow_data.speed_y);
				ano_of.of_alt_cm  = flow_data.dis_h/10;
			}
			else
			{
				flow_data.speed_x = 0;
				flow_data.speed_y = 0;
			}
}

//MSP_h
flow_data_analysis_msp flow_data_msp;

u8 flow_buf_msp_h[20];
void Flow_data_msp_h(u8 bytedata)
{	
	static u8 rec_sta=0;
	
	flow_buf_msp_h[rec_sta] = bytedata;
	//
	if(rec_sta==0)
	{
		if(bytedata==0x24)
		{
			rec_sta++;
		}
		else
		{
			rec_sta=0;
		}
	}
	else if(rec_sta==1)
	{
		if(bytedata==0x58)
		{
			rec_sta++;
		}	
		else
		{
			rec_sta=0;
		}		
	}
	else if(rec_sta==2)
	{
		if(bytedata==0x3c)
		{
			rec_sta++;
		}	
		else
		{
			rec_sta=0;
		}		
	}
	else if(rec_sta==6)
	{
		if(bytedata==0x05)
		{
			rec_sta++;
		}	
		else
		{
			rec_sta=0;
		}		
	}
	else if(rec_sta==13)//高度信息
	{
		rec_sta++;
		if(flow_buf_msp_h[6] == 0x05)
		{
			//解析成功
			Flow_Data_Analysis_msp_h(flow_buf_msp_h);
			//
			rec_sta=0;
		}else
		{
			rec_sta=0;
		}
	}
	else
	{
		rec_sta++;
	}
}
u8 flow_buf_msp_v[20];
void Flow_data_msp_v(u8 bytedata)
{	
	static u8 rec_sta_v=0;
	
	flow_buf_msp_v[rec_sta_v] = bytedata;
	//
	if(rec_sta_v==0)
	{
		if(bytedata==0x24)
		{
			rec_sta_v++;
		}
		else
		{
			rec_sta_v=0;
		}
	}
	else if(rec_sta_v==1)
	{
		if(bytedata==0x58)
		{
			rec_sta_v++;
		}	
		else
		{
			rec_sta_v=0;
		}		
	}
	else if(rec_sta_v==2)
	{
		if(bytedata==0x3c)
		{
			rec_sta_v++;
		}	
		else
		{
			rec_sta_v=0;
		}		
	}
	else if(rec_sta_v==6)
	{
		if(bytedata==0x09)
		{
			rec_sta_v++;
		}	
		else
		{
			rec_sta_v=0;
		}		
	}
	else if(rec_sta_v==17)//高度信息
	{
		if(flow_buf_msp_v[6] == 0x09)
		{
			//解析成功
			Flow_Data_Analysis_msp_h(flow_buf_msp_v);
			//
			rec_sta_v=0;
		}else
		{
			//
			rec_sta_v=0;
		}
	}
	else
	{
		rec_sta_v++;
	}
}
static void Flow_Data_Analysis_msp_h(u8 *buf_data)
{
		if(*(buf_data+6)==0x05)flow_data_msp.quality_h=*(buf_data+8);//激光数据可用0x64
		if(*(buf_data+6)==0x09)flow_data_msp.quality_m=*(buf_data+8);//光流数据可用0xF5
		//if(flow_data_msp.quality_h == 0x64 && *(buf_data+6)==0x05)
		if(*(buf_data+6)==0x05)
		{
			flow_data_msp.distance=(*(buf_data+9)|*(buf_data+10)<<8|*(buf_data+11)<<16|*(buf_data+12)<<24);
		}
		//else if(flow_data_msp.quality_m == 0xF5 && *(buf_data+6)==0x09)
		else if(*(buf_data+6)==0x09)
		{
			flow_data_msp.montion_x=(((*(buf_data+9)|*(buf_data+10)<<8|*(buf_data+11)<<16|*(buf_data+12)<<24) *10*flow_data_msp.distance));//cm/s
			flow_data_msp.montion_y=((*(buf_data+13)|*(buf_data+14)<<8|*(buf_data+15)<<16|*(buf_data+16)<<24) *10*flow_data_msp.distance);//cm/s
			flow_data_msp.montion_x=flow_data_msp.montion_y/100;
			flow_data_msp.montion_y=flow_data_msp.montion_x/100;
		}
}

