//
//  bmpGenerator.c
//  mylist
//
//  Created by Ramin on 2017-08-14.
//
//

#include "bmpGenerator.h"
#include <stdio.h>
#include <CGBitmapContext.h>
#include <ImageIO/ImageIO.h>
#include <UTCoreTypes.h>


void generateBitmap(argb* buffer, size_t width, size_t height){
    char* rgba = (char*)malloc(width*height*4);
    for(int i=0; i < width*height; ++i) {
        rgba[4*i] = buffer[i].r;
        rgba[4*i+1] = buffer[i].g;
        rgba[4*i+2] = buffer[i].b;
        rgba[4*i+3] = buffer[i].a;
    }
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef bitmapContext = CGBitmapContextCreate(
                                                       rgba,
                                                       width,
                                                       height,
                                                       8, // bitsPerComponent
                                                       4*width, // bytesPerRow
                                                       colorSpace,
                                                       kCGImageAlphaNoneSkipLast);
    
    CFRelease(colorSpace);
    
    CGImageRef cgImage = CGBitmapContextCreateImage(bitmapContext);
    CFURLRef url = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, CFSTR("/Users/Ramin/Desktop/image.png"), kCFURLPOSIXPathStyle, false);
    
    CFStringRef type = kUTTypePNG; // or kUTTypeBMP if you like
    CGImageDestinationRef dest = CGImageDestinationCreateWithURL(url, type, 1, 0);
    
    CGImageDestinationAddImage(dest, cgImage, 0);
    
    CFRelease(cgImage);
    CFRelease(bitmapContext);
    CGImageDestinationFinalize(dest);
    free(rgba);
}
