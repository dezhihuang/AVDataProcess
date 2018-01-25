
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.Formatter;

public class H264Parser {
    
    //0x00000001或0x000001是一个nalu的起始标志，遇到下一个此标志时为该nalu的结尾。
    //起始标志的后面第一个字节（type）里包含有nalu的类型，type & 0x1F即为该nalu的类型（nalu_type）。

    private static final int NALU_TYPE_SLICE    = 1;  //非IDR图像中不采用数据划分的片段，为P或B帧
    private static final int NALU_TYPE_DPA      = 2;  //非IDR图像中A类数据划分片段
    private static final int NALU_TYPE_DPB      = 3;  //非IDR图像中B类数据划分片段
    private static final int NALU_TYPE_DPC      = 4;  //非IDR图像中C类数据划分片段
    private static final int NALU_TYPE_IDR      = 5;  //IDR图像的片段，关键帧，I帧
    private static final int NALU_TYPE_SEI      = 6;  //补充增强信息
    private static final int NALU_TYPE_SPS      = 7;  //序列参数集（Sequence Parameter Set）
    private static final int NALU_TYPE_PPS      = 8;  //图像参数集PPS（Picture Parameter Set）
    private static final int NALU_TYPE_AUD      = 9;  //分割符
    private static final int NALU_TYPE_EOSEQ    = 10; //序列结束符
    private static final int NALU_TYPE_EOSTREAM = 11; //流结束符
    private static final int NALU_TYPE_FILL     = 12; //填充数据
    
    private static final int NALU_PRIORITY_DISPOSABLE = 0;
    private static final int NALU_PRIORITY_LOW        = 1;
    private static final int NALU_PRIORITY_HIGH       = 2;
    private static final int NALU_PRIORITY_HIGHEST    = 3;

    private boolean mFirstFind = true;  //第一次查找起始码
    private int mStartCodeLen = 0;      //起始码长度
    private int mCurStartCodeLen = 0;
    private int mFrameLen = 0;          //帧长度
    private int mFrameFirstByte = 0;    //帧数据中的第一个字节
    private int mCurFrameFirstByte = 0;
    private int mPos = 0;
    private int mLen = 0;
    private String mStartCode = "";
    
    private int mNaluType = 0;
    private int mForbiddenBit = 0;
    private int mNalReferenceIdc = 0;
    
    public static void main(String[] args) {
        if (args.length > 0) {
            H264Parser h264Parser = new H264Parser();
            h264Parser.ParseH264(args[0]);
        } else {
            System.out.println("Missing h264 filename.");
        }
    }
    
    //如果NALU对应的Slice为一帧的开始就用0x00000001(即4个字节长度)，否则就用0x000001(即3个字节长度)。
    public int FindStartCode(InputStream inputStream) {
        
        byte[] startCode = new byte[4];
        int len = -1;
        
        try {
            len = inputStream.read(startCode);
        } catch (Exception e) {
            e.printStackTrace();
        }
        
        if (len == 3) {
            if (startCode[0]==0 && startCode[1]==0 && startCode[2]==1) {
                len = 3;
            } else {
                len = 0;
            }
        } else if (len == 4) {
            if (startCode[0]==0 && startCode[1]==0 && startCode[2]==0 && startCode[3]==1) {
                len = 4;
            } else {
                len = 0;
            }
        }
        
        return len;
    }
    
    public int FindStartCode(List<Integer> startCodeList) {

        int len = startCodeList.size();
        
        if (len == 3) {
            if (startCodeList.get(0)==0 && startCodeList.get(1)==0 && startCodeList.get(2)==1) {
                return 3;
            }
        } else if (len == 4) {
            if (startCodeList.get(0)==0 && startCodeList.get(1)==0 && startCodeList.get(2)==0 && startCodeList.get(3)==1) {
                return 4;
            } else if (startCodeList.get(0)==0 && startCodeList.get(1)==0 && startCodeList.get(2)==1) {
                return 3;
            }
        }
        return 0;
    }
    
