#include "netserve.h"
#include "audio.h"
volatile int g_server_running = 1;
void server_sig_handler(int sig)
{
    printf("接收到信号 %d，停止服务器...\n", sig);
    g_server_running = 0;
}
int serve_set()
{
    signal(SIGTERM, server_sig_handler);
    signal(SIGINT, server_sig_handler);
    ao_set();
    adec_set();
    int sfd = socket(AF_INET,SOCK_STREAM,0);
    if(sfd == -1)
    {
        perror("socket");
        return -1;
    }
    int opt = 1;
    setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR, &opt,sizeof(opt));

    struct sockaddr_in self;
    self.sin_family = AF_INET;
    self.sin_port = htons(PORT);
    self.sin_addr.s_addr = inet_addr(IP);
    int ret = bind(sfd,(struct sockaddr *)&self,sizeof(self));
    if(ret == -1)
    {
        perror("bind");
        close(sfd);
        return -1;
    }
    printf("bind success\n");
    ret = listen(sfd,1);
    if(ret == -1)
    {
        perror("listen");
        close(sfd);
        return -1;
    }
    printf("listen success\n");
    int cfd = -1;
    while(g_server_running)
    {
        cfd = accept(sfd,NULL,NULL);
        if(cfd == -1)
        {
            if(g_server_running)
            {
                perror("accept failed");              
            }
            break;
        }
        printf("客户端连接成功\n");
        MB_AUDIO_INFO_S audio_infor;
        audio_infor.enSampleFmt = RK_SAMPLE_FMT_S16;
        audio_infor.u32Channels = 2;
        audio_infor.u32NBSamples = 1024;
        audio_infor.u32SampleRate = 48000;
        char buf[4096];
        int recv_ret;
        
        while(g_server_running)
        {
            memset(buf,0,sizeof(buf));
            recv_ret = recv(cfd, buf, sizeof(buf), 0);
            if(recv_ret <= 0)
            {
                if(recv_ret == 0) 
                {
                    printf("客户端正常断开连接\n");
                } 
                else 
                {
                    perror("recv failed\n");
                }
                break;
            }
            MEDIA_BUFFER mb = RK_MPI_MB_CreateAudioBufferExt(&audio_infor,RK_FALSE,0);
            if(!mb)
            {
                printf("创建音频缓冲区失败，跳过当前帧\n");
                continue;
            }
            memcpy(RK_MPI_MB_GetPtr(mb),buf,recv_ret);
            RK_MPI_MB_SetSize(mb, recv_ret);
            int send_ret = RK_MPI_SYS_SendMediaBuffer(RK_ID_ADEC, 0, mb);
            if(send_ret != 0) 
            {
                printf("发送媒体缓冲区失败，错误码: %d\n", send_ret);
                RK_MPI_MB_ReleaseBuffer(mb);  // 发送失败时手动释放
            }
            printf("接收成功，数据大小: %d 字节\n", recv_ret);
        }
        close(cfd);
        cfd = -1;
        printf("等待新客户端连接...\n");
    }
    if(cfd != -1) 
    {
        close(cfd);
    }  // 关闭客户端连接
    close(sfd);  // 关闭监听 socket
    printf("服务器已关闭\n");
    return 0;
}