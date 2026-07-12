#include "common.h"
#include "video.h"
#include "audio.h"
#include "lxy_muxer.h"
#include "netserve.h"
extern volatile int g_server_running;  // 声明全局变量
void *server_thread(void *arg)
{
    printf("Starting audio server thread...\n");
    serve_set();  // 调用服务器函数（阻塞）
    return NULL;
}
void sig_fun(int sig)
{
    printf("Main received signal %d\n", sig);
    g_server_running = 0; 
}
int main()
{
    signal(SIGTERM, sig_fun);
    signal(SIGINT, sig_fun);
    /* 
    RK_MPI_SYS_Init();
    SAMPLE_COMM_ISP_Init(0, RK_AIQ_WORKING_MODE_NORMAL, RK_FALSE, "/oem/etc/iqfiles/");
    SAMPLE_COMM_ISP_Run(0);
    SAMPLE_COMM_ISP_SetFrameRate(0, 30); 
    init_rtsp0();
    init_rtsp1();
    ai_set();
    vi_set();
    RGA0_set();
    RGA1_set();
    RGA2_set();
    vi_rga0_bind_register_cb();
    vi_rga1_bind_register_cb();
    vi_rga2_bind_register_cb();
    aenc0_set();
    aenc1_set();
    ai_aenc0_Bind();
    ai_aenc1_Bind();
    aenc_callcb();
    venc0_set(IMAGE_TYPE_RGB888,1920,1080);
    venc1_set(IMAGE_TYPE_RGB888,640,480);
    venc2_set(IMAGE_TYPE_RGB888,640,480);
    venc0_register_cb();
    venc1_register_cb();
    muxer_set();
 */
    pthread_t tid;
    pthread_create(&tid,NULL,server_thread,NULL);
    pthread_detach(tid); 
    while(g_server_running)
    {
        
    }
    return 0;
}