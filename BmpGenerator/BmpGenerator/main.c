//
//  main.c
//  BmpGenerator
//
//  Created by Ramin on 2017-08-14.
//  Copyright © 2017 Ramin. All rights reserved.
//

#include <stdio.h>
#include "fastFifo.h"

int main(int argc, const char * argv[]) {
    char* c = 'Q';
    printf("value is %d\n", (int)c);
    return 0;
    
    initQueue((void*)"root");
    for (int i=0; i<10; i++) {
        buffer* buf = createBuffer((void*)i);
        enqueue(buf);
    }
    
    buffer* head = dequeue();
    while(head && head->next){
        head = dequeue();
        int value = (int)head->data;
        printf("data is %d\n", value);
    }
    return 0;
}
