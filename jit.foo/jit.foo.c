
#include "jit.common.h"

typedef struct _jit_foo
{
	t_jit_object				obj;
	float myval;
} t_jit_foo;

static t_class* _jit_foo_class = NULL;

t_jit_err jit_foo_init(void);
t_jit_foo* jit_foo_new(void);
void jit_foo_free(t_jit_foo *x);
void jit_foo_scream(t_jit_foo *x, long i);

t_jit_err jit_foo_init(void)
{
	long attrflags=0;
	t_jit_object *attr;

	// create our class
	_jit_foo_class = jit_class_new("jit_foo", (method)jit_foo_new, (method)jit_foo_free,
										 sizeof(t_jit_foo), 0L);

	//add method to class
	jit_class_addmethod(_jit_foo_class, (method)jit_foo_scream, "scream", A_DEFLONG, 0L);
	
	//define attribute
	attr = jit_object_new(
		_jit_sym_jit_attr_offset,
		"myval",
		_jit_sym_float32,
		attrflags,
		(method)0L,
		(method)0L,
		calcoffset(t_jit_foo, myval));

	//add the attribute object to class
	jit_class_addattr(_jit_foo_class, attr);
	
	//return class
	jit_class_register(_jit_foo_class);

	return JIT_ERR_NONE;
}


//ctor
t_jit_foo *jit_foo_new(void)
{
	t_jit_foo *x;

	//allocate object
    x = jit_object_alloc(_jit_foo_class);
    if(x){
		//if successful, perfom any initialization
		x->myval = 0;
	}
	return x;
}

//dtor
void jit_foo_free(t_jit_foo *x)
{
	//nada
}

//scream method
void jit_foo_scream(t_jit_foo *x, long i){
	post("My VALUE is %f & my ARG is %d", x->myval, i);
}
