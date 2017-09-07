//
//  mymatrix.c
//  jit.mymatrix
//
//  Created by Ramin on 2017-07-28.
//
//


#include "jit.common.h"
#include "gsUtil.h"
#include <stdio.h>
#include "fifo.h"


void writeBitmaptoMemory(unsigned char* buffer, size_t width, size_t height){
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef bitmapContext = CGBitmapContextCreate(
                                                       buffer,
                                                       width,
                                                       height,
                                                       8, // bitsPerComponent
                                                       4*width, // bytesPerRow
                                                       colorSpace,
                                                       kCGImageAlphaPremultipliedFirst);
    
    CFRelease(colorSpace);
    
    CGImageRef cgImage = CGBitmapContextCreateImage(bitmapContext);
    CFStringRef type = kUTTypeBMP; // or kUTTypePNG if you like
    CFMutableDataRef dataRef = CFDataCreateMutable(NULL, width * height);
    
    CFDataAppendBytes(dataRef, buffer, width * height);
    CGImageDestinationRef dest = CGImageDestinationCreateWithData(dataRef, type, 1, NULL);

    CGImageDestinationAddImage(dest, cgImage, 0);
//
//    
//    CFRelease(cgImage);
//    CFRelease(bitmapContext);
//    CGImageDestinationFinalize(dest);
    
    post("writing to memory finished!");
}

void writeBitmaptoFile(unsigned char* buffer, size_t width, size_t height){
    
    FILE* f = fopen("/Users/Ramin/Desktop/image.buf", "wb, ccs=UTF-8");
    for (int i = 0; i < width * height * 4; i++) {
        fprintf(f, "%c", *(buffer+i));
        //post(*(buffer+i));
    }
//    int results = fputs(buffer, f);
//    if (results == EOF) {
//        post("failed to write");
//    }
    fclose(f);

    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef bitmapContext = CGBitmapContextCreate(
                                                       buffer,
                                                       width,
                                                       height,
                                                       8, // bitsPerComponent
                                                       4*width, // bytesPerRow
                                                       colorSpace,
                                                       kCGImageAlphaPremultipliedFirst);
    
    CFRelease(colorSpace);
    
    CGImageRef cgImage = CGBitmapContextCreateImage(bitmapContext);
    CFURLRef url = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, CFSTR("/Users/Ramin/Desktop/image.png"), kCFURLPOSIXPathStyle, false);
    
    CFStringRef type = kUTTypeBMP; // or kUTTypePNG if you like
    CGImageDestinationRef dest = CGImageDestinationCreateWithURL(url, type, 1, 0);
    
    CGImageDestinationAddImage(dest, cgImage, 0);
    

    CFRelease(cgImage);
    CFRelease(bitmapContext);
    CGImageDestinationFinalize(dest);
    
    post("writing to the file finished!");
}

typedef struct _jit_mymatrix
{
	t_jit_object				obj;
	float myval;
} t_jit_mymatrix;

static t_class* _jit_mymatrix_class = NULL;

t_jit_err jit_mymatrix_init(void);
t_jit_mymatrix* jit_mymatrix_new(void);
void jit_mymatrix_free(t_jit_mymatrix *x);
t_jit_err jit_mymatrix_matrix_calc(t_jit_mymatrix *x,
	void *inputs, void *outputs);

