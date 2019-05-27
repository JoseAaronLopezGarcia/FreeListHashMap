#include "pyflmap.h"

static void fl_dealloc(PyFLMap *self)
{
	FLHashMap_destroy(self->map);
	self->ob_type->tp_free((PyObject*)self);
}

static PyObject* fl_new(PyTypeObject *type, PyObject *args, PyObject *kwargs){
	PyFLMap *self;

	if (PyErr_CheckSignals())
		return NULL;

	self = (PyFLMap*)type->tp_alloc(type, 0);
	if (self){
		self->map = NULL;
	}
	return (PyObject*)self;
}

static int fl_init(PyFLMap *self, PyObject *args, PyObject *kwargs){

	if (PyErr_CheckSignals())
		return -1;

	self->map = FLHashMap_new();

	return 0;
}

static PyObject* fl_get(PyFLMap* self, PyObject* args){
	PyObject* key;
	PyObject* value;
	if (!PyArg_ParseTuple(args, "O:get", &key))
		return NULL;
	if (PyErr_CheckSignals())
		return NULL;
	
	size_t hash = PyObject_Hash(key);
	void* val = FLHashMap_get(self->map, hash);
	value = (val)? (PyObject*)val : Py_None;
	Py_INCREF(value);
	return value;
}

static PyObject* fl_set(PyFLMap* self, PyObject* args){
	PyObject* key;
	PyObject* value;
	
	if (!PyArg_ParseTuple(args, "OO:set", &key, &value))
		return NULL;
	if (PyErr_CheckSignals())
		return NULL;
	if (value == NULL || value == Py_None)
		return NULL;
	
	Py_INCREF(value);
	size_t hash = PyObject_Hash(key);
	void* val = FLHashMap_set(self->map, hash, (void*)value);
	if (val){
		value = (PyObject*)val;
	}
	else{
		Py_INCREF(Py_None);
		value = Py_None;
	}
	return value;
}

static PyObject* fl_del(PyFLMap* self, PyObject* args){
	PyObject* key;
	PyObject* value;
	if (!PyArg_ParseTuple(args, "O:del", &key))
		return NULL;
	if (PyErr_CheckSignals())
		return NULL;
	
	size_t hash = PyObject_Hash(key);
	void* val = FLHashMap_del(self->map, hash);
	if (val){
		value = (PyObject*)val;
	}
	else{
		Py_INCREF(Py_None);
		value = Py_None;
	}
	return value;
}

static PyObject* fl_printStats(PyFLMap* self, PyObject* args){
	FLHashMap_printStats(self->map);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyGetSetDef fl_getset[] = {
	{NULL}
};

static PyMethodDef fl_methods[] = {
	{"get", (PyCFunction)fl_get, METH_VARARGS, ""},
	{"set", (PyCFunction)fl_set, METH_VARARGS, ""},
	{"del", (PyCFunction)fl_del, METH_VARARGS, ""},
	{"printStats", (PyCFunction)fl_printStats, METH_VARARGS, ""},
	{NULL}
};

static PyTypeObject PyFLType = {
	PyObject_HEAD_INIT(NULL)
	0,
	"FLHashMap" "." "Map",
	sizeof(PyFLMap),
	0,
	(destructor)fl_dealloc,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
	"FL HashMap",
	0,
	0,
	0,
	0,
	0,
	0,
	fl_methods,
	0,
	fl_getset,
	0,
	0,
	0,
	0,
	0,
	(initproc)fl_init,
	0,
	fl_new,
};

PyTypeObject* PPyFLType = &PyFLType;

static PyMethodDef fl_functions[] = {
	{NULL}
};

PyMODINIT_FUNC initFLHashMap(void){
	PyObject* mdl;
	
	if (PyType_Ready(PPyFLType) < 0)
		return;
	
	mdl = Py_InitModule3("FLHashMap", fl_functions, "FL HashMap for Python");
	if (!mdl)
		return;
		
	Py_INCREF(PPyFLType);
	PyModule_AddObject(mdl, "Map", (PyObject*)PPyFLType);
}
