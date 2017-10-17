# 视音频数据处理入门

<br />


## 分离YUV420P像素数据中的Y、U、V分量

yuv视频下载：
>http://trace.eas.asu.edu/yuv/ 
         
yuv播放器：[修改了一个YUV/RGB播放器](http://blog.csdn.net/leixiaohua1020/article/details/50466201)

yuv420_split.cpp 程序中的函数可以将YUV420P数据中的Y、U、V三个分量分离开来并保存成三个文件。

调用方法：
> ./yuv420_split lena_256x256_yuv420p.yuv 256 256

如果视频帧的宽和高分别为w和h，那么一帧YUV420P像素数据一共占用w*h*3/2 Byte的数据。其中前w*h Byte存储Y，接着的w*h*1/4 Byte存储U，最后w*h*1/4 Byte存储V。上述调用函数的代码运行后，将会把一张分辨率为256x256的名称为lena_256x256_yuv420p.yuv的YUV420P格式的像素数据文件分离成为三个文件：<br />
output_420_y.y：纯Y数据，分辨率为256x256。<br />
output_420_u.y：纯U数据，分辨率为128x128。<br />
output_420_v.y：纯V数据，分辨率为128x128。<br />

>注意：<br />
>本文中像素的采样位数一律为8bit。由于1Byte=8bit，所以一个像素的一个分量的采样值占用1Byte。<br />
>输出的U、V分量在YUV播放器中也是当做Y分量进行播放的。<br />

<br />


## 




参考：[视音频数据处理入门：RGB、YUV像素数据处理](http://blog.csdn.net/leixiaohua1020/article/details/50534150)
![](./images/leixiaohua_avDataProcess.png)
