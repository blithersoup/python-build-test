#include <Python.h>

struct der_class {
    int x;
    char code[4];
};

static void
surface_dealloc(PyObject *self)
{
    if (((pgSurfaceObject *)self)->weakreflist)
        PyObject_ClearWeakRefs(self);
    surface_cleanup((pgSurfaceObject *)self);
    Py_TYPE(self)->tp_free(self);
}

static PyObject *
surface_str(PyObject *self)
{
    SDL_Surface *surf = pgSurface_AsSurface(self);

    if (!surf) {
        return PyUnicode_FromString("<Surface(Dead Display)>");
    }

    return PyUnicode_FromFormat("<Surface(%dx%dx%d SW)>", surf->w, surf->h,
                                surf->format->BitsPerPixel);
}

static struct PyMethodDef surface_methods[] = {
    {"get_at", surf_get_at, METH_VARARGS, DOC_SURFACEGETAT}
};

static PyGetSetDef surface_getsets[] = {
    {"_pixels_address", (getter)surf_get_pixels_address, NULL,
     "pixel buffer address (readonly)", NULL},
    {NULL, NULL, NULL, NULL, NULL}};

static PyTypeObject pgSurface_Type = {
    PyVarObject_HEAD_INIT(NULL, 0).tp_name = "testbuild.mod_c.test_cls_c",
    .tp_basicsize = sizeof(pgSurfaceObject),
    .tp_dealloc = surface_dealloc,
    .tp_repr = surface_str,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_doc = DOC_PYGAMESURFACE,
    .tp_weaklistoffset = offsetof(pgSurfaceObject, weakreflist),
    .tp_methods = surface_methods,
    .tp_getset = surface_getsets,
    .tp_init = (initproc)surface_init,
    .tp_new = surface_new,
};