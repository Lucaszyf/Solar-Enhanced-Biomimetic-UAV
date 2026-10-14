#ifndef DATA_PROCESS_H_
#define DATA_PROCESS_H_

#include "Ano_FcData.h"

void Flow_data_get(u8 bytedata);
void Flow_data_msp_h(u8 bytedata);
void Flow_data_msp_v(u8 bytedata);
static void Flow_Data_Analysis(u8 *buf_data);
static void Flow_Data_Analysis_msp_h(u8 *buf_data);

typedef struct
{
	s16 pos_x;
	s16 pos_y;
	s16 dT_ms;
	s16 dis_h;
	s16 valid;
	u8 belief;
	
	short speed_x;
	short speed_y;
}flow_data_analysis;

typedef struct
{
	s32 montion_x;//rad/s
	s32 montion_y;//rad/s
	u32 distance;//mm
	u8 quality_m;
	u8 quality_h;
}flow_data_analysis_msp;

extern flow_data_analysis flow_data;
extern flow_data_analysis_msp flow_data_msp;
#endif
