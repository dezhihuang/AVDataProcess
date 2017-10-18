#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int BOOL;
#define TRUE  1
#define FALSE 0

//位图文件头结构体
//这个结构体的长度是固定的14个字节。
//考虑到结构体的字节对齐，将bfType单独提取出来，否则会造成该结构体为16个字节。
static unsigned short bfType = 0x4D42;//指定文件类型，必须是0x424D，
                                      //即字符串“BM”，也就是说所有.bmp文件的头两个字节都是“BM”。
                                      //'BM'表示这是Windows支持的位图格式。
typedef struct {
    unsigned int bfSize;         //指定文件大小，以字节为单位，包括这14个字节。
    unsigned short bfReserved1;  //为保留字，不用考虑
    unsigned short bfReserved2;  //为保留字，不用考虑
    unsigned int bfOffBits;      //位图文件头到数据的偏移量，以字节为单位  
}BmpHead;

//位图信息头结构体
typedef struct {
    unsigned int biSize;          //该结构大小，字节为单位，一般为40个字节 
    unsigned int biWidth;         //指定图象的宽度，单位是象素。
    unsigned int biHeight;        //指定图象的高度，单位是象素。
                                  //注：这个值除了用于描述图像的高度之外，它还有另一个用处，
                                  //就是指明该图像是倒向的位图，还是正向的位图。
                                  //如果该值是一个正数，说明图像是倒向的，如果该值是个负数，则说明图像是正向的。
                                  //大多数的BMP文件都是倒向的位图，也就是高度值是一个正数。
    unsigned short biPlanes;      //为目标设备说明颜色平面数，必须为1，不用考虑
    unsigned short biBitCount;    //颜色深度，每个象素所需要的位数
    unsigned int biCompression;   //位图的压缩类型 
    unsigned int biSizeImage;     //位图的大小，以字节为单位
    unsigned int biXPelsPerMeter; //位图水平分辨率，每米像素数
    unsigned int biYPelsPerMeter; //位图垂直分辨率，每米像素数  
    unsigned int biClrUsed;       //位图实际使用的颜色表中的颜色数  
    unsigned int biClrImportant;  //位图显示过程中重要的颜色数  
}InfoHead;

BOOL rgb24_to_bmp(const char *file, int width, int height)
{
    if (file == NULL) {
        return FALSE;
    }
        
    FILE *fpRgb24 = fopen(file, "rb");
    FILE *fpBmp   = fopen("./out/output_rgb24_to_bmp.bmp", "wb");
    
    //从文件中读取RGB数据
    unsigned char *rgb24Buf = (unsigned char *)malloc(width*height*3);
    memset(rgb24Buf, 0, width*height*3);
    fread(rgb24Buf, 1, width*height*3, fpRgb24);

    int headSize = sizeof(BmpHead) + sizeof(InfoHead) + sizeof(bfType);
    
    BmpHead      bmpHead = {0};
    InfoHead bmpInfoHead = {0};
    
    bmpHead.bfSize    = width*height*3 + headSize;
    bmpHead.bfOffBits = headSize;
    
    bmpInfoHead.biSize      = sizeof(InfoHead);
    bmpInfoHead.biWidth     = width;
    bmpInfoHead.biHeight    = -height;//BMP存储像素数据在Y轴方向从下到上。
    bmpInfoHead.biPlanes    = 1;
    bmpInfoHead.biBitCount  = 24;
    bmpInfoHead.biSizeImage = width*height*3;

    fwrite(&bfType,      1, sizeof(bfType),      fpBmp);
    fwrite(&bmpHead,     1, sizeof(bmpHead),     fpBmp);
    fwrite(&bmpInfoHead, 1, sizeof(bmpInfoHead), fpBmp);
    
    //BMP采用的是小端（Little Endian）存储方式。
    //这种存储方式中“RGB24”格式的像素的分量存储的先后顺序为B、G、R。
    //由于RGB24格式存储的顺序是R、G、B，所以需要将“R”和“B”顺序作一个调换再进行存储。
    for (int i=0; i<height; i++) {
        for (int j=0; j<width; j++) {
            unsigned char tmp = rgb24Buf[(i*width+j)*3+2];
            rgb24Buf[(i*width+j)*3+2] = rgb24Buf[(i*width+j)*3+0];
            rgb24Buf[(i*width+j)*3+0] = tmp;
        }
    }
    fwrite(rgb24Buf, width*height*3, 1, fpBmp);
    
    free(rgb24Buf);
    fclose(fpBmp);
    fclose(fpRgb24);
    
    return TRUE;
}

int main(int argc, char *argv[])
{
    if (argc == 4) {
        rgb24_to_bmp(argv[1], atoi(argv[2]), atoi(argv[3]));
    }
    
    return 0;
}