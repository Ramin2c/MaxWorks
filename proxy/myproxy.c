//
//  myproxy.c
//
//  Created by Ramin Toussi on 2017-07-20.
//
//

#include "ext.h"						// standard Max include, always required

typedef struct _myObject{
    t_object object;
    long proxyIndex;
    void* proxy;
}t_proxyObject;

t_class* this_class;

//Methods
void *proxyNew();
void proxyBang(t_proxyObject *object);

void ext_main(void *r){
    t_class *c;
    
    c = class_new("myproxy", (method)proxyNew, (method)NULL, sizeof(t_proxyObject), 0L, 0);
    class_addmethod(c, (method)proxyBang, "bang");
    class_register(CLASS_BOX, c);
    
    this_class = c;
}

void *proxyNew(){
    t_proxyObject *proxyObject = (t_proxyObject*)object_alloc(this_class);
    proxyObject->proxy = proxy_new((t_object*)proxyObject, 1, &proxyObject->proxyIndex);
    
    return proxyObject;
}

void proxyBang(t_proxyObject *object){
    switch(proxy_getinlet((t_object*)object)){
        case 0:
            post("bang received on the left inlet.");
            break;
        case 1:
            post("bang received on the right inlet.");
            break;
            
    }
}

