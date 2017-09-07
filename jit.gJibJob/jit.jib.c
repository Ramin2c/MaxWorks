//
//  jit.jib.c
//  jit.gBridges.jib
//
//  Created by Ramin on 2017-09-07.
//
//

#include "jit.common.h"

typedef struct t_jib{
    t_object obj;
}t_jib;

void* jib_class;

t_jit_err jib_init(void);
t_jib* jib_new(void);
void jib_free(t_jib* jib);

t_jit_err jib_matrix_calc(t_jib* jib, void* inputs, void* outputs);
void jib_calc_ndim(t_jib* jib, long dim, long *dimsize, long planecount, t_jit_matrix_info *out_minfo, char *out_data);

t_jit_err jib_init(void){
    void* mop;
    jib_class = jit_class_new("jit_gjib", (method)jib_new, (method)jib_free, sizeof(t_jib), 0L);
    
    //add mop
    mop = jit_object_new(_jit_sym_jit_mop, 0, 1);
    jit_class_addadornment(jib_class, mop);
    
    //add methods
    jit_class_addmethod(jib_class, (method)jib_matrix_calc, "matrix_calc", A_CANT, 0L);
    
    //register the class
    jit_class_register(jib_class);
    
    return JIT_ERR_NONE;
}

t_jib* jib_new(void){
    t_jib* jib;
    jib = (t_jib*)jit_object_alloc(jib_class);
    if(!jib){
        jib = NULL;
        //TODO: log errors
    }
    return jib;
}

void jib_free(t_jib* jib){
    //TODO: proper cleaning
}

//for now, we process only 2 dimensional ARGB matrix
t_jit_err jib_matrix_calc(t_jib* jib, void* inputs, void* outputs){
    long olock;
    t_jit_err err = JIT_ERR_NONE;
    t_jit_matrix_info ominfo;
    char* odata;
    void* omatrix = jit_object_method(outputs, _jit_sym_getindex, 0);
    if(jib && omatrix){
        //lock the out matrix
        olock = (long)jit_object_method(omatrix, _jit_sym_lock, 1);
        jit_object_method(omatrix, _jit_sym_getinfo, &ominfo);
        jit_object_method(omatrix, _jit_sym_getdata, &odata);
        
        //TODO: log error
        //unlock the out matrix
        if (ominfo.dimcount != 2){
            err = JIT_ERR_MISMATCH_DIM;
            goto out;
        }
        if (ominfo.planecount != 4){
            err =  JIT_ERR_MISMATCH_PLANE;
            goto out;
        }
        if(!odata){
            err = JIT_ERR_INVALID_PTR;
            goto out;
        }

        long dim[2];
        dim[0] = ominfo.dim[0];
        dim[1] = ominfo.dim[1];
        
        jit_parallel_ndim_simplecalc1((method)jib_calc_ndim, jib, ominfo.dimcount, dim, ominfo.planecount, &ominfo, odata, 0);
        
    }
out:
    jit_object_method(omatrix, _jit_sym_lock, olock);
    return err;
}

void jib_calc_ndim(t_jib* jib, long dim, long *dimsize, long planecount, t_jit_matrix_info *out_minfo, char *out_data){
    
}
