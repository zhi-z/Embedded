# AliOS-Things目录文件夹说明

| 文件夹名称 | 内容描述                                             |
| ---------- | ---------------------------------------------------- |
| 3rdparty   | 文件系统                                             |
| .vscode    | VS code工作区配置文件                                |
| board      | 评估板（如STM32L496G-Discovery）                     |
| build      | 编译框架                                             |
| device     | 连接MCU/SoC的外设，比如支持使用AT命令的WiFi系列模组  |
| doc        | 文档                                                 |
| example    | 代码示例，通过了完备测试的应用程序（比如Alink）      |
| framework  | IoT 通用组件                                         |
| include    | 系统头文件                                           |
| kernel     | 包括Rhino和协议栈                                    |
| platform   | 芯片架构支持的相关文件                               |
| security   | 包括TLS，TFS， TEE在内的安全组件                     |
| tools      | 命令行界面（CLI）和用于建立远程设备中心的testbed工具 |
| utility    | IoT通用软件库，比如 cJSON                            |
| test       | UT测试用例                                           |

### 开发板专有代码目录 

```
此目录包含了以下文件，开发板专有代码（如通信IO管脚配置，初始化资源等）、AOS针对该开发板的资源配置代码、以及板子专属外设代码等内容。以developerkit为例：

    board/
        developerkit/                  # 板级代码，如pin、io设置等开发板特有设置及代码
            aos/                       # AliOS系统的相关设定，包括flash存储分区，内核参数设定等等
                soc_init.c             # mcu相关资源初始化代码
                board.c                # flash分区设置
                k_config.h             # 内核缓冲区设置
            hal/                       # hal库对应的Pin定义，功能设定等
            Inc/                       # cubeMX生成的h文件放这里
            Src/                       # 官方cube生成的Src文件可以扔这里
                hal_msp.c              # 为重要文件，资源初始化需要调用
                main.c                 # 只从此文件提取系统时钟初始化函数及对应头文件资源，不从此文件的main执行
            developerkit.mk            # 板级组件编译设置文件，和makefile的用法一致
```

### 外部设备通用代码

```
device/
    sensor/                        # 传感器实现
```

### 系统中间件

```
framework/                         # 系统中间件实现
    fota/                          # 在线升级功能实现，需编译成Mbins模式
    uDate/                         # 传感器数据读取及维护
    fsyscall/                      # 文件系统调用
```

### AOS头文件

AOS函数定义，通过统一化HAL及通用功能的函数声明，以及参数传递方式，提高代码的移植性。

```
include/                           
    aos/                           # aos 系统头文件
        internal/
            event_type_code.h      # aos 事件定义
            log_impl.h             # log 相关定义
        kernel.h                   # 内核函数定义及说明
    hal/                           # aos 封装HAL库头文件
        soc/                       # mcu 相关HAL封装
            gpio.h                 # GPIO函数头文件
            uart.h                 # uart函数头文件
```

### AOS内核实现代码

```
kernel/                            # AliOS内核
```

### 平台通用代码

```
platform/
    arch/                          # 平台文件，不用管
    mcu/                           # 处理器
        stm32l4xx_cube/            # stm32l4系列处理器通用代码
            Drivers/               # 官方cube生成的HAL和LL库代码
            aos/
                aos.c              # 真正的函数入口点main
            hal/                   # 根据AOS的hal.h头文件重新封装的hal实现库，统一aos内硬件函数
```

### 工具类代码

```
utility/
    libc/                          # arm C标准库
```