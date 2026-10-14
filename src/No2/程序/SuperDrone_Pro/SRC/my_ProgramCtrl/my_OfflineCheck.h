#ifndef MY_OFFLINECHECK
#define MY_OFFLINECHECK

#include "stm32f4xx.h"

// K210 0x42¼ì²â
extern u8 K210_42_offline_flag;
void K210_42_Offline_Check(u8 dT_ms);
void K210_42_Check_Reset(void);

#endif
