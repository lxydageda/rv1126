#include "lxy_muxer.h"
void muxer_set()
{
    MUXER_CHN_ATTR_S attr;
    attr.enType = MUXER_TYPE_FLV;//封装类型
    attr.enMode = MUXER_MODE_AUTOSPLIT;//封装格式-- 自动分割
    attr.stSplitAttr.enSplitType = MUXER_SPLIT_TYPE_TIME;//按时间分割
    attr.stSplitAttr.u32TimeLenSec = 5;//5s一个视频
    attr.stSplitAttr.enSplitNameType = MUXER_SPLIT_NAME_TYPE_AUTO;
    attr.stSplitAttr.stNameAutoAttr.bTimeStampEnable = RK_TRUE;//文件命名带时间戳
    attr.stSplitAttr.stNameAutoAttr.pcBaseDir = "/mnt/sdcard";//封装视频位置  sd
    attr.stSplitAttr.stNameAutoAttr.pcPrefix = "lxy";//文件前缀
    attr.stSplitAttr.stNameAutoAttr.u16StartIdx = 0;//文件索引
    //9203_19700101000000_10.mp4

    //视频流参数
    attr.stVideoStreamParam.enCodecType = RK_CODEC_TYPE_H264;//编码格式
    attr.stVideoStreamParam.enImageType = IMAGE_TYPE_NV12;//图像格式
    attr.stVideoStreamParam.u16Fps = 30;//帧率
    attr.stVideoStreamParam.u16Level = 41;
    attr.stVideoStreamParam.u16Profile = 77;//main profile
    attr.stVideoStreamParam.u32BitRate = 1920*1080;//比特率
    attr.stVideoStreamParam.u32Width = 1920;//图像宽
    attr.stVideoStreamParam.u32Height = 1080;//图像高
    //音频流参数
    attr.stAudioStreamParam.enCodecType = RK_CODEC_TYPE_G711A;//编码格式
    attr.stAudioStreamParam.enSampFmt = RK_SAMPLE_FMT_S16;//采样格式
    attr.stAudioStreamParam.u32Channels = 1;//声道数
    attr.stAudioStreamParam.u32NbSamples = 1024;//每帧采样点个数
    attr.stAudioStreamParam.u32SampleRate = 8000;//采样率

    attr.u32MuxerId = 0;
    RK_MPI_MUXER_EnableChn(0,&attr);

    //绑定封装器和视频流
    MPP_CHN_S Vsrc;
    Vsrc.enModId = RK_ID_VENC;
    Vsrc.s32DevId = 0;
    Vsrc.s32ChnId = 2;

    MUXER_CHN_S dest;
    dest.enModId = RK_ID_MUXER;
    dest.enChnType = MUXER_CHN_TYPE_VIDEO;
    dest.s32ChnId = 0;
    RK_MPI_MUXER_Bind(&Vsrc,&dest);

    MPP_CHN_S Asrc;
    Asrc.enModId = RK_ID_AENC;
    Asrc.s32ChnId = 1;

    dest.enChnType = MUXER_CHN_TYPE_AUDIO;
    RK_MPI_MUXER_Bind(&Asrc,&dest);

    RK_MPI_MUXER_StreamStart(0);
}