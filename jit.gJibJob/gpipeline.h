//
//  gpipeline.h
//  jit.gJibJob
//
//  Created by Ramin on 2017-09-08.
//
//

#ifndef gpipeline_h
#define gpipeline_h

#include <stdio.h>

//initialize and run the pipeline
int initPipeline(int argc, char *argv[]);

//stop and close the pipeline and do the necessary cleanups
void teardownPipeline();

//start sampling from the sink
void startSampling();

//stop sampling from the sink
void stopSampling();

#endif /* gpipeline_h */
