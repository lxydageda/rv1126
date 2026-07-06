#ifndef AUDIO_H
#define AUDIO_H
#include "common.h"
void ai_set();
void ao_set();
void ai_ao_Bind();
void aenc_set();
void ai_aenc_Bind();
void aenc_callcb();
void adec_set();
void test_read_adec();
#endif