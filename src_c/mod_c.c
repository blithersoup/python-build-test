#include <Python.h>
#include "mod_c.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>

// static void
// obj_dealloc(PyObject *self)
// {
//     if (((derClassObject *)self)->weakreflist)
//         PyObject_ClearWeakRefs(self);
//     free(((derClassObject*) self)->cls->code);
//     Py_TYPE(self)->tp_free(self);
// }
static void
obj_dealloc(PyObject *self)
{
    derClassObject *obj = (derClassObject *)self;

    if (obj->weakreflist)
        PyObject_ClearWeakRefs(self);

    if (obj->cls) {
        if (obj->cls->code != NULL) {
            free(obj->cls->code);
            obj->cls->code = NULL;  // Set to NULL after freeing to avoid double-free
        }
    }

    Py_TYPE(self)->tp_free(self);
}

static PyObject *
obj_str(PyObject *self)
{
    der_class *cls = derClassObject_asDerClass(self);

    if (!cls) {
        return PyUnicode_FromString("<Obj(no data)>");
    }

    return PyUnicode_FromFormat("%s", cls->code);
}

static PyObject *
obj_repr(PyObject *self)
{
    der_class *cls = derClassObject_asDerClass(self);
    
    if (!cls) {
        return PyUnicode_FromString("");
    }

    return PyUnicode_FromFormat("<Obj(%d:\"%s\")>", cls->x, cls->code);
}

/*
static struct PyMethodDef obj_methods[] = {
    {"__str__", (PyCFunction)obj_str, METH_NOARGS, "obj_str"},
    {NULL}
};*/

static intptr_t
obj_init(derClassObject *self, PyObject *args, PyObject *kwds)
{
    PyObject *str = NULL;
    if (!PyArg_ParseTuple(args, "U", &str))
        return -1;
    
    der_class *cls = derClassObject_asDerClass(self);

    
    if (str) {
        Py_INCREF(str);
        Py_ssize_t py_size;
        
        const char *c_str = PyUnicode_AsUTF8AndSize(str, &py_size);
        int len = (int) py_size;
        
        cls->code = malloc((len + 1) * sizeof(char));
        if (cls->code == NULL) {
            PyErr_NoMemory();
            return -1;
        }

        strncpy(cls->code, c_str, len);
        cls->code[len] = '\0';
        cls->x = len;
    }

    return 0;
}

static PyObject *
obj_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    derClassObject *self;
    self = (derClassObject*)type->tp_alloc(type, 0);
    
    if (self == NULL) {
        return NULL;
    }

    self->cls = (der_class *) malloc(sizeof(der_class));

    if (self->cls == NULL) {
        Py_DECREF(self);
        return PyErr_NoMemory();
    }
    self->cls->x = 0;
    self->cls->code = NULL;
    
    return (PyObject *) self;
}

static PyTypeObject derClass_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "testmod.mod_c.test_cls_c",
    .tp_basicsize = sizeof(derClassObject),
    .tp_itemsize = sizeof(char),
    .tp_dealloc = (destructor)obj_dealloc,
    .tp_repr = obj_repr,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_doc = "test_cls_c (len, str)",
    .tp_weaklistoffset = offsetof(derClassObject, weakreflist),
    // .tp_methods = obj_methods,
    .tp_init = (initproc)obj_init,
    .tp_new = obj_new,
    .tp_str = (reprfunc)obj_str
};

static struct PyModuleDef module_definition = {
    PyModuleDef_HEAD_INIT,
    "testmod",                // Module name
    "Test module with C extension",  // Module docstring
    -1,
    NULL,                     // Module state (unused)
    NULL,                     // Methods to execute when the module is loaded
    NULL,                     // Slot definitions (unused)
    NULL                      // Traverse function for GC (unused)
};

// Module initialization function
PyMODINIT_FUNC PyInit_mod_c(void) {
    PyObject *module;

    module = PyModule_Create(&module_definition);
    if (module == NULL) {
        return NULL;
    }

    // Import and initialize the class
    if (PyType_Ready(&derClass_Type) < 0) {
        Py_DECREF(module);
        return NULL;
    }

    // Add the class to the module
    Py_INCREF(&derClass_Type);
    if (PyModule_AddObject(module, "test_cls_c", (PyObject *)&derClass_Type) < 0) {
        Py_DECREF(&derClass_Type);
        Py_DECREF(module);
        return NULL;
    }

    return module;
}