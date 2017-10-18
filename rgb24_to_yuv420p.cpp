#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int BOOL;
#define TRUE  1
#define FALSE 0

unsigned char clip_value(unsigned char x, unsigned char min, unsigned char max)
{
    if (x > max) {
        return max;
    }
    if (x < min) {
        return min;
    }
    return x;
}


//将RGB24格式像素数据转换为YUV420P格式像素数据
BOOL rgb24_to_yuv420p(const char *file, int width, int height)
{
    if (file == NULL) {
        return FALSE;
    }
    
    FILE *fpRgb24   = fopen(file, "rb+");
    FILE *fpYuv420p = fopen("./out/rgb24_to_yuv420p.yuv", "wb+");
    
    unsigned char *rgb24Buf   = (unsigned char *)malloc(width*height*3);
    memset(rgb24Buf, 0, width*height*3);
    unsigned char *yuv420pBuf = (unsigned char *)malloc(width*height*3/2);
    memset(rgb24Buf, 0, width*height*3/2);
    
    //读取RGB图像数据
    fread(rgb24Buf, 1, width*height*3, fpRgb24);
    
    //初始化Y、U、V像素数据内存起始位置
    unsigned char *ptrY = yuv420pBuf;
    unsigned char *ptrU = ptrY + width*height;
    unsigned char *ptrV = ptrU + width*height/4; 
    
    for (int i=0; i<height; i++) {
        
        //读取一行RGB数据
        unsigned char *ptrRGB = rgb24Buf + width*i*3;
        
        for (int j=0; j<width; j++) {
            unsigned char r = *(ptrRGB++);
            unsigned char g = *(ptrRGB++);
            unsigned char b = *(ptrRGB++);
            
            //RGB像素数据向YUV转换
            //这个公式没看懂！！！
            unsigned char y = (unsigned char)((66*r  + 129*g + 25*b  + 128) >> 8) + 16;
            unsigned char u = (unsigned char)((-38*r - 74*g  + 112*b + 128) >> 8) + 128;
            unsigned char v = (unsigned char)((112*r - 94*g  - 18*b  + 128) >> 8) + 128;
            
            *(ptrY++) = clip_value(y, 0, 255);
            
            //U、V在水平和垂直方向的取样数是Y的一半
            if (i%2==0 && j%2==0) {
                *(ptrU++) = clip_value(u, 0, 255);
                *(ptrV++) = clip_value(v, 0, 255);
            }
        }
    }
    fwrite(yuv420pBuf, 1, width*height*3/2, fpYuv420p);
    
    free(rgb24Buf);
    free(yuv420pBuf);
    fclose(fpRgb24);
    fclose(fpYuv420p);
    
    return TRUE;
}

int main(int argc, char *argv[])
{
    if (argc == 4) {
        rgb24_to_yuv420p(argv[1], atoi(argv[2]), atoi(argv[3]));
    }
    
    return 0;
}