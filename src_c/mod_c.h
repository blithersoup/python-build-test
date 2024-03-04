#include "Python.h"
#ifndef MOD_C_H
#define MOD_C_H

typedef struct _der_class {
    int x;
    char *code;
} der_class;

typedef struct {
    PyObject_HEAD struct der_class *cls;
    
    int owner;
    
    PyObject *weakreflist;
    
    PyObject *locklist;
    
    PyObject *dependency;
} derClassObject;

#define derClassObject_asDerClass(x) (((derClassObject*)x)->cls)

#endif