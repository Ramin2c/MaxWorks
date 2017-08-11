//  
//  mylist.c
//
//  Created by Ramin Toussi on 2017-08-10.
//
//

#ifdef WIN32
	#define MAXAPI_USE_MSCRT
#endif // WIN32

#include "ext.h"

static t_class *this_class;

typedef struct _mylist{
    t_object theObject;
}t_mylist;

void *mylist_new(t_symbol *s, long argc, t_atom *argv);
void mylist_free(t_mylist *x);

void receiveList(t_mylist *list, t_symbol *sym, long argc, t_atom *atom);
void dobang(t_mylist *list);

void ext_main(void *r){
    //Define the class 
    t_class *c;
    
    c = class_new("mylist", (method)mylist_new, (method)mylist_free, sizeof(t_mylist), 0L, 0);

    class_addmethod(c, (method)receiveList, "list", A_GIMME, 0);
    class_addmethod(c, (method)dobang, "bang", 0);

    class_register(CLASS_BOX, c);
    
    this_class = c;
} 

void *mylist_new(){
    t_mylist *mylist = (t_mylist*)object_alloc(this_class);
    return mylist;
}

//The object destructor
void mylist_free(){
    //clean up, free memory, etc
}

typedef struct pallete{
    long a;
    long r;
    long g;
    long b;
}argb;

argb pixels[100];
int i = 0;

//Message Handlers
void receiveList(t_mylist *list, t_symbol *sym, long argc, t_atom *atom){
    long a = atom_getlong(atom);
    long r = atom_getlong(atom + 1);
    long g = atom_getlong(atom + 2);
    long b = atom_getlong(atom + 3);
    argb c;
    c.a = a;
    c.r = r;
    c.g = g;
    c.b = b;
    pixels[i] = c;
    i++;
    post("%d %d %d %d", a, r, g, b);
}

void dobang(t_mylist *list){
    
}
