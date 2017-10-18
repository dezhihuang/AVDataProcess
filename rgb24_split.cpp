#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int BOOL;
#define TRUE  1
#define FALSE 0

BOOL rgb24_split(const char *file, int width, int height)
{
    if (file == NULL) {
        return FALSE;
    }
    
    FILE *fp   = fopen(file, "rb+");
    FILE *fp_r = fopen("./out/output_rgb24_r.y", "wb+");
    FILE *fp_g = fopen("./out/output_rgb24_g.y", "wb+");
    FILE *fp_b = fopen("./out/output_rgb24_b.y", "wb+");
    
    unsigned char *data = (unsigned char *)malloc(width*height*3);
    memset(data, 0, width*height*3);
    
    fread(data, 1, width*height*3, fp);
    
    for(int i=0; i<width*height*3; i=i+3) {
        fwrite(data+i+0, 1, 1, fp_r);
        fwrite(data+i+1, 1, 1, fp_g);
        fwrite(data+i+2, 1, 1, fp_b);
    }
    
    free(data);
    fclose(fp);
    fclose(fp_r);
    fclose(fp_g);
    fclose(fp_b);
    
    return TRUE;
}

int main(int argc, char *argv[])
{
    if (argc == 4) {
        rgb24_split(argv[1], atoi(argv[2]), atoi(argv[3]));
    }
    
    return 0;
}