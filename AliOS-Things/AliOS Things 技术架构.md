# AliOS Things 技术架构

AliOS Things 架构可以适用于分层架构和组件化架构。一般来说，从底部到顶部，AliOS Things 包括：

1. 板级支持包（BSP）：主要是由SoC供应商开发和维护
2. 硬件抽象层（HAL）：比如WiFi 和 UART
3. 内核： 包括Rhino实时操作系统内核、Yloop、VFS、KV存储
4. 协议栈： 包括TCP/IP协议栈（LwIP），uMesh网络协议栈
5. 安全 ： 安全传输层协议（TLS），可信服务框架（TFS） 可信运行环境（TEE）
6. AOS API： 提供可供应用软件和中间件使用的API
7. 中间件： 包括常见的物联网组件和阿里巴巴增值服务中间件
8. 示例应用： 各种示例代码

# AliOS Things内核：Rhino实时操作系统内核

特点：

1. **体积小**。为大多数的内核对象提供静态和动态分配。为小内存块设计的内存分配器。大部分的内核特性都是可以裁剪的。通过h文件进行配置和裁剪。K_config.h文件一般位于开发板目录结构下。
2. **功耗低**。提供了CPU 的 tickless idle模式来帮助系统节约电能和延长时间。类似于FREERTOS的tickless idle模式。
3. **实时性**。Rhino提供了两个调度策略。一个是基于优先级的抢占式调度和round-robin循环调度策略。对于这两个调度策略而言，具有最高优先级的任务都是被优先处理的。
4. **调试方便**。Rhino可以支持stack溢出、内存泄漏、内存损坏的检测。

## Yloop事件框架

Yloop事件框架是AliOS Things的异步事件框架。提供了一套机制来统一调度管理IO（主要是socket）、定时器、执行函数和事件。在降低了内存的使用的同时，还避免了多线程编程的复杂性。

每个Yloop实例（aos_loop_t）与特定的任务上下文绑定。主任务以外的任务也可以创建自己的Yloop实例。多Yloop实例也可以被创建，其中每个instance都被绑定到一个单一的任务，让强大的硬件获得更好的性能。

# 键值对存储（KV）

KV组件时AliOS Things中一个以key-value方式进行持久化存储的轻量级组件，主要为基于Nor Flash的小型MCU设备提供通用的key-value持久化存储接口。

其优势和特征包括：

1. 更少的擦写次数来延长flash的使用寿命
2. 电源安全，没有中间状态存在
3. 方便使用，关键码值支持二进制格式数据
4. 最低支持的flash是8kb

# 协议栈介绍

AliOS Things通过灵活的方式提供协议栈，其主要目的是为了帮助设备更容易的连接到云端。

1. 面向IP设备
   - 为直接连接的SoC提供了测试良好的LwIP协议栈，包括WiFi SoC，MCU+SDIO/SPI WIFI模块等
   - 为连接了通信模块（如WiFi   NB   GPRS）的MCU提供了SAL
   - 提供uMesh 去构建更复杂的网状网络拓扑
2. 面向非IP设备
   - LoRaWAN协议栈已经集成到系统当中
   - 提供的BLE标准的API和BLE协议栈

## TCP/IP协议栈（LwIP）

AliOS Things 拥有一个基于LwIP V2.0.0的TCP/IP协议栈，支持IPv4，IPv6,IPv4与IPv6共存。IPv4和IPv6已经在持续集成（CI）系统中经过良好测试，IPv6也已经在uMesh中被广泛使用和测试

## 套接字适配层（SAL）

SAL为WiFi/GPRS/NO-IoT系列模组提供了标准的Socket功能。对于AT命令在这个场景中的广泛使用，提供了AT Parser来帮助处理。

有了SAL，开发人员可以使用标准的Socket API访问网络，从而减少现有软件组件的集成工作。

## 自组织网络协议（uMesh）

uMesh是一个具有如下特征的mesh：

- 无缝支持IPv4 和IPv6
- RF标准独立，支持WiFi，BLE， 802.11和802.15.4等通信媒介
- 支持不同通信媒介间的异构组网
- 支持树状拓扑，网状拓扑和分层树状网格拓扑
- 支持低功耗特性
- 使用ID2对设备进行认证，AES-128对数据进行加密

# 安全性介绍

## 安全传输层协议（TLS）

继承于mbedtls,对footprint进行了高度优化

## 信任框架服务（TFS）

可以对接大部分的安全服务（比如ID2–一种安全策略，以后的文章中会涉及）的框架。

## 网络设备ID（ID2）

可信的IoT设备身份属性

## 秘钥管理（KM）

通过使用硬件的安全功能提供可信的runtime Root

## Ali-Crypto

提供经典的算法实现

## 可信执行环境（TEE）

提供完整的可信执行环境

# 中间组件

## 空中固件升级（FOTA）

FOTA 使设备固件更新容易。AliOS Things可根据硬件配置给出FOTA解决方案，提供端到端的解决方案。

特征:

- 支持丰富的物联网协议 (Alink，MQTT ，COAP）
- 支持HTTP / HTTPS / COAP固件下载
- 支持多bin、delta和A／B更新
- 提供OTA HAL方便进入端口

## uData

uData框架是基于传统Sensor Hub概念，结合IoT的业务场景和AliOS Things物联网操作系统的特点设计而成的一个面对IoT的感知设备处理框架。

uData框架主要分kernel和framework两层，kernel层主要是负责传感器驱动，硬件端口配置和相关的静态校准，包括轴向校准等；framework层主要是负责应用服务管理、动态校准管理和对外模块接口等。

uData的目标范围服务是物联网商业服务，像无人机玩具、智能路灯、扫地机器人等。传感器驱动程序不仅能提供传感器SDK，还能提供传感器驱动像ALS，气压计，温度，加速度计，陀螺仪，磁力计等。

## 物联网协议

AliOS Things支持丰富的云端连接协议：

- Alink：阿里云平台，适用于智能生活； 也包括WiFi配置组件YWSS。
- MQTT：标准MQTT协议；已和阿里云物联网套件良好结合。
- COAP：基于UDP的轻量级协议。和COAP FOTA结合便可为NB-IoT设备建立一个只有UDP的系统。

## AT解析器

AT解析器提供了处理AT命令连接通信模块的框架。AT解析器可处理串行流解析；回调OOB可以处理模块的特殊AT命令。与SAL一起使用，应用程序甚至可以在AT模块中使用BSD socket。

# 总结

AliOS Things是为了电量和资源有限的MCU、连接套接字SoC设计的，非常适合于物联网的设备。使用AliOS Things并借助阿里云的强大平台，可以实现物联网设备的快速简洁开发。



## 转载

https://www.yuque.com/beiqiang/alios_things/anrsiv