    public int GetNalu(InputStream inputStream) {
        mPos = mPos + mLen;
        
        //第一次查找起始码，只是为了移动文件指针
        if (mFirstFind) {
            mFirstFind = false;
            if (-1 == (mStartCodeLen = FindStartCode(inputStream))) {
                return -1;
            }
        }
        
        if (mStartCodeLen == 3) {
            mStartCode = "001";
        } else {
            mStartCode = "0001";
        }
        
        List<Integer> startCodeList = new ArrayList<>();
        int frameLen = 0;
        while(true) {
            int tmp = -1;
            
            //一个字节一个字节从文件流中读取
            try {
                tmp = inputStream.read();
            } catch (Exception e) {
                e.printStackTrace();
            }
            
            //读取到文件尾
            if (tmp == -1) {
                mFrameLen = frameLen;
                return -1;
            }
            
            //帧数据长度累加
            frameLen++;
            
            //将从文件中读取的数据保存到list中
            startCodeList.add(tmp);
            
            //从一帧的第一个字节中获取NALU信息
            if (frameLen == 1) {
                //如果上一次查找到的起始码长度是3，则上次已经保存了第一个字节的数据
                if (mStartCodeLen == 3) {
                    tmp = mFrameFirstByte;
                    frameLen++; //帧长度加上第一个字节
                }
                mCurFrameFirstByte = tmp;
                mForbiddenBit      = tmp & 0x80;
                mNalReferenceIdc   = tmp & 0x60;
                mNaluType          = tmp & 0x1f;
            }
            
            //保存四个字节到list中
            if (startCodeList.size() < 4) {
                continue;
            }
            
            //查找起始码，如果返回值大于0就表示找到了
            int startCodeLen = FindStartCode(startCodeList);
            if (startCodeLen > 0) {
                if (startCodeLen == 3) {
                    mFrameFirstByte = tmp;
                }
                frameLen = frameLen - 4;
                mLen = frameLen + mStartCodeLen;
                mStartCodeLen = startCodeLen;
                break;
            }
            startCodeList.remove(0);
        }
        mFrameLen = frameLen;
        return 0;
    }
    
    public boolean ParseH264(String fileName) {
        if (fileName.equals("")) {
            return false;
        }
        InputStream inputStream = null;
        try {
            File file = new File(fileName);
            inputStream = new FileInputStream(file);
        } catch (Exception e) {
            e.printStackTrace();
            inputStream = null;
        }
        if (inputStream == null) {
            return false;
        }
        
        System.out.println("-----+---------+----- NALU Table -+---------+------------+");  
        System.out.println(" NUM |   POS   |   IDC   |  TYPE  |   LEN   | START_CODE |");  
        System.out.println("-----+---------+---------+--------+---------+------------+"); 
        
        int iNaluNum = 0;
        while(true) {
            int dataLen = GetNalu(inputStream);
            
            iNaluNum++;
            String sNaluType = "";
            switch(mNaluType) {
                case NALU_TYPE_SLICE:    sNaluType = "SLICE";    break;
                case NALU_TYPE_DPA:      sNaluType = "DPA";      break;
                case NALU_TYPE_DPB:      sNaluType = "DPB";      break;
                case NALU_TYPE_DPC:      sNaluType = "DPC";      break;
                case NALU_TYPE_IDR:      sNaluType = "IDR";      break;
                case NALU_TYPE_SEI:      sNaluType = "SEI";      break;
                case NALU_TYPE_SPS:      sNaluType = "SPS";      break;
                case NALU_TYPE_PPS:      sNaluType = "PPS";      break;
                case NALU_TYPE_AUD:      sNaluType = "AUD";      break;
                case NALU_TYPE_EOSEQ:    sNaluType = "EOSEQ";    break;
                case NALU_TYPE_EOSTREAM: sNaluType = "EOSTREAM"; break;
                case NALU_TYPE_FILL:     sNaluType = "FILL";     break;
            }
            String sIdc = "";
            switch(mNalReferenceIdc >> 5) {
                case NALU_PRIORITY_DISPOSABLE:sIdc = "DISPOS"; break;  
                case NALU_PRIORITY_LOW:       sIdc = "LOW";    break;  
                case NALU_PRIORITY_HIGH:      sIdc = "HIGH";   break;  
                case NALU_PRIORITY_HIGHEST:   sIdc = "HIGHEST";break; 
            }
            Formatter formatter = new Formatter(System.out);
            formatter.format("%5d| %8d| %8s| %7s| %8d| %11s|\n", iNaluNum, mPos, sIdc, sNaluType, mFrameLen, mStartCode);
            if (dataLen == -1) {
                break;
            }
        }
        
        try {
            inputStream.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return true;
    }
}