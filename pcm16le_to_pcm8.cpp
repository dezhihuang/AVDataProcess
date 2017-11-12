//
//本程序中的函数可以通过计算的方式将PCM16LE双声道数据16bit的采样位数转换为8bit。
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int pcm16le_to_pcm8(const char *file)
{
    if (file == NULL) {
        printf("原始PCM文件为空！\n");
        return 0;
    }
    
    FILE *fp = fopen(file, "rb+");
    if (fp == NULL) {
        printf("原始PCM文件打开失败！\n");
        return 0;
    }
    
    FILE *fp1 = fopen("./output/pcm16le_to_pcm8.pcm", "wb+");
    if (fp1 == NULL) {
        printf("文件打开或创建失败！\n");
        return 0;
    }
    
    unsigned char buf[4] = {0};
    
    while ( !feof(fp) ) {
        //从文件中读取一次采样值，因为是16位的，所以需读取4个字节
        //左右声道采样值间隔存储,前两个字节为左声道采样值，后两个字节为右声道采样值
        fread(buf, 1, 4, fp);
        
        //将前两个字节(左声道采样值)强制转换为 short类型，因为short类型长度为两个字节
        short *sample = (short *)buf;
        
        //右移8位，相当于除以256(2的8次方)
        //将pcm16(short类型)的值以256为除数取模，作为pcm8的采样值
        unsigned char pcm8 = (*sample) >> 8;
        
        //因为short类型的范围为-32768~32767，经过上一步获得的结果为-128~127
        //所以转成unsigned char需要加上128，unsigned char类型的范围为0~255
        pcm8 = pcm8 + 128;
        
        //写入左声道的采样值
        fwrite(&pcm8, 1, 1, fp1);
        
        //将前两个字节(右声道采样值)强制转换为 short类型
        sample = (short *)(buf + 2);
        
        pcm8 = (*sample) >> 8;
        
        //-128~127 => 0~128
        pcm8 = pcm8 + 128;
        
        //写入右声道的采样值
        fwrite(&pcm8, 1, 1, fp1);
    }
    
    fclose(fp);
    fclose(fp1);
    
    return 1;
}


int main()
{
    char file[] = "./mediadata/NocturneNo2inEflat_44.1k_s16le.pcm";
    if (pcm16le_to_pcm8(file)) {
        printf("操作成功！\n");
    } else {
        printf("操作失败！\n");
    }
    
    return 0;
}