t_jit_err jit_mymatrix_init(void)
{
	long attrflags = 0;
	t_jit_object *attr;
	//matrix operator
	t_jit_object *mop;

	// create our class
	_jit_mymatrix_class = jit_class_new("jit_mymatrix", (method)jit_mymatrix_new, (method)jit_mymatrix_free,
		sizeof(t_jit_mymatrix), 0L);
	/*    Define and set the matrix operator
	 1. Create an instance of the mop object
	 2. Enforce a single type for all inputs and outputs
	 3. Enforce a single plane count for all inputs and outputs
	 4. Add the created mop object as an adornment to the class
	 */
	mop = jit_object_new(_jit_sym_jit_mop, 1, 1);
	jit_mop_single_type(mop, _jit_sym_char);
	jit_mop_single_planecount(mop, 4);
	jit_class_addadornment(_jit_mymatrix_class, mop);

	//add method to class
	jit_class_addmethod(_jit_mymatrix_class, (method)jit_mymatrix_matrix_calc, "matrix_calc", A_CANT, 0L);

	//define attribute
	attr = jit_object_new(
		_jit_sym_jit_attr_offset,
		"myval",
		_jit_sym_float32,
		attrflags,
		(method)0L,
		(method)0L,
		calcoffset(t_jit_mymatrix, myval));

	//add the attribute object to class
	jit_class_addattr(_jit_mymatrix_class, attr);


	//return class
	jit_class_register(_jit_mymatrix_class);

    
    //Start G-Streamer
//    post("starting the pipeline");
//    initPipeline(NULL, NULL);
    
    //setupTCPSender(5555);
    
    
	return JIT_ERR_NONE;
}


//ctor
t_jit_mymatrix *jit_mymatrix_new(void)
{
	t_jit_mymatrix *x;

	//allocate object
	x = jit_object_alloc(_jit_mymatrix_class);
	if (x) {
		//if successful, perfom any initialization
		x->myval = 0;
    }
	
    return x;
}

//dtor
void jit_mymatrix_free(t_jit_mymatrix *x)
{
	//nada
}

t_jit_err jit_mymatrix_matrix_calc(t_jit_mymatrix *x,
	void *inputs, void *outputs)
{
	void* inMatrix;
	void* outMatrix;
	long inMatrixLock;
	t_jit_err err = JIT_ERR_NONE;

	inMatrix = jit_object_method(inputs, _jit_sym_getindex, 0);
	inMatrixLock = (long)jit_object_method(inMatrix, _jit_sym_lock, 1);

	if (!inMatrix) {
		err = JIT_ERR_INVALID_INPUT;
		post("Invalid Input");
		return err;
	}

	outMatrix = jit_object_method(outputs, _jit_sym_getindex, 0);
	if (!outMatrix) {
		err = JIT_ERR_INVALID_OUTPUT;
		post("Invalid Output");
		return err;
	}

	if (!x) {
		err = JIT_ERR_INVALID_OBJECT;
		post("Invalid Object");
		return err;
	}

	t_jit_matrix_info info;
	jit_object_method(inMatrix, _jit_sym_getinfo, &info);

	long planeCount = info.planecount;
	long dimCount = info.dimcount;
	long width = info.dim[0];
	long height = info.dim[1];
    long dimStrideW = info.dimstride[0];
    long dimStrideH = info.dimstride[1];
	post("dimcount = %d, planecount = %d, width = %d, height = %d, dimstrideW = %d, dimStrideH = %d", dimCount, planeCount, width, height, dimStrideW, dimStrideH);

	unsigned char* inMatrixData;
	jit_object_method(inMatrix, _jit_sym_getdata, &inMatrixData);

    //remove the stride bytes from the data
    unsigned char* pixelsBuffer = (unsigned char*)malloc(width * height * 4 * sizeof(unsigned char));
    for (long i = 0; i < height; i++) {
        uchar* ip = (uchar*)inMatrixData + (i * dimStrideH);
        memcpy(pixelsBuffer + (i * width * 4), ip, width * 4);
    }
    
    post("size = ", sizeof(inMatrix));
    struct buffer*  buf = createBuffer(inMatrix, sizeof(inMatrix));
    enqueue(buf);
    
    //setBuffer(pixelsBuffer);
    //sendData(pixelsBuffer);
    //post("size of pixelsBuffer %c : %d", pixelsBuffer, sizeof(pixelsBuffer));
    //writeBitmaptoMemory(pixelsBuffer, width, height);
    //writeBitmaptoFile(pixelsBuffer, width, height);
//    free(pixelsBuffer);
//    post("DONE!!!");
    
	jit_object_method(inMatrix, _jit_sym_lock, inMatrixLock);
	return err;
}
