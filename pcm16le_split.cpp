//
//本程序中的函数可以将PCM16LE双声道数据中左声道和右声道的数据分离成两个文件。
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int pcm16le_split(const char *file)
{
    if (file == NULL) {
        printf("文件路径为空！\n");
        return 0;
    }
    
    FILE *fp = fopen(file, "rb+");
    if (fp == NULL) {
        printf("文件打开失败！\n");
        return 0;
    }
    
    FILE *fp_l = fopen("./output/output_l.pcm", "wb+");
    if (fp_l == NULL) {
        printf("左声道文件打开或创建失败！\n");
        return 0;
    }
    
    FILE *fp_r = fopen("./output/output_r.pcm", "wb+");
    if (fp_r == NULL) {
        printf("右声道文件打开或创建失败！\n");
        return 0;
    }
    
    unsigned char buf[4] = {0};
    
    //PCM16LE双声道数据中左声道和右声道的采样值是间隔存储的。
    //每个采样值占用2Byte空间。
    while (!feof(fp)) {
        fread(buf, 1, 4, fp);
        
        //保存左声道的数据，一个采样值16位，两个字节
        fwrite(buf, 1, 2, fp_l);
        
        //保存右声道的数据
        fwrite(buf+2, 1, 2, fp_r);
    }
    
    fclose(fp);
    fclose(fp_l);
    fclose(fp_r);
    
    return 1;
} 

int main()
{
    char file[] = "./mediadata/NocturneNo2inEflat_44.1k_s16le.pcm";
    if (pcm16le_split(file)) {
        printf("操作成功！！！\n");
    } else {
        printf("操作失败！！！\n");
    }
}
