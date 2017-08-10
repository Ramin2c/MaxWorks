//
//  template.c
//
//  Created by Ramin Toussi on 2017-04-26.
//
//

#include "ext.h"							// standard Max include, always required
#include "ext_obex.h"						// required for new style Max object

/*
typedef struct _simp{
    t_object obj;
} simp;

void *simp_new(t_symbol *s, long argc, t_atom *argv);
void simp_free(simp *s);

t_class *the_simp;

void ext_main(void *r){
    post("ext_main");
    t_class *c;
    c = class_new("simp", (method)simp_new, (method)simp_free, (long)sizeof(simp), 0, A_GIMME, 0);
    class_register(CLASS_BOX, c);
    the_simp = c;
}

void *simp_new(t_symbol *s, long argc, t_atom *argv){
    simp *smp = NULL;
    smp = (simp*)object_alloc(the_simp);
    post("Bing!");
    return (smp);
}

void simp_free(simp *s){
    ;
}
*/

typedef struct _simp
{
    t_object s_obj; // t_object header
    long s_value; // something else
    
    void* outlet;
    
    void* proxy;
    long m_in;
    
    
    void* gen_outlet;
    
} t_simp;


void *simp_new();
void simp_free();
void simp_int(t_simp *x, long n);
void simp_bang(t_simp *x);
void simp_in1(t_simp *x, long n);
void simp_in2(t_simp *x, long n);
void assist(t_simp *x, void *b, long m, long a, char *s);
void myobject_inletinfo(t_simp *x, void *b, long index, char *t);

t_class *s_simp_class; // global pointer to our class definition that is setup in ext_main()

void ext_main(void *r){
    t_class *c;
    c = class_new("simp", (method)simp_new, (method)simp_free, (long)sizeof(t_simp), 0L, A_GIMME, 0);
    class_addmethod(c, (method)simp_int, "int", A_LONG, 0);
    class_addmethod(c, (method)simp_bang, "bang", 0);
    class_addmethod(c, (method)assist, "assist", A_CANT, 0);
    //class_addmethod(c, (method)stdinletinfo, "inletinfo", A_CANT, 0);
    //class_addmethod(c, (method)myobject_inletinfo, "inletinfo", A_CANT, 0);
    //class_addmethod(c, (method)simp_in1, "in1", A_LONG, 0);
    //class_addmethod(c, (method)simp_in2, "in2", A_LONG, 0);
    class_register(CLASS_BOX, c);
    s_simp_class = c;
    post("Simp class up!");
}

void *simp_new(){
    post("*simp_new called!");
    t_simp *x = (t_simp *)object_alloc(s_simp_class);
    //intin(x, 2);
    //intin(x, 1);
    //x->outlet = bangout(x);
    
    x->proxy = proxy_new(x, 1, &x->m_in);
    x->gen_outlet = outlet_new(x, NULL);
    return (x);
}

void assist(t_simp *x, void *b, long m, long a, char *s){
    post("m = %1d, a = %1d, s = %1s", m, a, s);
}


void myobject_inletinfo(t_simp *x, void *b, long index, char *t){
        if (index == 2)
            *t = 1;
}

void simp_int(t_simp *x, long n){
    t_object t;
    
    switch (proxy_getinlet(x)) {
        case 0:
            post("long on received on left %1d", n);
            break;
        case 1:
            post("long on received on right %1d", n);
            break;
            
        default:
            break;
    }
    post("long received");
    x->s_value = n;
}

void simp_bang(t_simp *x){
    switch (proxy_getinlet(x)) {
        case 0:
            post("bang on received on left");
            break;
        case 1:
            post("bang on received on right");
            break;
            
        default:
            break;
    }
    
    //also bang!
    //outlet_bang(x->gen_outlet);
    
    //and "green 43 crazy 8.34."
    t_atom arg[3];
    
    atom_setlong(arg, 43);
    atom_setsym(arg + 1, gensym("crazy"));
    atom_setfloat(arg + 2, 8.34);

    outlet_anything(x->gen_outlet, gensym("green"), 3, arg);
}

void simp_in1(t_simp *x, long n){
    post("long received on the other inlet");
    x->s_value = n;
}

void simp_in2(t_simp *x, long n){
    post("long received on yet another inlet");
    x->s_value = n;
}
void simp_free(){
    ;
}

