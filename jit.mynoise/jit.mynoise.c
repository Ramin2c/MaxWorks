/*
	Copyright 2001 - Cycling '74
	Joshua Kit Clayton jkc@cycling74.com
*/

#include "jit.common.h"

#define jflone 	0x3f800000
#define jflmsk  0x007fffff

typedef struct _jit_mynoise_vecdata
{
	long			randlast;
} t_jit_mynoise_vecdata;

typedef struct _jit_mynoise
{
	t_object		ob;
} t_jit_mynoise;

void *_jit_mynoise_class;

t_jit_err jit_mynoise_init(void);
t_jit_mynoise *jit_mynoise_new(void);
void jit_mynoise_free(t_jit_mynoise *x);
t_jit_err jit_mynoise_getvecdata(t_jit_mynoise *x, t_jit_mynoise_vecdata *vd);
t_jit_err jit_mynoise_matrix_calc(t_jit_mynoise *x, void *inputs, void *outputs);

void jit_mynoise_calculate_ndim(t_jit_mynoise_vecdata *vecdata, long dim, long *dimsize, long planecount, t_jit_matrix_info *out_minfo, char *bop);
void jit_mynoise_vector_char		(long n, t_jit_mynoise_vecdata *vecdata, t_jit_op_info *out);
void jit_mynoise_vector_char_norand		(long n, t_jit_op_info *out);

void jit_noise_vector_long		(long n, t_jit_mynoise_vecdata *vecdata, t_jit_op_info *out);
void jit_noise_vector_float32	(long n, t_jit_mynoise_vecdata *vecdata, t_jit_op_info *out);
void jit_noise_vector_float64	(long n, t_jit_mynoise_vecdata *vecdata, t_jit_op_info *out);

unsigned char image[40000];

typedef unsigned short byte;
byte getRandom() {
    byte res = rand() % 256;
    return res;
}

t_jit_err jit_mynoise_init(void)
{
	void *mop;

	_jit_mynoise_class = jit_class_new("jit_mynoise",(method)jit_mynoise_new,(method)jit_mynoise_free,
									 sizeof(t_jit_mynoise),0L);

	//add mop
	mop = jit_object_new(_jit_sym_jit_mop,0,1);
	jit_class_addadornment(_jit_mynoise_class,mop);
	//add methods
	jit_class_addmethod(_jit_mynoise_class, (method)jit_mynoise_matrix_calc, 		"matrix_calc", 		A_CANT, 0L);

	jit_class_register(_jit_mynoise_class);

    post("init");
	return JIT_ERR_NONE;
}

t_jit_err jit_mynoise_getvecdata(t_jit_mynoise *x, t_jit_mynoise_vecdata *vd)
{
	if (x&&vd) {
		vd->randlast 	= jit_rand();
		return JIT_ERR_NONE;
	} else {
		return JIT_ERR_INVALID_PTR;
	}
}

t_jit_err jit_mynoise_matrix_calc(t_jit_mynoise *x, void *inputs, void *outputs)
{
	t_jit_err err = JIT_ERR_NONE;
	long out_savelock;
	t_jit_matrix_info out_minfo;
	char *out_bp;
	long i,dimcount,planecount,dim[JIT_MATRIX_MAX_DIMCOUNT];
	t_jit_mynoise_vecdata	vecdata;
	void *out_matrix;

	out_matrix 	= jit_object_method(outputs, _jit_sym_getindex, 0);

	if (x && out_matrix) {
		out_savelock = (long) jit_object_method(out_matrix,_jit_sym_lock,1);

		jit_object_method(out_matrix,_jit_sym_getinfo,&out_minfo);

		jit_object_method(out_matrix,_jit_sym_getdata,&out_bp);

		if (!out_bp) { err=JIT_ERR_INVALID_OUTPUT; goto out;}

		//get dimensions/planecount
		dimcount   = out_minfo.dimcount;
		planecount = out_minfo.planecount;
        

        for (i=0; i<dimcount; i++) {
			dim[i] = out_minfo.dim[i];
		}
        
        
        //out_bp = image;
        
        //return JIT_ERR_NONE;

		//jit_mynoise_getvecdata(x,&vecdata);
		jit_parallel_ndim_simplecalc1((method)jit_mynoise_calculate_ndim,
									  &vecdata, dimcount, dim, planecount, &out_minfo, out_bp,
									  0 /* flags1 */);
	} else {
		return JIT_ERR_INVALID_PTR;
	}

out:
	jit_object_method(out_matrix,_jit_sym_lock,out_savelock);
	return err;
}


