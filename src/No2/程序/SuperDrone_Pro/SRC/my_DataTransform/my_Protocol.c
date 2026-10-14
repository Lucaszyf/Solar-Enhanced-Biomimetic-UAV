#include "my_protocol.h"

// F1 
u8 F1_Data[100];
//匿名科技给用户的ID分配为0XF1 - 0XFF,广播地址统一为0XFF
void ANO_SendF1(s8 _1, s16 _2, s16 _3)
{	
	u8 _cnt = 0;
	
	F1_Data[_cnt++] = 0xAA;
	F1_Data[_cnt++] = 0xFF;
	F1_Data[_cnt++] = 0xF1;
	
	F1_Data[_cnt++] = 5; //LEN
	
	// _1
	F1_Data[_cnt++] = BYTE0(_1);
	
	// _2
	F1_Data[_cnt++] = BYTE0(_2);
	F1_Data[_cnt++] = BYTE1(_2);
	//_3
	F1_Data[_cnt++] = BYTE0(_3);
	F1_Data[_cnt++] = BYTE1(_3);
	
	u8 sc=0, ac=0;
	for(u8 i=0;i<F1_Data[3]+4;i++)
	{
		sc += F1_Data[i];
		ac += sc;
	}
	F1_Data[_cnt++] = sc;
	F1_Data[_cnt++] = ac;
	//发送数组、长度
	Uart5_Send(F1_Data, _cnt);

}

// F2
u8 F2_Data[100];
void ANO_SendF2(s16 _1, s16 _2, s16 _3, s16 _4, s16 _5, s16 _6, s16 _7, s16 _8, s16 _9,s8 _10)
{	
	u8 _cnt = 0;
	
	F2_Data[_cnt++] = 0xAA;
	F2_Data[_cnt++] = 0xFF;
	F2_Data[_cnt++] = 0xF2;
	
	F2_Data[_cnt++] = 19; //LEN
	
	// _1
	F2_Data[_cnt++] = BYTE0(_1);
	F2_Data[_cnt++] = BYTE1(_1);
	
	// _2
	F2_Data[_cnt++] = BYTE0(_2);
	F2_Data[_cnt++] = BYTE1(_2);
	
	//_3
	F2_Data[_cnt++] = BYTE0(_3);
	F2_Data[_cnt++] = BYTE1(_3);
	
	// _4
	F2_Data[_cnt++] = BYTE0(_4);
	F2_Data[_cnt++] = BYTE1(_4);
	
	// _5
	F2_Data[_cnt++] = BYTE0(_5);
	F2_Data[_cnt++] = BYTE1(_5);
	
	//_6
	F2_Data[_cnt++] = BYTE0(_6);
	F2_Data[_cnt++] = BYTE1(_6);
	
	//_7
	F2_Data[_cnt++] = BYTE0(_7);
	F2_Data[_cnt++] = BYTE1(_7);

	//_8
	F2_Data[_cnt++] = BYTE0(_8);
	F2_Data[_cnt++] = BYTE1(_8);

	//_9
	F2_Data[_cnt++] = BYTE0(_9);
	F2_Data[_cnt++] = BYTE1(_9);
	
	//_10
	F2_Data[_cnt++] = BYTE0(_10);

	u8 sc=0, ac=0;
	for(u8 i=0;i<F2_Data[3]+4;i++)
	{
		sc += F2_Data[i];
		ac += sc;
	}
	F2_Data[_cnt++] = sc;
	F2_Data[_cnt++] = ac;
	
	Uart5_Send(F2_Data, _cnt);

}
