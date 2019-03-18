# 基于msOS自动回火机的实现

## 1 简介

### 1.1 背景

基于msOS自动回火机的实现的实例是学习msOS比较好的入门实例，它包括msOS界面的设计、数据库的使用、系统节拍的使用、按键的使用、系统节拍使用等，通过这个例子可以很好的对msOS的使用有个大概的了解。

### 1.2 自动会火机简介

自动回火机是一个基于msOS系统开发的温度计时控制系统，可应用于焊接、回炉加热设备的控制，从A0口读取模拟量转为温度值并能够实时显示到菜单界面中，OUT1为加热状态控制端口，OUT2为保温控制端口。自动回火机能够实现升温计时，保温计时以及降温计时。在升温、保温以及降温过程中，能够在界面显示当前的状态。保温时间、保温值、降温值可根据用户需求进行修改。 整个项目主要分为两个部分，一个是界面的设计，另一个是逻辑业务的设计。

## 2 界面设计

### 2.1 界面组成部分

实现的界面效果如下图所示。该界面的控件包括4个Lable和3个TextBox，这三个TextBox是可调的分别是保温时间、保温的温度和降温的目标温度。图中长方形圈起来的为lable控件，只可读，而椭圆圈的为TextBox控件，是可读可写的。

![1531473028633](assets\1531473028633.png)

### 2.2 界面的实现过程

界面的实现分的过程分为三个步骤：

（1）首先在AppStruct的Menu中添加需要增加的界面，这个结构体在menu.h和data.h中，两个地方都要修改，如下图所示

![1531473461561](assets\1531473461561.png)

（2）创建界面初始化函数，在这个界面中直接设置界面要要显示的内容，添加控件等，实现的函数代码如下，包括控件的创建、添加，界面背景的创建。

```
void InitWorkForm(void)
{
	static Label TemperatureUpTimeLabel;  // 升温时间
    static Label CurrentTemperatureLabel; // 当前温度
	static TextBox TemperatureHoldTimeTextBox;  // 保温时间
	static TextBox TemperatureHoldTextBox;  // 保温温度
	static Label TemperatureDownTimeLabel;  // 降温时间
	static TextBox TemperatureDownTextBox;  // 降温目标温度
	
    static Label TemperatureStateLabel;
    static const string TemperatureStateString[] = {"升温","保温","降温","待机"};
    
    System.Gui.Form.Init(&App.Menu.WorkForm);
    App.Menu.WorkForm.BackTextPointer = "升温：   S    ℃"
                                        "保温：   S    ℃"
                                        "降温：   S    ℃"
                                        "状态：         ";

    // 加热时间
    System.Gui.Form.AddLabel(&App.Menu.WorkForm, &TemperatureUpTimeLabel);
    TemperatureUpTimeLabel.DataPointer = (void *)(&App.Data.TemperatureUpTime);
    TemperatureUpTimeLabel.Type = GuiDataTypeIntDec;
    TemperatureUpTimeLabel.Digits = 2;
    TemperatureUpTimeLabel.X = 7;
    TemperatureUpTimeLabel.Y = 0;

    // 当前温度
    System.Gui.Form.AddLabel(&App.Menu.WorkForm, &CurrentTemperatureLabel);
    CurrentTemperatureLabel.DataPointer = (void *)(&App.Data.CurrentTemperature);
    CurrentTemperatureLabel.Type = GuiDataTypeIntDec;
    CurrentTemperatureLabel.X = 13;
    CurrentTemperatureLabel.Y = 0;

    // 保温时间
    System.Gui.Form.AddTextBox(&App.Menu.WorkForm, &TemperatureHoldTimeTextBox);
    TemperatureHoldTimeTextBox.DataPointer = (void *)(&App.Data.TemperatureHoldTime);
    TemperatureHoldTimeTextBox.Type = GuiDataTypeIntDec;
    TemperatureHoldTimeTextBox.DataMax = 360;
    TemperatureHoldTimeTextBox.DataMin = 0;
    TemperatureHoldTimeTextBox.DataBigStep = 10;
    TemperatureHoldTimeTextBox.DataStep = 1;
    TemperatureHoldTimeTextBox.X = 7;
    TemperatureHoldTimeTextBox.Y = 1;
		
    // 保温温度
    System.Gui.Form.AddTextBox(&App.Menu.WorkForm, &TemperatureHoldTextBox);
    TemperatureHoldTextBox.DataPointer = (void *)(&App.Data.TemperatureHold);
    TemperatureHoldTextBox.Type = GuiDataTypeIntDec;
    TemperatureHoldTextBox.DataMax = 900;
    TemperatureHoldTextBox.DataMin = 300;
    TemperatureHoldTextBox.DataBigStep = 100;
    TemperatureHoldTextBox.DataStep = 10;
    TemperatureHoldTextBox.X = 13;
    TemperatureHoldTextBox.Y = 1;

    // 降温时间
    System.Gui.Form.AddLabel(&App.Menu.WorkForm, &TemperatureDownTimeLabel);
    TemperatureDownTimeLabel.DataPointer = (void *)(&App.Data.TemperatureDownTime);
    TemperatureDownTimeLabel.Type = GuiDataTypeIntDec;
    TemperatureDownTimeLabel.X = 7;
    TemperatureDownTimeLabel.Y = 2;

    // 降温温度
    System.Gui.Form.AddTextBox(&App.Menu.WorkForm, &TemperatureDownTextBox);
    TemperatureDownTextBox.DataPointer = (void *)(&App.Data.TemperatureDown);
    TemperatureDownTextBox.Type = GuiDataTypeIntDec;
    TemperatureDownTextBox.DataMax = 900;
    TemperatureDownTextBox.DataMin = 300;
    TemperatureDownTextBox.DataBigStep = 100;
    TemperatureDownTextBox.DataStep = 10;
    TemperatureDownTextBox.X = 13;
    TemperatureDownTextBox.Y = 2;

    // 状态显示
    System.Gui.Form.AddLabel(&App.Menu.WorkForm, &TemperatureStateLabel);
    TemperatureStateLabel.DataPointer = (void *)(&App.Data.TemperatureState);
    TemperatureStateLabel.Type = GuiDataTypeSnString;
    TemperatureStateLabel.Align = GuiDataAlignRight;
    TemperatureStateLabel.StringBlockPointer = TemperatureStateString;
    TemperatureStateLabel.X = 15;
    TemperatureStateLabel.Y = 3;

}
```