void jit_mynoise_calculate_ndim(t_jit_mynoise_vecdata *vecdata, long dimcount, long *dim, long planecount, t_jit_matrix_info *out_minfo, char *bop)
{
    //post("d = %d, %d, %d, %d", dim[0], dim[1], *dim, *(dim + sizeof(long)));
	long i, n;
	char *op;
	t_jit_op_info out_opinfo;

	if (dimcount < 1) return; //safety

	switch(dimcount) {
	case 1:
		dim[1]=1;
	case 2:
		//flatten planes - treat as single plane data for speed...ALWAYS
		n = dim[0];
		out_opinfo.stride = 1;
		n *= planecount;
		planecount = 4;
		if (out_minfo->type==_jit_sym_char) {
            long hStrideDest = out_minfo->dimstride[1];
            long hStrideSrc = 0;
            long width = out_minfo->dim[0];
            long height = out_minfo->dim[1];
            
            
            
            //fill the image array (raw bytes, no stride)
            for (int i=0; i<40000; i+=4) {
                byte A = 255;
                byte R = getRandom();
                byte G = getRandom();
                byte B = getRandom();
                image[i] = (char)A;
                image[i + 1] = (char)R;
                image[i + 2] = (char)G;
                image[i + 3] = (char)B;
            }
            
            //practice copying the bytes with strides to the destination
            for (int i = 0; i < height; i++) {
                unsigned char *src = image + (i * (width * planecount + hStrideSrc));
                memcpy(bop + (i * hStrideDest), src, width * planecount);
            }
            post("memcpy done");
//            int cnt = 0;
//            post("dim 0 = %d, dim 1 = %d", dim[0], dim[1]);
//            for (i=0; i<dim[1]; i++) {
//				vecdata->randlast 	= jit_rand();
//				out_opinfo.p = bop + i*out_minfo->dimstride[1];
//				//jit_mynoise_vector_char_norand(n, &out_opinfo);
//                cnt++;
//			}
//            post("%d times", dim[i]);
		} else if (out_minfo->type==_jit_sym_long) {
			for (i=0; i<dim[1]; i++) {
				vecdata->randlast 	= jit_rand();
				out_opinfo.p = bop + i*out_minfo->dimstride[1];
				jit_noise_vector_long(n,vecdata,&out_opinfo);
			}
		} else if (out_minfo->type==_jit_sym_float32) {
			for (i=0; i<dim[1]; i++) {
				vecdata->randlast 	= jit_rand();
				out_opinfo.p = bop + i*out_minfo->dimstride[1];
				jit_noise_vector_float32(n,vecdata,&out_opinfo);
			}
		} else if (out_minfo->type==_jit_sym_float64) {
			for (i=0; i<dim[1]; i++) {
				vecdata->randlast 	= jit_rand();
				out_opinfo.p = bop + i*out_minfo->dimstride[1];
				jit_noise_vector_float64(n,vecdata,&out_opinfo);
			}
		}
		break;
	default:
		for	(i=0; i<dim[dimcount-1]; i++) {
			op  = bop  + i*out_minfo->dimstride[dimcount-1];
            post("ndim from default jit_mynoise_calculate_ndim");
			jit_mynoise_calculate_ndim(vecdata,dimcount-1,dim,planecount,out_minfo,op);
		}
	}
}

