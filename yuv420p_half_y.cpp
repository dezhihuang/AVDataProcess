#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int BOOL;
#define TRUE  1
#define FALSE 0

//分离YUV420P像素数据中的亮度分量Y的数值减半，降低图像的亮度
BOOL yuv420p_half_y(const char *file, int width, int height)
{
    if (file == NULL) {
        return FALSE;
    }
    FILE *fp  = fopen(file, "rb+");
    FILE *fp0 = fopen("./out/output_420p_half_y.yuv", "wb+");
    
    unsigned char *data = (unsigned char *)malloc(width*height*3/2);
    memset(data, 0, width*height*3/2);
    
    fread(data, 1, width*height*3/2, fp);
    
    for(int i=0; i<width*height; i++) {
       data[i] = data[i] / 2; //Y分量减半
    }
    fwrite(data, 1, width*height*3/2, fp0);
    
    free(data);
    fclose(fp);
    fclose(fp0);
    
    return TRUE;
}

int main(int argc, char *argv[])
{
    if (argc == 4) {
        yuv420p_half_y(argv[1], atoi(argv[2]), atoi(argv[3]));
    }
    
    return 0;
}