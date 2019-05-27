#ifndef PYFL_H
#define PYFL_H

#include <Python.h>

extern "C"{
#include "flmap.h"
}

typedef struct
{
      PyObject_HEAD
      FLHashMap* map;
} PyFLMap;

extern PyTypeObject* PPyFLType;

#endif
