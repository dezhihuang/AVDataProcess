#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//生成RGB24格式的彩条测试

int main()
{
    int width    = 640;
    int height   = 480;
    int barWidth = width/9;
    
    unsigned char *data = (unsigned char *)malloc(width*height*3);
    memset(data, 0, width*height*3);
    
    FILE *fp = fopen("./out/rgb24_colorbar_640x480.rgb", "wb+");
    
    for (int i=0; i<height; i++) {
        for (int j=0; j<width; j++) {
            int barIndex = j / barWidth;
            switch(barIndex){
                case 0://红
                    data[(i*width + j)*3+0] = 255;
                    data[(i*width + j)*3+1] = 0;
                    data[(i*width + j)*3+2] = 0;
                    break;
                case 1://橙
                    data[(i*width + j)*3+0] = 255;
                    data[(i*width + j)*3+1] = 156;
                    data[(i*width + j)*3+2] = 0;
                    break;
                case 2://黄
                    data[(i*width + j)*3+0] = 255;
                    data[(i*width + j)*3+1] = 255;
                    data[(i*width + j)*3+2] = 0;
                    break;
                case 3://绿
                    data[(i*width + j)*3+0] = 0;
                    data[(i*width + j)*3+1] = 255;
                    data[(i*width + j)*3+2] = 0;
                    break;
                case 4:// 青
                    data[(i*width + j)*3+0] = 0;
                    data[(i*width + j)*3+1] = 255;
                    data[(i*width + j)*3+2] = 255;
                    break;
                case 5://蓝
                    data[(i*width + j)*3+0] = 0;
                    data[(i*width + j)*3+1] = 0;
                    data[(i*width + j)*3+2] = 255;
                    break;
                case 6://紫
                    data[(i*width + j)*3+0] = 255;
                    data[(i*width + j)*3+1] = 0;
                    data[(i*width + j)*3+2] = 255;
                    break;
                case 7://白
                    data[(i*width + j)*3+0] = 255;
                    data[(i*width + j)*3+1] = 255;
                    data[(i*width + j)*3+2] = 255;
                    break;
                case 8://黑
                    data[(i*width + j)*3+0] = 0;
                    data[(i*width + j)*3+1] = 0;
                    data[(i*width + j)*3+2] = 0;
                    break;
                default:
                    break;
            }
        }
    }
    fwrite(data, width*height*3, 1, fp);
    
    free(data);
    fclose(fp);
    
    return 0;
}