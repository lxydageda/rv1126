#ifndef VIDEO_H
#define VIDEO_H
#include "common.h"

void vi_set();
void vi_get_frame();
void vi_register_cb();
void venc0_set(IMAGE_TYPE_E imgtype,int width,int hight);
void venc1_set(IMAGE_TYPE_E imgtype,int width,int hight);
void venc2_set(IMAGE_TYPE_E imgtype,int width,int hight);
void vi_venc_bind();
void venc0_register_cb();
void venc1_register_cb();
void RGA0_set();
void RGA1_set();
void RGA2_set();
void vi_rga0_bind_register_cb();
void vi_rga1_bind_register_cb();
void vi_rga2_bind_register_cb();
void init_rtsp0();
void init_rtsp1();
#endif 
