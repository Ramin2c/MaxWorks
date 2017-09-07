//
//  gsUtil.c
//  mymatrix
//
//  Created by Ramin on 2017-08-24.
//
//

#include "gsUtil.h"
#include <gst/gst.h>

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <stdint.h>

GstElement *pipeline, *appsrc, *conv, *videosink;

int want = 1;

uint8_t b_white[384*288*4];
uint8_t b_black[384*288*4];

int width = 384;
int height = 288;
int planeCount = 4;

static void prepare_buffer() {
    
    static gboolean white = FALSE;
    static GstClockTime timestamp = 0;
    GstBuffer *buffer;
    guint size;
    GstFlowReturn ret;
    
    if (!want) return;
    want = 0;
    
    size = width * height * planeCount;
    
    buffer = gst_buffer_new_wrapped_full( 0, (gpointer)(white?b_white:b_black), size, 0, size, NULL, NULL );
    
    white = !white;
    
    GST_BUFFER_PTS (buffer) = timestamp;
    GST_BUFFER_DURATION (buffer) = gst_util_uint64_scale_int (1, GST_SECOND, 1);
    
    timestamp += GST_BUFFER_DURATION (buffer);
    
    ret = gst_app_src_push_buffer((GstAppSrc*)appsrc, buffer);
    
    if (ret != GST_FLOW_OK) {
        /* something wrong, stop pushing */
        // g_main_loop_quit (loop);
    }
}

static void cb_need_data (GstElement *appsrc, guint unused_size, gpointer user_data) {
    //prepare_buffer((GstAppSrc*)appsrc);
    want = 1;
}

int initPipeline () {
    
    GstElement *playbin;
    GstStateChangeReturn ret;
    GMainLoop *main_loop;
    GstBus *bus;
    GstMessage *msg;
    GstPlayFlags flags;
    
    /* Initialize GStreamer */
    gst_init (NULL, NULL);
    
    /* Create the elements */
    playbin = gst_element_factory_make ("playbin", "playbin");
    
    if (!playbin) {
        g_printerr ("Not all elements could be created.\n");
        return -1;
    }
    
    /* Set the URI to play */
    g_object_set (playbin, "uri", "https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm", NULL);
    //https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_cropped_multilingual.webm
    
    /* Set flags to show Audio and Video but ignore Subtitles */
    g_object_get (playbin, "flags", &flags, NULL);
    flags |= GST_PLAY_FLAG_VIDEO | GST_PLAY_FLAG_AUDIO; // switch on/off audio and video streams
    g_object_set (playbin, "flags", flags, NULL);
    bus = gst_element_get_bus(playbin);
    
    ret = gst_element_set_state (playbin, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr ("Unable to set the pipeline to the playing state.\n");
        gst_object_unref (playbin);
        return -1;
    }
    
    main_loop = g_main_loop_new (NULL, FALSE);
    g_main_loop_run (main_loop);
    
    msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
    
    /* Free resources */
    if (msg != NULL)
        gst_message_unref (msg);
    
    /* Free resources */
    g_main_loop_unref (main_loop);
    gst_object_unref (bus);
    gst_element_set_state (playbin, GST_STATE_NULL);
    gst_object_unref (playbin);
    
    return 0;
}
