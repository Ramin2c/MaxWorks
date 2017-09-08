//
//  gpipeline.c
//  jit.gJibJob
//
//  Created by Ramin on 2017-09-08.
//
//

#include "gpipeline.h"
#include <gst/gst.h>
#include <glib.h>
#include <gst/app/gstappsink.h>

GMainLoop *loop;
GstElement *pipeline, *videoSrc, *queue, *appSink; //might be needed *capsFilter, *videoConvert,
GstCaps *caps;

static void on_queue_overrun (/* GstQueue *queue */ void *queue, gpointer user_data);
static GstFlowReturn on_new_sample_from_sink (GstAppSink *sink, gpointer data);

int initPipeline(int argc, char *argv[]){
    gst_init(&argc, &argv);
    loop = g_main_loop_new(NULL, FALSE);
    
    pipeline = gst_pipeline_new("pipeline");
    videoSrc = gst_element_factory_make("videotestsrc", "source");
    appSink = gst_element_factory_make("appsink", "appsink");
    queue = gst_element_factory_make("queue", "q1");
    
    if(!pipeline || !videoSrc || !queue || !appSink){
        //TODO: log errors properly
        return -1;
    }
    
    gst_bin_add_many(GST_BIN(pipeline), videoSrc, queue, appSink, NULL);
    if(!gst_element_link_many(videoSrc, queue, appSink, NULL)){
        //TODO: log errors properly
        return -1;
    }
    
    g_object_set(videoSrc, "pattern", 13, NULL);
    
    g_signal_connect(appSink, "new-sample", G_CALLBACK(on_new_sample_from_sink), NULL);
    
#ifdef DEBUG
    g_object_set(queue, "silent", TRUE, NULL);
    g_signal_connect(queue, "overrun", G_CALLBACK(on_queue_overrun), NULL);
#endif
    
    guint maxSizeBuffers, maxSizeBytes;
    guint64	maxSizeTime;
    
    //TODO: find good values for max-size-*
    unsigned short factor = 2;
    g_object_get(queue, "max-size-buffers", &maxSizeBuffers, "max-size-bytes", &maxSizeBytes, "max-size-time", &maxSizeTime, NULL);
    g_object_set(queue, "max-size-buffers", maxSizeBuffers * factor, "max-size-bytes", maxSizeBytes * factor, "max-size-time", maxSizeTime * factor * factor, NULL);
    
    /* Start playing */
    GstStateChangeReturn ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE){
        //TODO: log errors properly
        return -1;
    }
    g_main_loop_run(loop);
    return 0;
}

//TODO: review for other necessary cleanups
void teardownPipeline(){
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);
}

void startSampling(){
    g_object_set(appSink, "emit-signals", TRUE, NULL);
}

void stopSampling(){
    g_object_set(appSink, "emit-signals", FALSE, NULL);
}

static void on_queue_overrun (/* GstQueue *queue */ void *queue, gpointer user_data);
static GstFlowReturn on_new_sample_from_sink (GstAppSink *sink, gpointer data){
    return GST_FLOW_OK;
}


