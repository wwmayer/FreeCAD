/***************************************************************************
 *   Copyright (c) 2011 Jürgen Riegel <juergen.riegel@web.de>              *
 *   Copyright (c) 2011 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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

#ifndef GUI_SELECTIONPY_H
#define GUI_SELECTIONPY_H

#include <FCConfig.h>


using PyObject = struct _object;
using PyMethodDef = struct PyMethodDef;

namespace Gui
{

class GuiExport SelectionPy
{
public:
    static PyObject* createModule();

private:
    static PyMethodDef Methods[];
    static PyObject *sAddSelection          (PyObject *self,PyObject *args);
    static PyObject *sUpdateSelection       (PyObject *self,PyObject *args);
    static PyObject *sRemoveSelection       (PyObject *self,PyObject *args);
    static PyObject *sClearSelection        (PyObject *self,PyObject *args);
    static PyObject *sIsSelected            (PyObject *self,PyObject *args);
    static PyObject *sCountObjectsOfType    (PyObject *self,PyObject *args);
    static PyObject *sGetSelection          (PyObject *self,PyObject *args);
    static PyObject *sSetPreselection       (PyObject *self,PyObject *args,PyObject *kwd);
    static PyObject *sGetPreselection       (PyObject *self,PyObject *args);
    static PyObject *sRemPreselection       (PyObject *self,PyObject *args);
    static PyObject *sGetCompleteSelection  (PyObject *self,PyObject *args);
    static PyObject *sGetSelectionEx        (PyObject *self,PyObject *args);
    static PyObject *sGetSelectionObject    (PyObject *self,PyObject *args);
    static PyObject *sSetSelectionStyle     (PyObject *self,PyObject *args);
    static PyObject *sAddSelObserver        (PyObject *self,PyObject *args);
    static PyObject *sRemSelObserver        (PyObject *self,PyObject *args);
    static PyObject *sAddSelectionGate      (PyObject *self,PyObject *args);
    static PyObject *sRemoveSelectionGate   (PyObject *self,PyObject *args);
    static PyObject *sGetPickedList         (PyObject *self,PyObject *args);
    static PyObject *sEnablePickedList      (PyObject *self,PyObject *args);
    static PyObject *sPreselect             (PyObject *self,PyObject *args);
    static PyObject *sSetVisible            (PyObject *self,PyObject *args);
    static PyObject *sPushSelStack          (PyObject *self,PyObject *args);
    static PyObject *sHasSelection          (PyObject *self,PyObject *args);
    static PyObject *sHasSubSelection       (PyObject *self,PyObject *args);
    static PyObject *sGetSelectionFromStack (PyObject *self,PyObject *args);
};

} //namespace Gui

#endif // GUI_SELECTIONPY_H
