# 基于msOS PID温度控制

## 1 简介

该实例是基于msOS平台，使用PID算法对温度进行控制。对于PID温度的控制，其实就是设定一个目标温度，通过PID算法让温度保持在目标温度左右，尽量的靠近目标温度。

## 2 硬件平台

- msOS开发板
- 100欧电阻
- LM35DT温度传感器

## 3 PID简介

对于PID的了解是在大学的时候开始的，主要是知道在工业控制方面用到很多，但不知道具体用在哪里，但一直觉得PID很难学。直到听了王总将的课以后，才觉得PID其实也没那么难，在课上王总把PID算法比作买米，例如当我们告诉老板要买多少斤米的时候，P算法就像是老板先估计大概的一个数，然后拿去称，当还少时就再往上加，当过多时，就减掉，尽量的接近目标值，这个过程就是P算法。但是P算法永远不能很好的接近目标值，只能接近目标值，所以要引入I算法，对于I算法就是当老板买米接近目标的时候，在这基础上进行加减，进行微调让当前值更加接近目标值，而不至于过大的调整。通过这个例子，让我对PID有了基本的认识，之后再通过《嵌入式微系统》这一本书，进一步对PID的原理有了更深的了解。其实，PID是通过误差反馈来实现的，P算法就是通过误差来做估计，快速的接近目标值，I算法通过误差来来做微调，获得平衡值以达到目标值，D算法是对于突然变换的情况，能够快速的进行补充，以实现系统更加稳定。

## 4 实现过程

首先，设定一个目标温度，然后通过PID控制PWM的输出，进而对电阻进行加热，使电阻的温度稳定在目标值的范围。PID加热的过程中需要判断当前的温度是否接近目标值，当接近目标值的时候就开启I算法进行微调，让温度慢慢的加上去，以更加准确的接近目标值，在此过程中只用到了P算法和I算法。PID控制PWM输出的端口使用OUT1,读取LM35DT电压模拟量的端口使用A0口，当读取到LM35DT电压的模拟量后，通过公式转换成温度，并显示到显示屏中。实现的过程如下图所示。

![1531981910994](C:\Users\JH\Desktop\RD总结\assets\1531981910994.png)

## 5 代码实现

### 5.1 温度转换

从LM35DT读取到的电压模拟量转换成温度。

```
acc = App.Data.Adc.A0;
data = (data*0.99f) + (acc*0.0008f);
```

### 5.2 PID控温

这里只用到PI算法来控制。当误差在3摄氏度的时候开启I算法。

```
void Pid(void)
{
	float Current;  // 当前值
    int Target;     // 目标值
    
	static int Error;  // 误差
	static int AddError;  // 累积误差
	
    static float PidOut;  // pid输出
	
    Target = App.Data.TargetTemperature;   //目标温度
    Current = App.Data.CurrentTemperature; // 当前温度

	Error = Target - Current; //误差

    if(Error < 3 && Error > -3)  // 当小于3度的时候开启i算法
    {
        AddError += Error;
        
        if (AddError > 60) 
            AddError = 60;
        else if(AddError < -60) 
            AddError = -60;
        
        PidOut = App.Data.Kp * Error + App.Data.Ki * AddError;  // pid计算公式
    }
    else
    {
        PidOut = App.Data.Kp * Error;
    }
        
	if (PidOut > 100)  
		PidOut = 100;
	else if (PidOut <= 0)   
		PidOut = 0;
	
	App.Data.PidPowerPercent = PidOut;
	
	System.Device.DO.Pwm.SetDutyRatio(PwmChannel1, App.Data.PidPowerPercent);
}
```

## 6 结果

温度能够在目标值范围2度左右震荡。

## 参考资料

