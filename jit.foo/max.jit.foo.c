/*
	Copyright 2001-2005 - Cycling '74
	Joshua Kit Clayton jkc@cycling74.com
*/

#include "jit.common.h"
#include "max.jit.mop.h"

typedef struct _max_jit_foo
{
	t_object		obj;
	void			*obex;
} t_max_jit_foo;

void* class_max_jit_foo;

void *max_jit_foo_new(t_symbol *s, long argc, t_atom *argv);
void max_jit_foo_free(t_max_jit_foo *x);
void max_jit_foo_assist(t_max_jit_foo *x, void *b, long m, long a, char *s);

void ext_main(void *r)
{
	void *p,*q;

	//Initialize the Jitter class
    jit_foo_init();
	
//    class_new("jit.foo",
//              (method)max_jit_foo_new,
//              (method)max_jit_foo_free,
//              (short)sizeof(t_max_jit_foo),
//              0L,
//              0
//    );

    setup(
          &class_max_jit_foo,
          (method)max_jit_foo_new,
          (method)max_jit_foo_free,
          (short)sizeof(t_max_jit_foo),
		  0L,
          A_GIMME,
          0);

    //specify a byte offset to keep additional information
    p = max_jit_classex_setup(calcoffset(t_max_jit_foo, obex));
	
    //lookup the Jitter class in the class registery
    q = jit_class_findbyname(gensym("jit_foo"));
    
    //wrap the Jitter class with the standard methods for Jitter objects
	max_jit_classex_standard_wrap(p, q, 0);
    
	//add an inlet/outlet assistance method
    addmess((method)max_jit_foo_assist, "assist", A_CANT, 0);
}

void max_jit_foo_free(t_max_jit_foo *x)
{
    //lookup the internal Jitter object instance and free
	jit_object_free(max_jit_obex_jitob_get(x));
	
    //free resources associated with the obex entry
    max_jit_obex_free(x);
}

void *max_jit_foo_new(t_symbol *s, long argc, t_atom *argv)
{
	t_max_jit_foo *x;
    long attrstart;
    void *o;

    //create the wrapper object instance beased on the max wrapper class, and the jitter class
	if (x=(t_max_jit_foo *)max_jit_obex_new(class_max_jit_foo, gensym("jit_foo"))) {
        //add dumpout outlet
        max_jit_obex_dumpout_set(x, outlet_new(x, 0L));
        
        //get normal args if necessary
        attrstart = max_jit_attr_args_offset(argc, argv);
        
        //instantiate Jitter objet
        if (o = jit_object_new(gensym("jit_foo"))){
            //set internal jitter object instace
            max_jit_obex_jitob_set(x, o);
            
            //process attribute arguments
            max_jit_attr_args(x, argc, argv);
        }
        else{
            //cannot initialize, clean up and report error
            freeobject((void*)x);
            x = NULL;
            error("jit.foo: out of memory");
        }
    }
	return (x);
}

void max_jit_foo_assist(t_max_jit_foo *x, void *b, long m, long a, char *s)
{
    // no inlet/outlet assistance
}
