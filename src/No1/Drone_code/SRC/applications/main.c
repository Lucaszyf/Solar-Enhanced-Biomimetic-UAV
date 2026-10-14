#include "include.h"
#include "Ano_FcData.h"

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{ 
  while (1)
  {
		//当系统出错后，会进入这个死循环
  }
}
#endif
//=======================================================================================
//=======================================================================================
int main(void)
{
	flag.start_ok = All_Init();		//进行所有设备的初始化，并将初始化结果保存
	Scheduler_Setup();				//调度器初始化，系统为裸奔，这里人工做了一个时分调度器
	while(1)
	{
		Scheduler_Run();			//运行任务调度器，所有系统功能，除了中断服务函数，都在任务调度器内完成
	}
}

