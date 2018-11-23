#  Makefile判断语句

Makefile中的ifeq 多条件使用
网上关于makefile中ifeq的介绍已经很多了，为什么我还要在写这篇文章，因为他们只说了if else两种条件的情况，并没有讲多于两种条件情况的使用。

多于两种情况的使用很简单，害我尝试很多种方法，如ifeq elifeq等等这些。其实就如同c中的if [else if] [else if]...else的使用一样，举个我使用的例子，Android中的NDK程序android.mk判断当前是哪种CPU架构：

    ifeq ($(TARGET_ARCH), arm)
        LOCAL_SRC_FILES := ...
    else ifeq ($(TARGET_ARCH), x86)
        LOCAL_SRC_FILES := ...
    else ifeq ($(TARGET_ARCH), mips)
        LOCAL_SRC_FILES := ...
    else 
        LOCAL_SRC_FILES := ...
    endif
很简单有木有，但网上的资料都没有去涉及，可能大多数来源都一样，呵呵，这是我看Makefile标准文档中得到了。希望给同样困惑的人一点帮助。

作者：liwugang43210 
来源：CSDN 
原文：https://blog.csdn.net/liwugang43210/article/details/47840429 
版权声明：本文为博主原创文章，转载请附上博文链接！