#include "video.h"
rtsp_demo_handle g_rtsplive = NULL;
rtsp_session_handle g_rtsp_session;
void init_rtsp()
{
    g_rtsplive = create_rtsp_demo(554);
    g_rtsp_session = rtsp_new_session(g_rtsplive, "/9203");// rtsp://ip/9203
    //设置视频流
    rtsp_set_video(g_rtsp_session, RTSP_CODEC_ID_VIDEO_H265, NULL, 0);
    rtsp_sync_video_ts(g_rtsp_session, rtsp_get_reltime(), rtsp_get_ntptime());
    //设置音频流
    rtsp_set_audio(g_rtsp_session,RTSP_CODEC_ID_AUDIO_G711A,NULL, 0);
    rtsp_sync_audio_ts(g_rtsp_session, rtsp_get_reltime(), rtsp_get_ntptime());
}
extern FILE *fp;
long Sumsize;
int framnum;
float sec;
void vi_set()
{
    int ret;
    VI_CHN_ATTR_S viAttr;
    viAttr.pcVideoNode = "rkispp_scale0";
    viAttr.u32Width = 1920;
    viAttr.u32Height = 1080;
    viAttr.enPixFmt = IMAGE_TYPE_NV12;
    viAttr.u32BufCnt = 3;
    viAttr.enBufType = VI_CHN_BUF_TYPE_MMAP;
    viAttr.enWorkMode = VI_WORK_MODE_NORMAL;
    ret = RK_MPI_VI_SetChnAttr(0,0,&viAttr);
    if(ret)
    {
        printf("设置vi通道属性失败\n");
        return;
    }
    printf("设置vi通道属性成功\n");
    ret = RK_MPI_VI_EnableChn(0,0);
    if(ret)
    {
        printf("设置vi通道使能失败\n");
        return;
    }
    printf("设置vi通道使能成功\n");
    ret = RK_MPI_VI_StartStream(0,0);
    if(ret)
    {
        printf("启动vi通道失败\n");
        return;
    }
    printf("启动vi通道成功\n");
}
void vi_get_frame()
{
    MEDIA_BUFFER buf =  RK_MPI_SYS_GetMediaBuffer(RK_ID_VI,0,-1);
    fwrite(RK_MPI_MB_GetPtr(buf),RK_MPI_MB_GetSize(buf),1,fp);
    printf("获取到的画面的大小：%d\n",RK_MPI_MB_GetSize(buf));
}

void vi_OutCbFunc(MEDIA_BUFFER mb)
{
    printf("获取到的画面的大小：%d\n",RK_MPI_MB_GetSize(mb));
    
}
void vi_register_cb()
{
    MPP_CHN_S ch;
    ch.enModId =RK_ID_VI;
    ch.s32ChnId = 0;
    ch.s32DevId = 0;
    RK_MPI_SYS_RegisterOutCb(&ch,vi_OutCbFunc);
}

void venc_set(IMAGE_TYPE_E imgtype,int width,int hight)
{
    VENC_CHN_ATTR_S vencAttr;
    //编码器属性
    vencAttr.stVencAttr.enType = RK_CODEC_TYPE_H265;//编码格式
    vencAttr.stVencAttr.imageType = imgtype;//图像格式
    vencAttr.stVencAttr.u32VirWidth = width;//stride宽度
    vencAttr.stVencAttr.u32VirHeight = hight;//stride高度
    vencAttr.stVencAttr.u32Profile = 77;// main profile
    vencAttr.stVencAttr.u32PicWidth = width;//编码图像宽度
    vencAttr.stVencAttr.u32PicHeight = hight;//编码图像高度
    //码率控制器数据
    vencAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265VBR;//动态码率
    vencAttr.stRcAttr.stH265Vbr.u32Gop = 30;//I帧间隔
    vencAttr.stRcAttr.stH265Vbr.u32SrcFrameRateDen = 1;
    vencAttr.stRcAttr.stH265Vbr.u32SrcFrameRateNum = 30;//数据源帧率30fps
    vencAttr.stRcAttr.stH265Vbr.fr32DstFrameRateDen = 1;
    vencAttr.stRcAttr.stH265Vbr.fr32DstFrameRateNum = 30;//目标帧率30fps
    vencAttr.stRcAttr.stH265Vbr.u32MaxBitRate = width*hight;
    if(RK_MPI_VENC_CreateChn(0,&vencAttr))
    {
        printf("创建编码通道失败\n");
    }
    else
        printf("创建编码通道ok！！！！\n");
}


