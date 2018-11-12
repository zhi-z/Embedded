# 云屏helloWord

## 1 主函数

```
int main(void)
{
    char size;
    # 串口初始化
	InitUsart1(921600);
	# 控件初始化
    InitMsGui();
    DelayMs(1000);//启动画面延时1S 
    InitMenu();

    while(1)
    {
	   // 云屏下法宗指令处理
        size = GetQueueCommand(cmdBuffer, CMD_SIZE);
        if(size>0){
            MsGui.Msg.ProcessMsg((PMsPhoneMessage)cmdBuffer);
        }

				// 页面刷新
				UpdateUI();
    }
}
```

## 2 在menu.h中创建窗口结构体

```
#ifndef __MENU_H
#define __MENU_H

#include "msGUI.h"

extern void InitMenu(void);

typedef struct
{
    Window window;
    Label labelHelloWorld;
}HelloWorldWindow;

typedef struct
{
    HelloWorldWindow helloWorldWindow;

}MenuStruct;

typedef struct
{
    MenuStruct Menu;
}AppStruct;

extern AppStruct App;
#endif


```

## 3 对窗口进行初始化

```
// 这个部分要在主函数中调用
void InitMenu(void)
{
    InitWindows();
    InitHelloWorldWindow();
}
```

```
// 窗口初始化
void InitWindows(void)
{  
	// 创建窗口
    MsGui.Window.InitWindow(&App.Menu.helloWorldWindow.window);
    // 窗口名称
    App.Menu.helloWorldWindow.window.Id = "HelloWorldId";
    // 显示窗口
    App.Menu.helloWorldWindow.window.Display = "1";
    // 设置窗口背景颜色
    App.Menu.helloWorldWindow.window.BackgroundColor = "#ffffff";
}
```

```
// 对初始化lable，把控件添加到窗口
void InitHelloWorldWindow(void)
{
    Label *tempLabel = NULL;
	// 创建窗口
    MsGui.Window.CreateWindow(&App.Menu.helloWorldWindow.window);
	// 获取要添加lable的地址
    tempLabel = &App.Menu.helloWorldWindow.labelHelloWorld;
	// 对lable进行初始化
    MsGui.Label.InitLabel(tempLabel);
	// 设置lable属性
    tempLabel->Id = "HWID";
    tempLabel->FontColor = "blue";
    tempLabel->FontSize = "55";
    tempLabel->Left = "150";
    tempLabel->Top = "150";
    tempLabel->Width = "400";
    tempLabel->Name = "hello world ! ! !";
	// 添加lable到窗口中
    MsGui.Label.AddLabel(&App.Menu.helloWorldWindow.window, tempLabel);
}
```

通过以上两个步骤，可以往云屏中添加一个文字，完成一个云屏hello的实现。

## 4 实现的效果

![1530929238355](C:\Users\JH\Desktop\RD总结\assets\1530929238355.png)

完成一个hello的实现。