## 3 逻辑业务中实现

逻辑业务的设计主要读取读取模拟量，通过端口控制加入和夹子的控制，在整系统中我设置了四个状态进行考虑，分别为升温、保温、降温和待机转态，这四个状态我是以全局变量的形式保存在数据库中的。在逻辑业务中每当到达1s之后就就读取模拟量的输入，并显示在显示屏上，同时也判断当前的状态，比如刚开始按键开始的时候，就设置为升温状态，这时候，控制温度的端口和控制夹子的端口也打开。在节拍中实现的代码如下，这部分的代码只是实现了通过按键控制的模式。

```
static void AppSystick100(void) // 每10ms扫描一次
{
    float AdcData;
    int TemporaryData;
    static byte Time10msNumberFlag = 0;   
    
    Time10msNumberFlag++;
    
    if(Time10msNumberFlag == 100)//1
    {						
        Time10msNumberFlag = 0;
        TemporaryData = App.Data.Adc.A0;
        if (TemporaryData >= 744)
            AdcData = TemporaryData - 744;
        else
            AdcData = 0;
        AdcData = AdcData * 50 / 186 + 300;		
        App.Data.CurrentTemperature = AdcData;//显示温度
				
        if (App.Data.TemperatureState == TemperatureUpState)
        {
            App.Data.DO.Y0 = on;  //启动夹子
            App.Data.DO.Y1 = on;  //开始加热 
            App.Data.TemperatureUpTime++;   // 升温时间
				
            if (App.Data.CurrentTemperature >= App.Data.TemperatureHold)//温度超过设定阈值
            {
                App.Data.TemperatureState = TemperatureHoldState;  //设置保温为保温状态，停止计时，进入保温状态          
            }		
        }
		else if (App.Data.TemperatureState == TemperatureHoldState)
        {
            App.Data.TemperatureHoldTime--;      //保温倒计时
            App.Data.DO.Y1 = off;         //结束加热
            App.Data.DO.Y2 = on;          //开始保温
            
            if (App.Data.TemperatureHoldTime<=0) //保温结束
            {
                App.Data.TemperatureHoldTime = App.Data.InitTemperatureHoldTime;  //重新读取保温时间
                App.Data.DO.Y2 = off;     //结束保温
                App.Data.TemperatureState = TemperatureDownState;// 设置为降温状态，进行降温	
            }
            	
        }
		else if (App.Data.TemperatureState == TemperatureDownState)
        {
            App.Data.TemperatureDownTime++;//降温计时
            
            if (App.Data.TemperatureDown >= App.Data.CurrentTemperature) //当前温度低于设定的降温温度
            {				
                PostMessage(MessageKey, KeyLongStop);	//抛一个按键消息，结束降温计时 
                System.Device.Misc.SetBeep(true); //蜂鸣器打开
                System.Device.Timer.Start(0, TimerSystick, 100, BeepOff);
            }
        }
		 
	}       	
}
```

在实现以上代码之前首先在数据库中添加我们需要增加的变量，在使用过程中我发现，对于需要在显示屏上显示的数据变量，最好定义在数据库中，这样可以方便进行读写和显示。需要注意的是，在系统节拍中使用蜂鸣器最好是使用定时器来实现。

## 4 总结

通过这个案列，让我对msOS的系统有了更深入的了解，也在系统界面与逻辑业务的使用变得更清晰了。通过这个实例，让我学会了界面、业务使用和IO口的使用。总之，通过这个例子让我学习到了很多不光是msOS的知识，也慢慢的发现msOS使用的一些套路，在此，特别感谢同事的帮助。