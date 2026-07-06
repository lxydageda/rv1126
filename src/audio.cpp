#include "audio.h"
extern FILE *fp;
extern rtsp_demo_handle g_rtsplive ;
extern rtsp_session_handle g_rtsp_session;
void ai_set()
{
    int ret;
    AI_CHN_ATTR_S aiAttr;
    aiAttr.pcAudioNode = "default:CARD=Device";
    aiAttr.enSampleFormat = RK_SAMPLE_FMT_S16;
    aiAttr.u32Channels = 1;
    aiAttr.u32SampleRate = 8000;
    aiAttr.u32NbSamples = 1024;
    aiAttr.enAiLayout = AI_LAYOUT_NORMAL;
    if(RK_MPI_AI_SetChnAttr(0,&aiAttr))
    {
        printf("ai通道配置失败\n");
    }
    printf("ai通道配置成功\n");
    RK_MPI_AI_EnableChn(0);
    RK_MPI_AI_StartStream(0);//如果绑定可以省略
    printf("ok了\n");
}
void ao_set()
{
    AO_CHN_ATTR_S aoAttr;
    aoAttr.pcAudioNode = "default:CARD=rockchipi2s0sou";
    aoAttr.enSampleFormat = RK_SAMPLE_FMT_S16;
    aoAttr.u32Channels = 1;
    aoAttr.u32NbSamples = 1024;
    aoAttr.u32SampleRate = 8000;
    RK_MPI_AO_SetChnAttr(1,&aoAttr);
    RK_MPI_AO_EnableChn(1);
}
void ai_ao_Bind()
{
    MPP_CHN_S ai,ao;
    ai.s32ChnId = 0;
    ai.enModId = RK_ID_AI;
    ao.s32ChnId = 1;
    ao.enModId = RK_ID_AO;
    RK_MPI_SYS_Bind(&ai,&ao);
}


void aenc_set()
{
    AENC_CHN_ATTR_S aencAttr;
    aencAttr.enCodecType = RK_CODEC_TYPE_G711A;
    aencAttr.u32Bitrate = 64000;
    aencAttr.u32Quality = 1;
    aencAttr.stAencG711A.u32Channels = 1;
    aencAttr.stAencG711A.u32NbSample = 1024;
    aencAttr.stAencG711A.u32SampleRate = 8000;
    if(RK_MPI_AENC_CreateChn(0,&aencAttr))
    {
    printf("创建音频编码通道失败@!!!!!\n");
    }
    else
    printf("创建音频编码通道 ok@!!!!!\n");
}
void ai_aenc_Bind()
{
    MPP_CHN_S aiCHn,aencChn;
    aiCHn.enModId = RK_ID_AI;
    aiCHn.s32ChnId = 0;
    aencChn.enModId = RK_ID_AENC;
    aencChn.s32ChnId = 0;
    RK_MPI_SYS_Bind(&aiCHn,&aencChn);
}

void aencOutCbFun(MEDIA_BUFFER mb)
{
    /* fwrite(RK_MPI_MB_GetPtr(mb),RK_MPI_MB_GetSize(mb),1,fp);
    printf("编码音频:%d\n",RK_MPI_MB_GetSize(mb)); */
    if (g_rtsplive && g_rtsp_session)
    {
        rtsp_tx_audio(g_rtsp_session,(const uint8_t *)RK_MPI_MB_GetPtr(mb), RK_MPI_MB_GetSize(mb),
                  RK_MPI_MB_GetTimestamp(mb));
        rtsp_do_event(g_rtsplive);
    }
    RK_MPI_MB_ReleaseBuffer(mb);
}
void aenc_callcb()
{
    MPP_CHN_S aencChn;
    aencChn.enModId = RK_ID_AENC;
    aencChn.s32ChnId = 0;
    RK_MPI_SYS_RegisterOutCb(&aencChn,aencOutCbFun);
}

void adec_set()
{
    ADEC_CHN_ATTR_S adecAttr;
    adecAttr.enCodecType = RK_CODEC_TYPE_G711A;//编码格式
    adecAttr.stAdecG711A.u32Channels = 1;//声道数
    adecAttr.stAdecG711A.u32SampleRate = 8000;

    if(RK_MPI_ADEC_CreateChn(0,&adecAttr) == 0)
    printf("创建解码通道成功\n");

    MPP_CHN_S src,dest;
    src.enModId = RK_ID_ADEC;
    src.s32ChnId = 0;
    dest.enModId = RK_ID_AO;
    dest.s32ChnId = 1;
    RK_MPI_SYS_Bind(&src,&dest);
}

void test_read_adec()
{   
    int fd = open("lxyaudio",O_RDONLY);
    MB_AUDIO_INFO_S infor;
    infor.u32Channels = 1;
    infor.u32NBSamples = 1024;
    infor.u32SampleRate = 8000;
    infor.enSampleFmt = RK_SAMPLE_FMT_S16;
    MEDIA_BUFFER mb;
    while(1)
    {
        mb = RK_MPI_MB_CreateAudioBufferExt(&infor,RK_FALSE,0);
        if(::read(fd,RK_MPI_MB_GetPtr(mb),RK_MPI_MB_GetSize(mb)) == 0)
        {
            printf("读到文件末尾!!!!\n");
            break;
        }
    }
    RK_MPI_SYS_SendMediaBuffer(RK_ID_ADEC,0,mb);
    RK_MPI_MB_ReleaseBuffer(mb);
}
   
