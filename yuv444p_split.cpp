#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int BOOL;
#define TRUE  1
#define FALSE 0

BOOL yuv444p_split(const char *file, int width, int height)
{
    if (file == NULL) {
        return FALSE;
    }
    FILE *fp  = fopen(file, "rb+");
    
    FILE *fp0 = fopen("./out/output_444p_yuv.y", "wb+");
    FILE *fp1 = fopen("./out/output_444p_y.y",   "wb+");
    FILE *fp2 = fopen("./out/output_444p_u.y",   "wb+");
    FILE *fp3 = fopen("./out/output_444p_v.y",   "wb+");
    
    unsigned char *data = (unsigned char *)malloc(width*height*3);
    memset(data, 0, width*height*3);
    
    fread(data, 1, width*height*3, fp);
    
    fwrite(data,                1, width*height*3, fp0);
    fwrite(data,                1, width*height,   fp1); //Y
    fwrite(data+width*height,   1, width*height,   fp2); //U
    fwrite(data+width*height*2, 1, width*height,   fp3); //V
    
    free(data);
    fclose(fp);
    fclose(fp1);
    fclose(fp2);
    fclose(fp3);
    
    return TRUE;
}

int main(int argc, char *argv[])
{
    if (argc == 4) {
        yuv444p_split(argv[1], atoi(argv[2]), atoi(argv[3]));
    }
    
    return 0;
}