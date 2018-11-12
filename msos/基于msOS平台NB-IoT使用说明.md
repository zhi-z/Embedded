# 基于msOS平台NB-IoT使用说明

## 1 NB-IoT代码说明

如图所示，代码结构

![1533605583085](C:\Users\JH\AppData\Local\Temp\1533605583085.png)

在Nbiot文件夹下中共有6个c文件，每个c文件对于一个h文件。这几个文件是一层一层网上进行封装的。

- nbiot.c：单个AT命令发送，把每个AT命令封装成一个函数；
- nbiot_api.c ：对上一层的AT命令进行封装，有打开或关闭NB-IoT等函数；
- nbiot_buffer.c：数据缓冲区；
- nbiot_protocol.c：发送数据指定的协议，在这个文件中对发送数据进行初始化。在对应的.h文件中可以修改或者添加需要发送数据，对于需要发送的数据在这里封装成结构体，当想要发送数据时，直接对结构体赋值即可；
- nbiot_toplayer_api.c：对AT命令更进一步封装，属于顶层封装，在这个文件中可以直接调用函数打开设备和发送数据，对于NB-IoT初始化也是在这个文件中；
- tool_utils.c：数据转换工具。

## 2 数据发送到电信服务器

发送数据步骤：

1. 上电后先进行初始化：初始化函数在nbiot_toplayer_api.c中，直接调用进行初始化即可；
2. 数据打包：例如，在这里定义了一个结构体，这里包含着需要发送的数据，当要发送数据时候只需要对以下结构体进行赋值即可。也可以根据场景的需要对发送数据结构体进行修改。这里默认可以发送91个字节数据。

```
typedef struct _MessageStruct
{
	char	index[4];		//记录发送消息的数量，计满清零“0000”-“9999”
	DeviceStruct	device_info;
	ModuleStruct	module_info;
	ParameterStruct2	param_info;
	SensorStruct	sensor_info;
}MessageStruct;
```

3. NB-IoT配置：在nbiot_config.h文件中，一般需要修改发送到IoT平台的地址和端口，在这里也设置了一个默认的地址，如图所示：

![1533607743848](E:\GitHub\work\assets\1533607743848.png)

4. 接着发送数据前需要先打开NB-IoT，在msOS系统中已经定义好了接口，直接用如下代码打开：

```
System.Device.Nbiot.Open();
```

5. 最后就是发送数据，同样直接在调用接口就可以直接发送，实现数据发送的代码如下：

```
System.Device.Nbiot.SendMessage();
```

- **注意**：这里数据发送的协议使用的是CoAP协议，在发送数据之前需要在电信服务器上注册设备才能进行发送。
- 调试：可以通过打开串口助手查看数据发送是否成功，如果发送成功会返回OK。