# AliOS Things 笔记

1. 由于STM32的驱动函数和HAL层定义的接口并非完全相同，所以需要在STM32L4驱动上封装一层。





创任务：

aos_task_new 

hal_gpio_output_toggle(&led); // 这个函数在内核部分





宏定义的问题：

如果在某些地方使用到宏定义，要想打开这个宏定义需要在Makefile文件中对添加这个宏开关，例如：GLOBAL_DEFINES += CONFIG_DM_DEVTYPE_SINGLE  ，打开CONFIG_DM_DEVTYPE_SINGLE 宏定义