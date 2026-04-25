#ifndef ADAPTER_H
#define ADAPTER_H

//#include "video_debug.h"
#include "display.h"

typedef struct gb_s GB;

void video_init_source(GB *gb, VideoSource *out_source);
// Tehcnically this had: struct GB *gb... so .. that might break it doing a typedef struct ...
//void gb_debug_video_init_source(struct GB *gb, DebugVideoSource *out_source);

#endif