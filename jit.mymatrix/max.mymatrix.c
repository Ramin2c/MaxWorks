/*
	Copyright 2001-2005 - Cycling '74
	Joshua Kit Clayton jkc@cycling74.com
*/

#include "jit.common.h"
#include "max.jit.mop.h"

typedef struct _max_mymatrix
{
	t_object		obj;
	void			*obex;
} t_max_mymatrix;

void* class_max_mymatrix;

void *max_mymatrix_new(t_symbol *s, long argc, t_atom *argv);
void max_mymatrix_free(t_max_mymatrix *x);
void max_mymatrix_assist(t_max_mymatrix *x, void *b, long m, long a, char *s);

void ext_main(void *r)
{
	void *p,*q;

	//Initialize the Jitter class
    jit_mymatrix_init();
	
//    class_new("jit.foo",
//              (method)max_jit_foo_new,
//              (method)max_jit_foo_free,
//              (short)sizeof(t_max_jit_foo),
//              0L,
//              0
//    );

    setup(
          (t_messlist**)&class_max_mymatrix,
          (method)max_mymatrix_new,
          (method)max_mymatrix_free,
          (short)sizeof(t_max_mymatrix),
		  0L,
          A_GIMME,
          0);

    //specify a byte offset to keep additional information
    p = max_jit_classex_setup(calcoffset(t_max_mymatrix, obex));
	
    //lookup the Jitter class in the class registery
    q = jit_class_findbyname(gensym("jit_mymatrix"));
    
    //add default methods and attribs for the MOP wrap
    max_jit_classex_mop_wrap(p,q,0);

    //wrap the Jitter class with the standard methods for Jitter objects
	max_jit_classex_standard_wrap(p, q, 0);
    
	//add an inlet/outlet assistance method
    //addmess((method)max_mymatrix_assist, "assist", A_CANT, 0);
}

void max_mymatrix_free(t_max_mymatrix *x)
{
    //lookup the internal Jitter object instance and free
	jit_object_free(max_jit_obex_jitob_get(x));
	
    //free resources associated with the obex entry
    max_jit_obex_free(x);
}

void *max_mymatrix_new(t_symbol *s, long argc, t_atom *argv)
{
	t_max_mymatrix *x;
    void *o;

    //create the wrapper object instance beased on the max wrapper class, and the jitter class
	if (x=(t_max_mymatrix *)max_jit_obex_new(class_max_mymatrix, gensym("jit_mymatrix"))) {
        
        //instantiate Jitter objet
        if (o = jit_object_new(gensym("jit_mymatrix"))){
            // handle standard MOP max wrapper setup tasks
            max_jit_mop_setup_simple(x, o, argc, argv);
            
            //set internal jitter object instace
            max_jit_obex_jitob_set(x, o);
            
            //process attribute arguments
            max_jit_attr_args(x, argc, argv);
        }
        else{
            //cannot initialize, clean up and report error
            error("jit.mymatrix: out of memory");
            freeobject((void*)x);
            x = NULL;
        }
    }
	return (x);
}

void max_mymatrix_assist(t_max_mymatrix *x, void *b, long m, long a, char *s)
{
    // no inlet/outlet assistance
}
