# AliOS Things的启动过程分析（二）

在AliOS Things的启动过程分析（一）中分析了developerkit从系统上电到调用main函数所经历的一些步骤，接下来详细分析一下main函数的一些工作，主要是内核的相关初始化工作。main函数所处的位置位于               platform\mcu\stm32l4xx_cube\aos\aos.c文件中。下面是main函数的代码。

```
int main(void)
{
    sys_start();
    return 0;
}

static void sys_start(void)
{
    aos_heap_set();   //设置堆的起始地址和长度
    
    aos_init();		//即内核初始化，在其中完成了必要的初始化（内存初始化、内核对象初始化）、新建立了几个默认的任务（定时器任务、空闲任务、工作队列等）
//建立sys_init任务，进入应用层代码
    krhino_task_create(&demo_task_obj, "aos-init", 0,AOS_DEFAULT_APP_PRI, 
        0, demo_task_buf, AOS_START_STACK, (task_entry_t)sys_init, 1);
    
    aos_start();   //即内核启动。代表开始任务调度
}

void aos_heap_set(void)
{
    g_mm_region[0].start = (uint8_t*)&__bss_end__;   //在ld链接文件中指定的地址
    g_mm_region[0].len   = 
        ((uint8_t*)&_estack - (uint8_t*)&__bss_end__) - RHINO_CONFIG_SYSTEM_STACK_SIZE;
                                     //RAM区的尾地址-bss段的结束地址-偏移地址
}
```

在main函数之中完成的工作是首先要使用aos_heap_set进行堆区域的设置，之后使用aos_init函数（其实是对于krhino_init函数的封装）进行内核的初始化，在内核初始化之后，建立一个sys_init的系统任务，在该系统任务中会调用应用业务层代码application_start()函数。前面已经介绍了aos_heap_set函数的主要目的，接下来分析aos_init(即krhino_init的源码)

```
kstat_t krhino_init(void)
{
    g_sys_stat = RHINO_STOPPED;   //设置系统状态为STOPPED

#if (RHINO_CONFIG_USER_HOOK > 0)    //钩子函数，默认是开启的
    krhino_init_hook();            //钩子函数初始化
#endif

#if (RHINO_CONFIG_CPU_NUM > 1)      //如果是多核的处理器，因为developer仅仅有一个CPU，所以处理不会执行
    krhino_spin_lock_init(&g_sys_lock);
#endif

//运行队列初始化，系统中如果有多个任务并行处理的话，建立过多的任务对于系统的压力很大，使用工作队列同样可以
//实现任务的相关操作，减轻系统压力
    runqueue_init(&g_ready_queue);  

    tick_list_init();//初始化g_tick_head双向链表

/*内核对象初始化，内核对象数据结构为：kobj_list_t 内部包含：互斥量   信号量  消息队列  内存池 事件等*/
#if (RHINO_CONFIG_SYSTEM_STATS > 0)
    kobj_list_init();
#endif

//内存初始化，根据前面aos_heap_init函数的g_mm_region参数对内存进行初始化
#if (RHINO_CONFIG_MM_TLF > 0)
    k_mm_init();
#endif

//动态内存管理初始化，
#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
    klist_init(&g_res_list);
    krhino_sem_create(&g_res_sem, "res_sem", 0);
    dyn_mem_proc_task_start();
#endif


#if (RHINO_CONFIG_CPU_NUM > 1)
    for (uint8_t i = 0; i < RHINO_CONFIG_CPU_NUM; i++) {
        krhino_task_cpu_create(&g_idle_task[i], "idle_task", NULL, RHINO_IDLE_PRI, 0,
                               &g_idle_task_stack[i][0], RHINO_CONFIG_IDLE_TASK_STACK_SIZE,
                               idle_task, i, 1u);
    }
#else  //单核处理器创建空闲任务，系统空闲时运行该任务
    krhino_task_create(&g_idle_task[0], "idle_task", NULL, RHINO_IDLE_PRI, 0,
                       &g_idle_task_stack[0][0], RHINO_CONFIG_IDLE_TASK_STACK_SIZE,
                       idle_task, 1u);
#endif

//工作队列初始化
#if (RHINO_CONFIG_WORKQUEUE > 0)
    workqueue_init();
#endif

//定时器初始化，建立一个定时器任务，用于管理系统的定时器列。
#if (RHINO_CONFIG_TIMER > 0)
    ktimer_init();
#endif

    rhino_stack_check_init();

    return RHINO_SUCCESS;
}
```

以上为内核的初始化部分的相关介绍，主要是新建立了几个任务，初始化了一些必要的内核对象等。内核初始化的细节部分没有具体去介绍，因为内核部分涉及到的数据结构较多，在此处进行介绍，篇幅较长，以后建立一个内核的章节来介绍其内部实现细节。

在完成了内核的初始化工作之后，会调用任务建立接口新建立一个sys_init的任务，该任务会进行板级的相关初始化，并调用业务层代码入口函数application_start。

新建立了任务仅仅是在任务队列中加入了该任务，任务并没有开始调度执行，aos_start（即krhino_start）为内核启动进入调度的函数。此时sys_init任务才能够运行起来。

aos_start内核调度函数如下所示：

