# AliOS Things 笔记

1. 由于STM32的驱动函数和HAL层定义的接口并非完全相同，所以需要在STM32L4驱动上封装一层。





创任务：

aos_task_new 

hal_gpio_output_toggle(&led); // 这个函数在内核部分