void vi_venc_bind()
{
    MPP_CHN_S src,dest;
    src.enModId = RK_ID_VI;
    src.s32DevId = 0;
    src.s32ChnId = 0;

    dest.enModId = RK_ID_VENC;
    dest.s32DevId = 0;
    dest.s32ChnId = 0;
    //将vi和venc绑定，vi采集到的数据 自动送到venc
    RK_MPI_SYS_Bind(&src,&dest);
}

//编码回调  编码成功一帧画面调用一次回调函数
void venc_OutCbFunc(MEDIA_BUFFER mb)
{
    /* fwrite(RK_MPI_MB_GetPtr(mb),RK_MPI_MB_GetSize(mb),1,fp);
    Sumsize+=RK_MPI_MB_GetSize(mb);framnum+=1;sec = framnum/30;
    printf("图像大小:%d\n",RK_MPI_MB_GetSize(mb));
    printf("总大小：%ld,目前帧数：%d,记录时长：%.2f\n",Sumsize,framnum,sec);
    if(sec >= 10)
    {
        exit(0);
    }*/
    if (g_rtsplive && g_rtsp_session) 
    {
        rtsp_tx_video(g_rtsp_session,(const uint8_t *)RK_MPI_MB_GetPtr(mb), RK_MPI_MB_GetSize(mb),
                  RK_MPI_MB_GetTimestamp(mb));
        rtsp_do_event(g_rtsplive);
    }
    RK_MPI_MB_ReleaseBuffer(mb); 

}

void venc_register_cb()
{
    MPP_CHN_S ch;
    ch.enModId = RK_ID_VENC;
    ch.s32DevId = 0;
    ch.s32ChnId = 0;
    RK_MPI_SYS_RegisterOutCb(&ch,venc_OutCbFunc);
}
void RGA_set()
{
    RGA_ATTR_S rgaAttr;
    //输入图像信息
    rgaAttr.stImgIn.imgType = IMAGE_TYPE_NV12;//输入图像格式
    rgaAttr.stImgIn.u32X = 0;
    rgaAttr.stImgIn.u32Y = 0;
    rgaAttr.stImgIn.u32Width = 1920;//输入图像宽
    rgaAttr.stImgIn.u32Height = 1080;//输入图像高
    rgaAttr.stImgIn.u32HorStride = 1920;
    rgaAttr.stImgIn.u32VirStride = 1080;
    //输出图像信息
    rgaAttr.stImgOut.imgType = IMAGE_TYPE_BGR888;//输出图像信息
    rgaAttr.stImgOut.u32X = 0;
    rgaAttr.stImgOut.u32Y = 0;
    rgaAttr.stImgOut.u32Width = 640;
    rgaAttr.stImgOut.u32Height = 480;
    rgaAttr.stImgOut.u32HorStride = 640;
    rgaAttr.stImgOut.u32VirStride = 480;
    rgaAttr.bEnBufPool = RK_TRUE;//使用缓存池
    rgaAttr.u16BufPoolCnt = 3;//缓存数量
    rgaAttr.enFlip = RGA_FLIP_NULL;//镜像
    rgaAttr.u16Rotaion = 0;//旋转
    if(RK_MPI_RGA_CreateChn(0,&rgaAttr))
    {
    printf("创建 RGA 失败\n");
    }
    else
    printf("创建 RGA 通道成功\n");
}
void rga_OutCbFunc(MEDIA_BUFFER mb)
{
 RK_MPI_MB_BeginCPUAccess(mb,RK_FALSE);
 Mat src(480,640,CV_8UC3,RK_MPI_MB_GetPtr(mb));
 putText(src,"lxy666",Point(10,200),FONT_HERSHEY_PLAIN,2,Scalar(255,0,0),3);
 RK_MPI_MB_EndCPUAccess(mb,RK_FALSE);
 //把数据送到编码器
 RK_MPI_SYS_SendMediaBuffer(RK_ID_VENC,0,mb);
 RK_MPI_MB_ReleaseBuffer(mb);
}
void vi_rga_bind_register_cb()
{
 MPP_CHN_S vi_chn,rga_chn;
 vi_chn.enModId = RK_ID_VI;
 vi_chn.s32DevId = 0;
 vi_chn.s32ChnId = 0;
 rga_chn.enModId = RK_ID_RGA;
 rga_chn.s32DevId = 0;
 rga_chn.s32ChnId = 0;
 RK_MPI_SYS_Bind(&vi_chn,&rga_chn);
 RK_MPI_SYS_RegisterOutCb(&rga_chn,rga_OutCbFunc);
}

