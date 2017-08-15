//
//  fastFifo.c
//  BmpGenerator
//
//  Created by Ramin on 2017-08-14.
//  Copyright © 2017 Ramin. All rights reserved.
//

/*
 * This FIFO implementation enables fast enqueue and dequeue of entities 
 * with fixed size by removing all the /if/ checks. Instead, to make sure "head"
 * and "rear" instances are available (not NULL), the "initQueue" method should
 * always be called first and before any enqueue/dequeue operation.
 * Also, note that the first dequeue call returns the "root" of the queue, which
 * is virtually enqueued as the "head" when "initQueue" method was called.
 */
 

#include "fastFifo.h"

static buffer* head = NULL;
static buffer* rear = NULL;

static int stride = 1;
static int bufferSize = 1;

buffer* initQueue(void* root){
    head = createBuffer(root);
    rear = head;
    return head;
}

int getStride(){
    return stride;
}
void setStride(int value){
    stride = value;
}

int getDataBufferSize(){
    return bufferSize;
}
void setDataBufferSize(int value){
    bufferSize = value;
}

buffer* getHead(){
    return head;
}

buffer* getRear(){
    return rear;
}

buffer* createBuffer(void* data){
    buffer* buf = (buffer*)malloc(bufferSize);
    buf->data = data;
    buf->next = NULL;
    return buf;
}

bool canReadQueue(){
    if (head == NULL)
        return false;
    else if ((rear - head) >= stride * bufferSize)
        return true;
    else
        return false;
}

void enqueue(buffer* buf){
    rear->next = buf;
    rear = buf;
}

buffer* dequeue(){
    buffer* tmp = head;
    head = tmp->next;
    return tmp;
}
