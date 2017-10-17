#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int BOOL;
#define TRUE  1
#define FALSE 0

//分离YUV420P像素数据中的Y、U、V分量
BOOL yuv420p_border(const char *file, int width, int height, int border)
{
    if (file == NULL) {
        return FALSE;
    }
    FILE *fp  = fopen(file, "rb+");   
    FILE *fp0 = fopen("./out/output_420p_border.yuv", "wb+");
    
    unsigned char *data = (unsigned char *)malloc(width*height*3/2);
    memset(data, 0, width*height*3/2);
    
    fread(data, 1, width*height*3/2, fp);
    
    //给Y分量添加边框
    for(int i=0; i<height; i++) {
        for(int j=0; j<width; j++) {
            //添加左右边框
            if (j < border) {
                data[i*width + j] = 255;
                continue;
            }
            if (j >= width-border) {
                data[i*width + j] = 255;
                continue;
            }
            
            //添加上下边框
            if (i < border) {
                data[i*width + j] = 255;
                continue;
            }
            if (i >= width-border) {
                data[i*width + j] = 255;
                continue;
            }
        }
    }

    int tmpBorder = border/2;
    int tmpW = width/2;
    int tmpH = height/2;
    
    //给U分量添加边框
    int uDataStart = width*height;
    for(int i=0; i<tmpH; i++) {
        for(int j=0; j<tmpW; j++) {
            //添加左右边框
            if (j < tmpBorder) {
                data[uDataStart + i*tmpW + j] = 255;
                continue;
            }
            if (j >= tmpW-tmpBorder) {
                data[uDataStart + i*tmpW + j] = 255;
                continue;
            }
            
            //添加上下边框
            if (i < tmpBorder) {
                data[uDataStart + i*tmpW + j] = 255;
                continue;
            }
            if (i >= tmpW-tmpBorder) {
                data[uDataStart + i*tmpW + j] = 255;
                continue;
            }
        }    
    }
    
    //给V分量添加边框
    int vDataStart = width*height*5/4;
    for(int i=0; i<tmpH; i++) {
        for(int j=0; j<tmpW; j++) {
            //添加左右边框
            if (j < tmpBorder) {
                data[vDataStart + i*tmpW + j] = 255;
                continue;
            }
            if (j >= tmpW-tmpBorder) {
                data[vDataStart + i*tmpW + j] = 255;
                continue;
            }
            
            //添加上下边框
            if (i < tmpBorder) {
                data[vDataStart + i*tmpW + j] = 255;
                continue;
            }
            if (i >= tmpW-tmpBorder) {
                data[vDataStart + i*tmpW + j] = 255;
                continue;
            }
        }    
    }
    
    fwrite(data, 1, width*height*3/2, fp0);
    
    free(data);
    fclose(fp);
    fclose(fp0);
    
    return TRUE;
}

int main(int argc, char *argv[])
{
    if (argc == 5) {
        yuv420p_border(argv[1], atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
    }
    
    return 0;
}