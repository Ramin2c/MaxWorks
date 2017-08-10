//  
//  template.c
//
//  Created by Ramin Toussi on 2017-04-26.
//
//

// Do not edit directly here, generate your own copy with similar structure instead

//In case of developing on Windows, use the Microsoft C runtime, instead of Max C runtime
#ifdef WIN32
	#define MAXAPI_USE_MSCRT
#endif // WIN32

// standard Max include, always required
#include "ext.h"							

//Global pointer to the class
static t_class *this_class;

//object declaration
typedef struct _temp{
    t_object theObject;     //t_object header should always be here and as the first member
}t_temp;

//Define method prototypes including the constructor and optional destructor
void *temp_new();
void temp_free();

void do_something(t_temp *tmp);

//The initialization routine must be called ext_main
void ext_main(void *r){
    //Define the class 
    t_class *c;
    
/*    
    Create the class with the new instance routine
    Be noted the first parameter, "temp" MUST be same as the filename
    Otherwise the object will fail to work in the MAX patcher 
*/    
    c = class_new("temp", (method)temp_new, (method)temp_free, sizeof(t_temp), 0L, 0);

/*
    As the first step in creating inlets, add the method that will respond to the incoming message.
	Add all the object methods (those that are to be exposed to the MAX environment)
    And the associated functions or so called "Message Handlers" (the C code here)
    Notice the third parameter, "XXX" is what MAX sees and triggers, examples are "bang" and "int".
*/    
    class_addmethod(c, (method)do_something, "XXX", 0);
    
    //Register it in the name space 
    class_register(CLASS_BOX, c);
    
    //Set to the global pointer
    this_class = c;
} 

//The object constructor
void *temp_new(){
    t_temp *temp = (t_temp*)object_alloc(this_class);
    
    //Also, code for inlets & outlets creations goes here
    
    return temp;
}

//The object destructor
void temp_free(){
    //clean up, free memory, etc
}

//Message Handlers
void do_something(t_temp *tmp /*if needed, extra parameter can go here */){
    //bla bla bla
}

