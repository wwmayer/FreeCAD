/***************************************************************************
 *   Copyright (c) Jürgen Riegel <juergen.riegel@web.de>                   *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef BASE_UNITSAPIPY_H
#define BASE_UNITSAPIPY_H

#include <Base/Unit.h>
#include <Base/Quantity.h>

// Python stuff
using PyObject = struct _object;
using PyMethodDef = struct PyMethodDef;

namespace Base
{

class BaseExport UnitsApiPy
{
public:
    static PyObject* createModule();

    /// generate a value for a quantity with default user preferred system
    static double toDouble(PyObject* args, const Base::Unit& u = Base::Unit());
    /// generate a value for a quantity with default user preferred system
    static Quantity toQuantity(PyObject* args, const Base::Unit& u = Base::Unit());

private:
    static PyMethodDef Methods[];
    // clang-format off
    // static python wrapper of the exported functions
    // the python API wrapper methods
    static PyObject* sParseQuantity     (PyObject* self, PyObject* args);
    static PyObject* sListSchemas       (PyObject* self, PyObject* args);
    static PyObject* sGetSchema         (PyObject* self, PyObject* args);
    static PyObject* sSetSchema         (PyObject* self, PyObject* args);
    static PyObject* sSchemaTranslate   (PyObject* self, PyObject* args);
    static PyObject* sToNumber          (PyObject* self, PyObject* args);
    // clang-format on
};

}

#endif  // BASE_UNITSAPIPY_H
