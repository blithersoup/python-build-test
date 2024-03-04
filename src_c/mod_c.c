#include <Python.h>
#include "mod_c.h"
#include <stddef.h>

static void
obj_dealloc(PyObject *self)
{
    if (((derClassObject *)self)->weakreflist)
        PyObject_ClearWeakRefs(self);
    free(((derClassObject*) self)->cls->code);
    Py_TYPE(self)->tp_free(self);
}

static PyObject *
obj_str(PyObject *self)
{
    der_class *cls = derClassObject_asDerClass(self);

    if (!cls) {
        return PyUnicode_FromString("<Obj(Dead Display)>");
    }

    return PyUnicode_FromFormat("<Obj(%d:\"%.*s\")>", cls->x, cls->x, cls->code);
}

static PyObject *
obj_get(PyObject *self)
{
    der_class *cls = derClassObject_asDerClass(self);
    
    if (!cls) {
        return PyUnicode_FromString("");
    }

    return PyUnicode_FromFormat("%.*s", cls->x, cls->code);
}

static struct PyMethodDef obj_methods[] = {
    {"__repr__", obj_get, METH_VARARGS, "obj_get"}
};

static intptr_t
obj_init(derClassObject *self, PyObject *args, PyObject *kwds)
{
    PyObject *str = NULL;
    if (!PyArg_ParseTuple(args, "S", &str))
        return -1;
    
    der_class *cls = derClassObject_asDerClass(self);

    if (str) {
        Py_INCREF(str);
        int len = (size_t) PyUnicode_GetLength(str);

        PyObject *py_bytes_str = PyUnicode_AsEncodedString(str, "utf-8", "strict");
        char *c_str = PyBytes_AsString(py_bytes_str);

        cls->code = malloc(len * sizeof(char));

        for (int i = 0; i < len; i++) {
            cls->code[i] = c_str[i];
        }

        Py_DECREF(py_bytes_str);
    }

    return 0;
}

static PyObject *
obj_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    derClassObject *self;

    self = (derClassObject*)type->tp_alloc(type, 0);

    der_class *cls = derClassObject_asDerClass(self);

    if (cls) {
        cls->x = 0;
        cls->code = NULL;
    }

    return (PyObject *) self;
}

static PyTypeObject derClass_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "testbuild.mod_c.test_cls_c",
    .tp_basicsize = sizeof(int),
    .tp_dealloc = (destructor)obj_dealloc,
    .tp_repr = obj_str,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_doc = "test_cls_c (len, str)",
    .tp_weaklistoffset = offsetof(derClassObject, weakreflist),
    .tp_methods = obj_methods,
    .tp_init = (initproc)obj_init,
    .tp_new = obj_new,
};