```
kstat_t krhino_start(void)
{
    ktask_t *preferred_task;

    if (g_sys_stat == RHINO_STOPPED) {  //当前系统为STOPPED状态
#if (RHINO_CONFIG_CPU_NUM > 1)     //多核处理器的调度方法
        for (uint8_t i = 0; i < RHINO_CONFIG_CPU_NUM; i++) {
            preferred_task            = preferred_cpu_ready_task_get(&g_ready_queue, i);
            preferred_task->cpu_num   = i;
            preferred_task->cur_exc   = 1;
            g_preferred_ready_task[i] = preferred_task;
            g_active_task[i]          = g_preferred_ready_task[i];
            g_active_task[i]->cur_exc = 1;
        }
#else          //单核处理器
       //从RDY队列中取出最高优先级的任务
        preferred_task = preferred_cpu_ready_task_get(&g_ready_queue, 0);
        g_preferred_ready_task[0] = preferred_task;//保存位于RDY队列中的当前最高优先级的任务
        g_active_task[0] = preferred_task;  //当前运行的任务
#endif

#if (RHINO_CONFIG_USER_HOOK > 0)
        krhino_start_hook();   //启动钩子函数，主要用于记录当前任务的时间信息
#endif

        g_sys_stat = RHINO_RUNNING;  //修改系统状态为RUNNING
        cpu_first_task_start();  //任务启动，设置任务控制块

        /* should not be here */
        return RHINO_SYS_FATAL_ERR;
    }

    return RHINO_RUNNING;
}
```

aos_start函数运行起来之后，相当于此时内核已经完成了调度。当前系统中除了默认建立的空闲任务和定时器任务之外，还有一个刚刚建立的sys_init的任务。接下里分析一下sys_init函数所完成的主要功能：

sys_init函数代码如下所示，这是系统开始运行的第一个和硬件版以及应用程序有关的任务代码，前面的初始化部分仅仅是内核相关的初始化代码。在该函数中完成的功能是：

1. SoC处理器与硬件开发板相关初始化
2. hal
3. 开发板的flash分区以及cli组件初始化
4. 向应用程序传递参数初始化
5. alios things相关的组件初始化

```
static void sys_init(void)
{
/*在该函数中主要完成flash 预存取 、数据以及指令缓存使能或者关闭，
//设置NVIC 组别，设置时钟时基标准、设置。以及进行HAL_Mspinit硬件初始化*/
    stm32_soc_init();
#ifdef BOOTLOADER
    main();
#else
//进行OTA对象的初始化以及wifi的初始化
    hw_start_hal();
//对flash区域进行逻辑分析，设置各个分区的起始和终止地址
//新建一个fota信号量
    board_init();
//传参初始化，设置传参个数，数据地址，以及是否使能cli组件
    var_init();
#ifdef AOS_CPLUSPLUS
    cpp_init();
#endif
//函数完成的功能是各个组件的初始化以及进入应用程序
    aos_kernel_init(&kinit);
#endif
}
```

接下来主要分析aos_kernel_init(&kinit);函数，以注释的形式进行分析

```
int aos_kernel_init(kinit_t *kinit)
{
  //VFS虚拟文件系统初始化，详情可以参考https://www.yuque.com/beiqiang/linux_related/itm2vp  的介绍
#ifdef AOS_VFS
    vfs_init();  //建立VFS互斥锁，初始化inode数组
    vfs_device_init();  //注册/dev/event节点以及操作方法
#endif
    
#ifdef CONFIG_AOS_CLI
    cli_service_init(kinit);  //CLI组件的初始化，并将参数传递进来
#else
    extern void log_no_cli_init(void);
    log_no_cli_init();
#endif
    
#ifdef AOS_KV
    aos_kv_init();   //KV组件存储初始化
#endif

#ifdef WITH_SAL
    sal_device_init();  //SAL网络框架初始化
#endif

#ifdef AOS_LOOP
    aos_loop_init();   //在主任务中系统默认创建了一个YLOOP实例
#endif

#ifdef OSAL_RHINO
    trace_start();
#endif

#ifdef AOS_UOTA 
    ota_service_init(); //OTA固件升级初始化
#endif

#ifdef AOS_SENSOR
    sensor_init(); //传感器模块初始化
#endif

#ifdef AOS_GPS
    gps_init();
#endif


// auto_component generated by the compiler system, now gcc support
#if defined (__GNUC__) && !defined (__CC_ARM)
    //aos_components_init();
#endif

#ifdef AOS_BINS
    app_pre_init();

#ifdef AOS_FRAMEWORK_COMMON
        aos_framework_init(); //MESH网络的初始化工作，uData传感器框架的初始化工作
#endif

    if (app_info->app_entry) {
#if (RHINO_CONFIG_USER_SPACE > 0)
        app_info->app_entry(0, NULL);
#else
        app_info->app_entry((void *)kmbins_tbl, 0, NULL);
#endif
    }
#else

#if (RHINO_CONFIG_CPU_USAGE_PERIOD > 0)  //CPU 使用记录
    krhino_task_cpu_usage_init();
#endif

    application_start(kinit->argc, kinit->argv);  //调用业务层代码入口函数
#endif

    return 0;
}
```

#### 总结

以上就是从系统开始进入main函数到调用业务层函数入口application_start的全部过程，其中有很多组件以及内核的初始化工作。为了能够更好的实现自己的代码，对系统的整个流程有一个清晰的认识是很必要的工作。在本篇文章中，仅仅是简单的分析了较为顶层的一个初始化流程，具体的细节会在后续的文章中继续分析。

### 转载

[AliOS Things的启动过程分析（二）](https://www.yuque.com/beiqiang/alios_things/nnfgvs)