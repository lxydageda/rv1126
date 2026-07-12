
#ifndef COMMON_H
#define COMMON_H
extern "C"{
#include <assert.h>
#include <fcntl.h>
#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#include "common/sample_common.h"
#include "rkmedia_api.h"
#include "librtsp/rtsp_demo.h"
#include "rkmedia_venc.h"

#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}
#include <opencv2/opencv.hpp>
using namespace cv;
#endif