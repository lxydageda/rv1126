#include "common.h"
#include "video.h"
#include "audio.h"
FILE *fp;
void sig_fun(int mun)
{
    RK_MPI_VI_DisableChn(0,0);
    SAMPLE_COMM_ISP_Stop(0);
    exit(0);
}
int main()
{
  
    signal(2,sig_fun);
    RK_MPI_SYS_Init();
    SAMPLE_COMM_ISP_Init(0, RK_AIQ_WORKING_MODE_NORMAL, RK_FALSE, "/oem/etc/iqfiles/");
    SAMPLE_COMM_ISP_Run(0);
    SAMPLE_COMM_ISP_SetFrameRate(0, 30); 
    /* //录音频 
    ai_set();
    aenc_set();
    ai_aenc_Bind();
    aenc_callcb(); 
    //放音频*/
    ao_set();
    adec_set();
    test_read_adec(); 
/*     init_rtsp();
    vi_set();
    venc_set(IMAGE_TYPE_NV12,1920,1080);
    vi_venc_bind();
    venc_register_cb();

    ai_set();
    aenc_set();
    ai_aenc_Bind();
    aenc_callcb(); */

    while(1)
    {
        
    }
    return 0;
}