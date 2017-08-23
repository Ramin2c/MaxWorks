//
//  mymatrix.c
//  jit.mymatrix
//
//  Created by Ramin on 2017-07-28.
//
//


#include "jit.common.h"
#include <CGBitmapContext.h>
#include <ImageIO/ImageIO.h>
#include <UTCoreTypes.h>

typedef struct argbColorSpace{
    long a;
    long r;
    long g;
    long b;
}argb;

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
    long dataWidth = (width + dimStrideW) * planeCount;
	post("dimcount = %d, planecount = %d, width = %d, height = %d, dimstrideW = %d, dimStrideH = %d", dimCount, planeCount, width, height, dimStrideW, dimStrideH);

	unsigned char* inMatrixData;
	jit_object_method(inMatrix, _jit_sym_getdata, &inMatrixData);

    unsigned char* argb = (unsigned char*)malloc(width * height * 4);
    for (long i = 0; i < height; i++) {
        uchar* ip = (uchar*)inMatrixData + (i * dimStrideH);
        memcpy(argb + (i * dimStrideH), ip, width);
    }
    
    for (long i = 0; i < width * height; i+=4) {
        uchar a = argb[i];
        uchar r = argb[i + 1];
        uchar g = argb[i + 2];
        uchar b = argb[i + 3];
        post("A R G B = %d %d %d %d ", a, r, g, b);
    }
    
    post("----------------");
    long index = 0;
    for (long i = 0; i < height; i++) {
        uchar* ip = (uchar*)inMatrixData + (i * dimStrideH);
        for (long j = 0; j < width; j++) {
            argb buf;
            buf.a = *ip;
            buf.r = *(ip + 1);
            buf.g = *(ip + 2);
            buf.b = *(ip + 3);
            
            //*(buffer + index) = buf;
            post("A R G B = %d %d %d %d ", buf.a, buf.r, buf.g, buf.b);
            index++;
            ip += 4;
        }
    }
    //generateBitmap(buffer, width, height);
    post("DONE!!!");
    
	jit_object_method(inMatrix, _jit_sym_lock, inMatrixLock);
	return err;
}