void jit_mynoise_vector_char(long n, t_jit_mynoise_vecdata *vecdata, t_jit_op_info *out)
{
	uchar *op;
	long os;
	unsigned long  idum=vecdata->randlast;

	op  = ((uchar *)out->p);
	os  = out->stride;

	if (os==1) {
		++n; --op;
		while (--n) {
			idum  = 1664525L * idum + 1013904223L;
			*++op = (idum>>16L); //shift right to remove low order correlation
		}
		vecdata->randlast = idum;
	} else {
		while (n--) {
			idum = 1664525L * idum + 1013904223L;
			*op = (idum>>16L);
			op+=os;
		}
		vecdata->randlast = idum;
	}
}

void jit_mynoise_vector_char_norand(long n, t_jit_op_info *out)
{
    uchar *op;
    long os;
    
    op  = ((uchar *)out->p);
    os  = out->stride;
    
    if (os==1) {
        ++n; --op;
        while (--n) {
            *++op = 0xAA;
        }
    } else {
        while (n--) {
            *op = 0xAA;
            op+=os;
        }
    }
}

t_jit_mynoise *jit_mynoise_new(void)
{
	t_jit_mynoise *x;

	if (x=(t_jit_mynoise *)jit_object_alloc(_jit_mynoise_class)) {
		//nada
        srand(time(NULL));
        post("new");

	} else {
		x = NULL;
	}
	return x;
}

void jit_mynoise_free(t_jit_mynoise *x)
{
	//nada
    post("free");
}

void jit_noise_vector_long(long n, t_jit_mynoise_vecdata *vecdata, t_jit_op_info *out)
{
	t_int32 *op;
	long os;
	unsigned long  idum=vecdata->randlast,idum2;

	op  = ((t_int32 *)out->p);
	os  = out->stride;

	if (os==1) {
		++n; --op;
		idum2 = idum;
		while (--n) {
			idum  = 1664525L * idum2 + 1013904223L;
			idum2 = 1664525L * idum + 1013904223L;
			*++op = (idum2&0xFFFF0000)|(idum>>16L); //shift right to remove low order correlation
		}
		vecdata->randlast = idum2;
	} else {
		idum2 = idum;
		while (n--) {
			idum = 1664525L * idum2 + 1013904223L;
			idum2 = 1664525L * idum + 1013904223L;
			*op = (idum2&0xFFFF0000)|(idum>>16L);
			op+=os;
		}
		vecdata->randlast = idum2;
	}
}

void jit_noise_vector_float32(long n, t_jit_mynoise_vecdata *vecdata, t_jit_op_info *out)
{
	float *op;
	long os;
	unsigned long  idum=vecdata->randlast,itemp;

	op  = ((float *)out->p);
	os  = out->stride;

	if (os==1) {
		++n; --op;
		while (--n) {
			idum = 1664525L * idum + 1013904223L;
			itemp = jflone | (jflmsk & idum);
			*++op = ((*(float *)&itemp) - 1.0);
		}
		vecdata->randlast = idum;
	} else {
		while (n--) {
			idum = 1664525L * idum + 1013904223L;
			itemp = jflone | (jflmsk & idum);
			*op = ((*(float *)&itemp) - 1.0);
			op+=os;
		}
		vecdata->randlast = idum;
	}
}

void jit_noise_vector_float64(long n, t_jit_mynoise_vecdata *vecdata, t_jit_op_info *out)
{
	double *op;
	long os;
	unsigned long  idum=vecdata->randlast,itemp;

	op  = ((double *)out->p);
	os  = out->stride;

	if (os==1) {
		++n; --op;
		while (--n) {
			idum = 1664525L * idum + 1013904223L;
			itemp = jflone | (jflmsk & idum);
			*++op = ((*(float *)&itemp) - 1.0);
		}
		vecdata->randlast = idum;
	} else {
		while (n--) {
			idum = 1664525L * idum + 1013904223L;
			itemp = jflone | (jflmsk & idum);
			*op = ((*(float *)&itemp) - 1.0);
			op+=os;
		}
		vecdata->randlast = idum;
	}
}
