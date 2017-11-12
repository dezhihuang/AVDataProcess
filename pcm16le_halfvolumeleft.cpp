//
//本程序中的函数可以将PCM16LE双声道数据中左声道的音量降低一半。
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int pcm16le_halfvolumeleft(const char *file)
{
    if (file == NULL) {
        printf("原始文件为空！\n");
        return 0;
    }
    
    FILE *fp = fopen(file, "rb+");
    if (fp == NULL) {
        printf("原始文件打开失败！\n");
        return 0;
    }
    
    FILE *fp1 = fopen("./output/output_halfleft.pcm", "wb+");
    if (fp1 == NULL) {
        printf("文件打开或创建失败！\n");
        return 0;
    }
    
    unsigned char buf[4] = {0};
    
    while(!feof(fp)) {
        //从文件中读取一次采样值，因为是16位的，所以需读取4个字节
        //左右声道采样值间隔存储
        //前两个字节为左声道采样值，后两个字节为右声道采样值
        fread(buf, 1, 4, fp);
        
        //将前两个字节(左声道采样值)强制转换为 short类型
        short *sample = (short *)buf;
        
        //将左声道采样值减半
        *sample = *sample / 2;
        
        //将减半的左声道采样值写入文件
        fwrite(sample, 1, 2, fp1);
        
        //将原始的右声道采样值写入文件
        fwrite(buf+2, 1, 2, fp1);
    }
    
    fclose(fp);
    fclose(fp1);
    
    return 1;
}

int main()
{
    char file[] = "./mediadata/NocturneNo2inEflat_44.1k_s16le.pcm";
    if (pcm16le_halfvolumeleft(file)) {
        printf("操作成功！！！\n");
    } else {
        printf("操作失败！！！\n");
    }
}