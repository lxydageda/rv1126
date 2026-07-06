#ifndef VIDEO_H
#define VIDEO_H
#include "common.h"
void vi_set();
void vi_get_frame();
void vi_register_cb();
void venc_set(IMAGE_TYPE_E imgtype,int width,int hight);
void vi_venc_bind();
void venc_register_cb();
void RGA_set();
void vi_rga_bind_register_cb();
void init_rtsp();
#endif 
