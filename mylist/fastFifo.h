//
//  fastFifo.h
//  BmpGenerator
//
//  Created by Ramin on 2017-08-14.
//  Copyright © 2017 Ramin. All rights reserved.
//

#ifndef fifo_h
#define fifo_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct structDataBuffer{
    void* data;
    struct structDataBuffer* next;
}buffer;

buffer* initQueue(void* root);
buffer* createBuffer(void* data);

bool canReadQueue();

void enqueue(buffer *buf);
buffer* dequeue();

int getStride();
void setStride(int value);

int getDataBufferSize();
void setDataBufferSize(int value);

buffer* getHead();
buffer* getRear();

#endif /* fifo_h */
