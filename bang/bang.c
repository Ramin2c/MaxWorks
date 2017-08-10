//
//  bang.c
//
//  Created by Ramin Toussi on 2017-07-18.
//
//


#include "ext.h"							// standard Max include, always required

static t_class *this_class;

typedef struct _bang{
    t_object bangObject;
    void* b_out;
}t_bang;

//Methods
void *bang_new();
void do_bang(t_bang *bang); 
void myobject_in(t_bang *x, long n);

void ext_main(void *r){
    t_class *c;
    
    c = class_new("bang", (method)bang_new, (method)NULL, sizeof(t_bang), 0L, 0);
    class_addmethod(c, (method)do_bang, "bang", 0);
    class_addmethod(c, (method)myobject_in, "in1", A_LONG, 0);
    
    class_register(CLASS_BOX, c);
    
    this_class = c;
}

void *bang_new(){
    t_bang *bang = (t_bang*)object_alloc(this_class);
    bang->b_out = bangout(bang);
    
    intin(bang, 1);
    
    return bang;
}

void do_bang(t_bang *bang){
    post("Bang received on the left intlet!");
    outlet_bang(bang->b_out);
    post("Bang sent!");
}

void myobject_in(t_bang *x, long n)
{
    post("Int %d received in right inlet",n);
}
