//
//  gsUtil.h
//  mymatrix
//
//  Created by Ramin on 2017-08-24.
//
//

//Support for G-Streamer stuff

#ifndef gsUtil_h
#define gsUtil_h

#include <stdio.h>

/*
 playbin flags
 borrowed from playback-tutorial-1.c
 */
typedef enum {
    GST_PLAY_FLAG_VIDEO         = (1 << 0), /* We want video output */
    GST_PLAY_FLAG_AUDIO         = (1 << 1), /* We want audio output */
    GST_PLAY_FLAG_TEXT          = (1 << 2)  /* We want subtitle output */
} GstPlayFlags;

int initPipeline();
void setBuffer(unsigned char* bytes);

#endif /* gsUtil_h */
