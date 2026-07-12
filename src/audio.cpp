#include "audio.h"
extern FILE *fp;
extern rtsp_demo_handle g_rtsplive0 ;
extern rtsp_session_handle g_rtsp_session0;
extern rtsp_demo_handle g_rtsplive1 ;
extern rtsp_session_handle g_rtsp_session1;
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
    aoAttr.u32Channels = 2;
    aoAttr.u32NbSamples = 1024;
    aoAttr.u32SampleRate = 48000;
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


void aenc0_set()
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
    printf("创建音频编码通道0失败@!!!!!\n");
    }
    else
    printf("创建音频编码通道0 ok@!!!!!\n");
}
void aenc1_set()
{
    AENC_CHN_ATTR_S aencAttr;
    aencAttr.enCodecType = RK_CODEC_TYPE_G711A;
    aencAttr.u32Bitrate = 64000;
    aencAttr.u32Quality = 1;
    aencAttr.stAencG711A.u32Channels = 1;
    aencAttr.stAencG711A.u32NbSample = 1024;
    aencAttr.stAencG711A.u32SampleRate = 8000;
    if(RK_MPI_AENC_CreateChn(1,&aencAttr))
    {
    printf("创建音频编码通道1失败@!!!!!\n");
    }
    else
    printf("创建音频编码通道1 ok@!!!!!\n");
}
void ai_aenc0_Bind()
{
    MPP_CHN_S aiCHn,aencChn;
    aiCHn.enModId = RK_ID_AI;
    aiCHn.s32ChnId = 0;
    aencChn.enModId = RK_ID_AENC;
    aencChn.s32ChnId = 0;
    RK_MPI_SYS_Bind(&aiCHn,&aencChn);
}
void ai_aenc1_Bind()
{
    MPP_CHN_S aiCHn,aencChn;
    aiCHn.enModId = RK_ID_AI;
    aiCHn.s32ChnId = 0;
    aencChn.enModId = RK_ID_AENC;
    aencChn.s32ChnId = 1;
    RK_MPI_SYS_Bind(&aiCHn,&aencChn);
}
void aencOutCbFun(MEDIA_BUFFER mb)
{
    if (g_rtsplive0 && g_rtsp_session0)
    {
        rtsp_tx_audio(g_rtsp_session0,(const uint8_t *)RK_MPI_MB_GetPtr(mb), RK_MPI_MB_GetSize(mb),
                  RK_MPI_MB_GetTimestamp(mb));
        rtsp_do_event(g_rtsplive0);
    }
    if (g_rtsplive1 && g_rtsp_session1)
    {
        rtsp_tx_audio(g_rtsp_session1,(const uint8_t *)RK_MPI_MB_GetPtr(mb), RK_MPI_MB_GetSize(mb),
                  RK_MPI_MB_GetTimestamp(mb));
        rtsp_do_event(g_rtsplive1);
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
    adecAttr.stAdecG711A.u32Channels = 2;//声道数
    adecAttr.stAdecG711A.u32SampleRate = 48000;

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

void aac_dec_ao()
{
    //打开解码器
    AVCodecContext *codecContext = avcodec_alloc_context3(NULL);
    codecContext->channels = 2;//双声道
    codecContext->channel_layout = AV_CH_LAYOUT_STEREO;//声道布局
    codecContext->sample_rate = 48000;//采样率
    codecContext->frame_size = 1024;//采样点个数
    codecContext->bit_rate = 64000;//比特率
    codecContext->sample_fmt = AV_SAMPLE_FMT_S16;//采样格式

    AVCodec *codec = avcodec_find_decoder_by_name("libfdk_aac");
    if(codec == NULL){
        printf("没有fdc_aac解码器\n");
    }
    if(avcodec_open2(codecContext,codec,NULL)==0)
    {
        printf("打开解码器成功\n");
    }
    //设置ao  通道1
    ao_set();
    //循环读取aac音频 送到解码器解码， 从解码器中取到的音频送到ao
    int fd = open("/code/test.aac",O_RDONLY);

    AVFrame *frame = av_frame_alloc();
    uint8_t *buf = (uint8_t *)malloc(4096);
    AVPacket *packet = av_packet_alloc();
    packet->data = buf;
    int readCnt;

    MB_AUDIO_INFO_S audioInfor;
    audioInfor.enSampleFmt = RK_SAMPLE_FMT_S16;
    audioInfor.u32Channels = 2;
    audioInfor.u32NBSamples = 1024;
    audioInfor.u32SampleRate = 48000;
    MEDIA_BUFFER mb;
    while(1)
    {
        readCnt = ::read(fd,buf,4096);
        printf("readcnt = %d\n",readCnt);
        if(readCnt == 0)
            break;
        packet->size = readCnt;
        //送到解码器解码
        avcodec_send_packet(codecContext,packet);
        //从解码器中取到音频
        while(1)
        {
            if(avcodec_receive_frame(codecContext,frame))
                break;
            printf("解码后的数据 字节数:%d\n",frame->linesize[0]);
            mb = RK_MPI_MB_CreateAudioBufferExt(&audioInfor,RK_FALSE,0);
            memcpy(RK_MPI_MB_GetPtr(mb),frame->data[0],frame->linesize[0]);
            RK_MPI_SYS_SendMediaBuffer(RK_ID_AO,1,mb);
        }
    }
}

