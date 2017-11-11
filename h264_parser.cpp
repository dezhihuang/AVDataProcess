/*该程序可以从H.264码流中分析得到它的基本单元NALU，并且可以简单解析NALU首部的字段。通过修改该程序可以实现不同的H.264码流处理功能*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//0x00000001或0x000001是一个nalu的起始标志，遇到下一个此标志时为该nalu的结尾。
//起始标志的后面第一个字节（type）里包含有nalu的类型，type & 0x1F即为该nalu的类型（nalu_type）。
typedef enum {
    NALU_TYPE_SLICE    = 1,  //非IDR图像中不采用数据划分的片段，为P或B帧
    NALU_TYPE_DPA      = 2,  //非IDR图像中A类数据划分片段
    NALU_TYPE_DPB      = 3,  //非IDR图像中B类数据划分片段
    NALU_TYPE_DPC      = 4,  //非IDR图像中C类数据划分片段
    NALU_TYPE_IDR      = 5,  //IDR图像的片段，关键帧，I帧
    NALU_TYPE_SEI      = 6,  //补充增强信息
    NALU_TYPE_SPS      = 7,  //序列参数集（Sequence Parameter Set）
    NALU_TYPE_PPS      = 8,  //图像参数集PPS（Picture Parameter Set）
    NALU_TYPE_AUD      = 9,  //分割符
    NALU_TYPE_EOSEQ    = 10, //序列结束符
    NALU_TYPE_EOSTREAM = 11, //流结束符
    NALU_TYPE_FILL     = 12, //填充数据
}NaluType;
//类型说明参考：http://www.cnblogs.com/lidabo/p/4481601.html



typedef enum {
    NALU_PRIORITY_DISPOSABLE = 0,
    NALU_PRIORITY_LOW        = 1,
    NALU_PRIORITY_HIGH       = 2,
    NALU_PRIORITY_HIGHEST    = 3,
}NaluPriority;

typedef struct  
{  
    int startcode_len;     //! 起始码长度，3个字节或4个字节  
    unsigned len;          //! NALU长度，不包括起始码长度
    unsigned max_size;     //! Nal Unit Buffer size  
    int forbidden_bit;     //! should be always FALSE
                           //! 禁止位：网络发现NAL单元有比特错误时可设置该比特为1，以便接收方丢掉该单元。    
    int nal_reference_idc; //! 表示NAL的优先级。0～3，取值越大，表示当前NAL越重要，需要优先受到保护。
                           //! 如果当前NAL是属于参考帧的片，或是序列参数集，或是图像参数集这些重要的单位时，
                           //! 本句法元素必需大于0。  
    int nalu_type;         //! NALU类型     
    char *buf;             //! contains the first byte followed by the EBSP  
} NALU_t;  

//如果NALU对应的Slice为一帧的开始就用0x00000001（四个字节），否则就用0x000001（三个字节）。
int findStartCode3(unsigned char *buf)
{
    if ( buf[0]!=0 || buf[1]!=0 || buf[2]!=1 ) {
        return 0; //0x000001
    } else {
        return 1;
    }
}
int findStartCode4(unsigned char *buf)
{
    if ( buf[0]!=0 || buf[1]!=0 || buf[2]!=0 || buf[3]!=1 ) {
        return 0; //0x00000001
    } else {
        return 1;
    }
}
//如果NALU对应的Slice为一帧的开始就用0x00000001(即4个字节长度)，否则就用0x000001(即3个字节长度)。
int findStartCode(const unsigned char *buf)
{
    if (strlen((const char*)buf) >= 3) {
        if ( buf[0]==0 && buf[1]==0 && buf[2]==1) {
            return 3;
        }
    }
    if (strlen((const char*)buf) >= 4) {
        if ( buf[0]==0 && buf[1]==0 && buf[2]==0 && buf[3]==1) {
            return 4;
        }
    }
    return 0;
}


//获取NALU单元
int getNalu(NALU_t *nalu, FILE *fp)
{
    if (nalu == NULL) {
        return -1;
    }
    
    unsigned char *buf = (unsigned char *)malloc(nalu->max_size);
    if (buf == NULL) {
        return 0;
    } else {
        memset(buf, 0, nalu->max_size);
    }
    
    //默认起始码长度为3个字节
    nalu->startcode_len = 3;
    
    //读取前三个字节以判断NALU的起始标志
    //如果fread返回值不等于3，则表示已到文件结尾
    if (3 != fread(buf, 1, 3, fp)) {
        free(buf);
        return 0;
    }
    
    //获取起始码长度
    int info4 = 0; //4个字节长度的起始码，一帧的开始
    int info3 = 0; //3个字节长度的起始码
    int pos = 0;   //当前位置
    
    //首先查找3字节的起始码
    info3 = findStartCode3(buf);
    if (info3 != 1) {
        //如果不是3字节长度的起始码，就再读取一个字节以判断是否为4字节长度的起始码
        if (1 != fread(buf+3, 1, 1, fp)) {
            free(buf);
            return 0;
        }
        info4 = findStartCode4(buf);
        if (info4 != 1) {
            free(buf);
            return -1;
        } else {
            pos = 4;
            nalu->startcode_len = 4;
        }
    } else {
        pos = 3;
        nalu->startcode_len = 3;
    }
    
    while (1) {
        //如果文件结束
        if (feof(fp)) { 
            nalu->len = (pos-1) - nalu->startcode_len;
            memcpy(nalu->buf, &buf[nalu->startcode_len], nalu->len);
            
            nalu->forbidden_bit     = nalu->buf[0] & 0x80; //1 bit
            nalu->nal_reference_idc = nalu->buf[0] & 0x60; //2 bit
            nalu->nalu_type         = nalu->buf[0] & 0x1f; //5 bit
            
            free(buf);
          
            return pos-1;
        }
        
        //循环读取一个字符，为了查找下一个起始码的位置
        buf[pos++] = fgetc(fp);
        
        //首先查找4字节的起始码
        info4 = findStartCode4(&buf[pos-4]);
        if (info4 != 1) {
            //如果不是4字节的起始码，再判断是否是3字节的起始码
            info3 = findStartCode3(&buf[pos-3]);
        }

        //如果查找到了起始码就退出
        if (info3==1 || info4==1) {
            break;
        }
    }
    
    //程序能走到这一步表示查找到起始码
    //文件指针往回走3/4个字节，即起始码字节长度
    int rewind = (info4==1) ? -4 : -3;
    if (0 != fseek(fp, rewind, SEEK_CUR)) {
        free(buf);
    }
    
    //NALU长度不包括起始码长度
    //当前位置减去下一个起始码长度，再减去当前起始码长度
    nalu->len = (pos+rewind) - nalu->startcode_len;
    
    //NALU内容不包括起始码
    memcpy(nalu->buf, &buf[nalu->startcode_len], nalu->len);
    
    nalu->forbidden_bit     = nalu->buf[0] & 0x80; //1000 0000，禁止位(1bit)
    nalu->nal_reference_idc = nalu->buf[0] & 0x60; //0110 0000，重要性指示位(2bit)
    nalu->nalu_type         = nalu->buf[0] & 0x1f; //0001 1111，NALU类型(5bit)
   
    free(buf);
    
    //返回NALU长度(包括起始码长度)
    return pos+rewind;
}


int h264_parser(char *file)
{
    if (file == NULL) {
        return 0;
    }
    
    //从文件中读取h264码流
    FILE *h264BitStream = fopen(file, "rb+");
    if (h264BitStream == NULL) {
        return 0;
    }
    
    NALU_t *nalu = (NALU_t *)malloc(sizeof(NALU_t));
    if (nalu == NULL) {
        return 0;
    } else {
        memset(nalu, 0, sizeof(NALU_t));
    }
    
    nalu->max_size = 100000;
    nalu->buf = (char *)malloc(100000);
    if (nalu->buf == NULL) {
        return 0;
    } else {
        memset(nalu->buf, 0, 100000);
    }
      
    printf("-----+-------- NALU Table ------+---------+\n");  
    printf(" NUM |    POS  |    IDC |  TYPE |   LEN   |\n");  
    printf("-----+---------+--------+-------+---------+\n");  
    
    int data_offset=0;  
    int nal_num=0;      //NALU计数
    
    while(!feof(h264BitStream)) {
        //分析一个NALU，返回NALU长度(包括起始码长度)
        int dataLen = getNalu(nalu, h264BitStream);
        
        char sType[20] = {0};
        switch(nalu->nalu_type) {
            case NALU_TYPE_SLICE:    sprintf(sType, "SLICE");    break;
            case NALU_TYPE_DPA:      sprintf(sType, "DPA");      break;
            case NALU_TYPE_DPB:      sprintf(sType, "DPB");      break;
            case NALU_TYPE_DPC:      sprintf(sType, "DPC");      break;
            case NALU_TYPE_IDR:      sprintf(sType, "IDR");      break;
            case NALU_TYPE_SEI:      sprintf(sType, "SEI");      break;
            case NALU_TYPE_SPS:      sprintf(sType, "SPS");      break;
            case NALU_TYPE_PPS:      sprintf(sType, "PPS");      break;
            case NALU_TYPE_AUD:      sprintf(sType, "AUD");      break;
            case NALU_TYPE_EOSEQ:    sprintf(sType, "EOSEQ");    break;
            case NALU_TYPE_EOSTREAM: sprintf(sType, "EOSTREAM"); break;
            case NALU_TYPE_FILL:     sprintf(sType, "FILL");     break;
        }
        char idc_str[20] = {0};
        switch(nalu->nal_reference_idc >> 5) {
            case NALU_PRIORITY_DISPOSABLE:sprintf(idc_str,"DISPOS"); break;  
            case NALU_PRIORITY_LOW:       sprintf(idc_str,"LOW");    break;  
            case NALU_PRIORITY_HIGH:      sprintf(idc_str,"HIGH");   break;  
            case NALU_PRIORITY_HIGHEST:   sprintf(idc_str,"HIGHEST");break;  
        }
        FILE *out = stdout;
        fprintf(out,"%5d| %8d| %7s| %6s| %8d|\n",nal_num,data_offset,idc_str,sType,nalu->len);  
        
        //NAL单元计数
        nal_num++;
        
        //数据偏移，也就是下一个NALU起始位置(起始码的位置)
        data_offset = data_offset + dataLen;
    }
    
    //释放内存
    if (nalu) {
        if (nalu->buf) {
            free(nalu->buf);
            nalu->buf = NULL;
        }
        free(nalu);
    }
    return 0;
}


int main(int argc, char *argv[])
{
    if (argc == 2) {
        h264_parser(argv[1]);
    }
    return 0;
}


