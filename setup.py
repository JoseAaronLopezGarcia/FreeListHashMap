#!/usr/bin/env python
#-*- coding: ISO-8859-1 -*-

__rcsid__ = ''

from distutils.core import setup, Extension
import os

import sys

src = ["pyflmap.cpp", "flmap.c"]
macros = [("LINUX", 1)]

setup(name="FLHashMap",
ext_modules=[Extension("FLHashMap",
			src,
			extra_compile_args=["-O3", "-Wall", "-fPIC"],
			define_macros=macros,
			libraries=[],
			library_dirs=[]
		)
	]
)
