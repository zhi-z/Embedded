# LVGL分析

## 1 littlevgl线程

littlevgl有几个线程，作用是什么？

三个，主线程一个，和在主线程的hal_init函数中创建的另两个sdl线程。

主线程完成一系列初始化工作后，循环每10ms调用在lv_init函数中注册的三个任务，indev_proc_task，lv_refr_task，anim_task，重点关注的是每50ms处理一次的输入处理indev_proc_task和每30ms处理一次的画面刷新lv_refr_task。

monitor_sdl_refr_thread线程负责输出图像和鼠标键盘的输入处理，每次处理后sleep 50ms。

tick_thread线程每5ms增加计时变量一次，用来为全局提供时间。

 ## 2 输入检测

输入检测到图像输出的过程（以单个按钮被鼠标按下为例）

- monitor_sdl_refr_thread线程中

鼠标按下，记录鼠标位置数据

- 主线程中

​         在indev_proc_task中读取鼠标位置数据，和控件位置比较，得到控件指针，调用控件signal_func函数，此函数再调用lv_btn_set_state(btn, LV_BTN_STATE_PR)设置按钮状态为按下状态，此时画面并没有实际更新，可以看成是发出了画面更新请求。

​         在lv_refr_task中处理收到的画面更新请求，实际上是修改了画面变量tft_fb

- monitor_sdl_refr_thread线程中

​         根据变量tft_fb的值更新画面

## 3 画面刷新

```
lv_refr_task(30ms调用一次)
	lv_refr_areas
		lv_refr_area_with_vdb
			lv_refr_area_part_vdb
				lv_vdb_flush
					lv_disp_flush(vdb_act->area.x1, vdb_act->area.y1, vdb_act->area.x2, vdb_act->area.y2, vdb_act->buf);
						active->driver.disp_flush(x1, y1, x2, y2, color_p);实现画面刷新
						

active->driver.disp_flush：在初始化的时候进行注册，如下：
    lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/
    disp_drv.disp_flush = ex_disp_flush;     
```

## 参考

[littlevgl架构浅析](http://www.manongjc.com/article/77318.html)

