#include "ANO_DT.h"
#include "Drv_usart.h"

#define BYTE0(dwTemp)       ( *( (char *)(&dwTemp)		) )
#define BYTE1(dwTemp)       ( *( (char *)(&dwTemp) + 1) )
#define BYTE2(dwTemp)       ( *( (char *)(&dwTemp) + 2) )
#define BYTE3(dwTemp)       ( *( (char *)(&dwTemp) + 3) )

void ANO_SendF1(s8 _1, s16 _2, s16 _3);
void ANO_SendF2(s16 _1, s16 _2, s16 _3, s16 _4, s16 _5, s16 _6, s16 _7, s16 _8, s16 _9,s8 _10);

