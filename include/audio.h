#ifndef AUDIO_H
#define AUDIO_H
#include "common.h"
void ai_set();
void ao_set();
void ai_ao_Bind();
void aenc0_set();
void aenc1_set();
void ai_aenc0_Bind();
void ai_aenc1_Bind();
void aenc_callcb();
void adec_set();
void test_read_adec();
void aac_dec_ao();
#endif