#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int BOOL;
#define TRUE  1
#define FALSE 0

//将YUV420P像素数据去掉颜色（变成灰度图）
BOOL yuv420p_gray(const char *file, int width, int height)
{
    if (file == NULL) {
        return FALSE;
    }
    FILE *fp  = fopen(file, "rb+");  
    FILE *fp0 = fopen("./out/output_420p_gray.yuv", "wb+");
    
    unsigned char *data = (unsigned char *)malloc(width*height*3/2);
    memset(data, 0, width*height*3/2);
    
    fread(data, 1, width*height*3/2, fp);
    
    //Gray
    memset(data+width*height, 128, width*height/2);
    fwrite(data, 1, width*height*3/2, fp0);
    
    
    free(data);
    fclose(fp);
    fclose(fp0);
    
    return TRUE;
}

int main(int argc, char *argv[])
{
    if (argc == 4) {
        yuv420p_gray(argv[1], atoi(argv[2]), atoi(argv[3]));
    }
    
    return 0;
}