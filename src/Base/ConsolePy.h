/***************************************************************************
 *   Copyright (c) 2002 Jürgen Riegel <juergen.riegel@web.de>              *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        *
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BASE_CONSOLEPY_H
#define BASE_CONSOLEPY_H

// Std. configurations
#include <FCGlobal.h>

// Python stuff
using PyObject = struct _object;
using PyMethodDef = struct PyMethodDef;

namespace Base
{

class BaseExport ConsolePy
{
public:
    static PyObject* createModule();

private:
    static PyMethodDef Methods[];
    // clang-format off
    // static python wrapper of the exported functions
    static PyObject* sPyLog                         (PyObject* self, PyObject* args);
    static PyObject* sPyMessage                     (PyObject* self, PyObject* args);
    static PyObject* sPyWarning                     (PyObject* self, PyObject* args);
    static PyObject* sPyDeveloperWarning            (PyObject* self, PyObject* args);
    static PyObject* sPyUserWarning                 (PyObject* self, PyObject* args);
    static PyObject* sPyTranslatedUserWarning       (PyObject* self, PyObject* args);
    static PyObject* sPyError                       (PyObject* self, PyObject* args);
    static PyObject* sPyDeveloperError              (PyObject* self, PyObject* args);
    static PyObject* sPyUserError                   (PyObject* self, PyObject* args);
    static PyObject* sPyTranslatedUserError         (PyObject* self, PyObject* args);
    static PyObject* sPyCritical                    (PyObject* self, PyObject* args);
    static PyObject* sPyNotification                (PyObject* self, PyObject* args);
    static PyObject* sPyTranslatedNotification      (PyObject* self, PyObject* args);
    static PyObject* sPySetStatus                   (PyObject* self, PyObject* args);
    static PyObject* sPyGetStatus                   (PyObject* self, PyObject* args);
    static PyObject* sPyGetObservers                (PyObject* self, PyObject* args);
    // clang-format on
};

}

#endif  // BASE_CONSOLEPY_H
