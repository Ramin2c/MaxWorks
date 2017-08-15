//
//  bmpGenerator.h
//  mylist
//
//  Created by Ramin on 2017-08-14.
//
//

#ifndef bmpGenerator_h
#define bmpGenerator_h

#include <stdio.h>

typedef struct argbColorSpace{
    long a;
    long r;
    long g;
    long b;
}argb;

void generateBitmap(argb* buffer, size_t width, size_t height);

#endif /* bmpGenerator_h */
