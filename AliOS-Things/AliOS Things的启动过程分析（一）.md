# AliOS Things的启动过程分析（一）

在本篇文章中，我们以developerkit开发板为例，介绍AliOS Things的启动过程。AliOS Things支持多种工具链进行编译链接的方式生成可执行文件，在这里主要是以GCC的方式（更加通用）来进行介绍。建议可以先阅读AliOS Things的编译体系，对整个的编译流程有个简单的了解。

一个可执行文件生成首先需要确定宿主平台（windows、linux等）、再确定所使用的工具链、确定所有使用的组件源文件等等、最后根据指定的链接脚本进行连接生成可执行文件。而启动过程的理解，也需要从连接脚本和工具链讲起。

以developerkit为例，在board/developer目录下的developerkit.mk文件当中会指定所使用的启动文件以及选择指定的编译工具链。

如下所示，为截取的少量代码，主要是指定所使用的启动文件以及根据条件来选择所使用的链接脚本文件。在developerkit.mk文件中还定义了SoC的相关信息，指定了所使用的源文件等，详情请自行参考该文件。

```
ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES += startup_stm32l496xx_keil.s    
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES += startup_stm32l496xx_iar.s  
else
$(NAME)_SOURCES += startup_stm32l496xx.s
endif
#以上为确定所使用的编译工具，是armcc /  iar  /或者是gcc
#并选择所使用的启动文件

#下述代码是根据编译工具来指定其加载文件，在该文件中定义了一些基本的启动加载信息
ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=board/developerkit/STM32L496.sct
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config board/developerkit/STM32L496.icf
else
ifeq ($(post_run),1)
GLOBAL_LDFLAGS += -T board/developerkit/STM32L496VGTx_FLASH.ld
GLOBAL_DEFINES += VECT_TAB_OFFSET=0x4000
GLOBAL_DEFINES += USING_FLAT_FLASH
else
ifeq ($(MBINS),)
GLOBAL_LDFLAGS += -T board/developerkit/STM32L496VGTx_FLASH.ld
else ifeq ($(MBINS),app)
GLOBAL_LDFLAGS += -T board/developerkit/STM32L496VGTx_FLASH_app.ld
else ifeq ($(MBINS),kernel)
GLOBAL_LDFLAGS += -T board/developerkit/STM32L496VGTx_FLASH_kernel.ld
endif
endif
endif
```

接下来，我们将以gcc编译工具和STM32L496VGTx_FLASH.ld链接脚本为例来进行理解其启动过程。以下代码是

STM32L496VGTx_FLASH.ld链接脚本文件的部分代码。该文件所完成的最终目的是将编译后生成的文件进行连接生成最终的可执行文件。在该文件中定义了堆栈大小，存储区域的分配大小、以及各种段的定义和存储。本篇文章中只分析和启动过程相关的部分代码定义。

在链接文件的起始部分，指定了**程序的入口位置是：Reset_Handler**。该函数位于startup_stm32l496xx.s文件中，后续会进行讲解。并且在后面定义了RAM区的结束地址、堆以及栈的大小，RAM 区域和FLASH区域的地址等。

```
/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = 0x20050000;    /* end of RAM */
/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x200;      /* required amount of heap  */
_Min_Stack_Size = 0x400; /* required amount of stack */

/* Specify the memory areas */
MEMORY
{
RAM (xrw)      : ORIGIN = 0x20000000, LENGTH = 320K
FLASH (rx)      : ORIGIN = 0x8000000, LENGTH = 1024K
}
```

接下来分析一些后续启动过程可能会用到的段定义，详细的文件内容可以实际参考.ld链接脚本。在此只介绍和启动过程相关的定义。在段定义的开始，首先存储到FLASH区域的是向量表，向量表的定义也位于

startup_stm32l496xx.s文件中。**接下来是常见的代码段的存储、只读数据段的存储以及数据段和bss段的存储**（中间忽略了很多段定义，可以自行参考）

```
/* Define output sections */
SECTIONS
{
  /* The startup code goes first into FLASH */
  .isr_vector :
  {
    . = ALIGN(8);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(8);
  } >FLASH

  /* The program code and other data goes into FLASH */
  .text :
  {
    . = ALIGN(8);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(8);
    _etext = .;        /* define a global symbols at end of code */
  } >FLASH

  /* Constant data goes into FLASH */
  .rodata :
  {
    . = ALIGN(8);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(8);
  } >FLASH

  /* used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data : 
  {
    . = ALIGN(8);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(8);
    _edata = .;        /* define a global symbol at data end */
  } >RAM AT> FLASH
  
  /* Uninitialized data section */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM
```

在startup_stm32l496xx.s文件中和启动过程相关的代码如下所示：

Reset_Handler是程序启动后最先执行的代码（在ld文件中ENTRY指定），这段代码是由汇编语言编写的完成的主要功能是：

1. 设置sp堆栈指针。将其设置为_estack    在ld链接脚本中将其指定为`RAM`的截止地址
2. 将`.data数据段`的内容拷贝到`RAM`区，`.data数据段`:用于存放在编译阶段(而非运行时)就能确定的数据，可读可写。也是通常所说的静态存储区，赋了初值的全局变量、常量和静态变量都存放在这个域。`.data数据段`的定义在ld文件中。
3. 清除`bss段`。通常是指用来存放程序中未初始化的全局变量的一块内存区域。BSS是英文Block Started by Symbol的简称。BSS段属于静态内存分配。裸机程序需要手动清零。
4. 跳转到SystemInit函数中进行时钟的初始化。该函数位于board\developerkit\Src\system_stm32l4xx.c文件中。
5. 调用应用程序入口函数main函数，main函数位于AliOS-Things\platform\mcu\stm32l4xx_cube\aos\aos.c文件中。

```
  .section	.text.Reset_Handler
	.weak	Reset_Handler
	.type	Reset_Handler, %function
Reset_Handler:
  ldr   sp, =_estack    /* Atollic update: set stack pointer */

/* Copy the data segment initializers from flash to SRAM */
  movs	r1, #0
  b	LoopCopyDataInit

CopyDataInit:
	ldr	r3, =_sidata
	ldr	r3, [r3, r1]
	str	r3, [r0, r1]
	adds	r1, r1, #4

LoopCopyDataInit:
	ldr	r0, =_sdata
	ldr	r3, =_edata
	adds	r2, r0, r1
	cmp	r2, r3
	bcc	CopyDataInit
	ldr	r2, =_sbss
	b	LoopFillZerobss
/* Zero fill the bss segment. */
FillZerobss:
	movs	r3, #0
	str	r3, [r2], #4

LoopFillZerobss:
	ldr	r3, = _ebss
	cmp	r2, r3
	bcc	FillZerobss

/* Call the clock system intitialization function.*/
    bl  SystemInit
/* Call static constructors */
    bl __libc_init_array
/* Call the application's entry point.*/
	bl	main

LoopForever:
    b LoopForever
    
.size	Reset_Handler, .-Reset_Handler
```

以上就是developerkit从系统上电开始运行到系统进入应用程序主代码（main函数）所要经历的一些步骤，接下来会针对main函数的一些初始化工作进行讲解。因为是阅读源码进行分析的，如果有任何错误之处敬请谅解。



## 转载

[AliOS Things的启动过程分析（一）](https://www.yuque.com/beiqiang/alios_things/ciuhb2)