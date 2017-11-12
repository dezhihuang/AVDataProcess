//
//本程序中的函数可以通过抽象的方式将PCM16LE双声道数据的速度提高一倍。
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int pcm16le_doublespeed(const char *file)
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
    
    FILE *fp1 = fopen("./output/output_doublespeed.pcm", "wb+");
    if (fp1 == NULL) {
        printf("文件打开或创建失败！\n");
        return 0;
    }
    
    int count = 0; //采样计数
    unsigned char buf[4] = {0};
    
    while( !feof(fp) ) {
        //从文件中读取一次采样值，因为是16位的，所以需读取4个字节
        //左右声道采样值间隔存储
        //前两个字节为左声道采样值，后两个字节为右声道采样值
        fread(buf, 1, 4, fp);
        
        //只把偶数次采样值写入文件
        if (count%2 == 0) {
            //保存左声道的数据，一个采样值16位，两个字节
            fwrite(buf, 1, 2, fp1);
        
            //保存右声道的数据，一个采样值16位，两个字节
            fwrite(buf+2, 1, 2, fp1);
        }
        
        count++;
    }
    
    fclose(fp);
    fclose(fp1);
    
    return 1;
}



int main()
{
    char file[] = "./mediadata/NocturneNo2inEflat_44.1k_s16le.pcm";
    if (pcm16le_doublespeed(file)) {
        printf("操作成功！！！\n");
    } else {
        printf("操作失败！！！\n");
    }
}
