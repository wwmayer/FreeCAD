// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2025 Werner Mayer <wmayer[at]users.sourceforge.net>     *
 *                                                                         *
 *   This file is part of FreeCAD.                                         *
 *                                                                         *
 *   FreeCAD is free software: you can redistribute it and/or modify it    *
 *   under the terms of the GNU Lesser General Public License as           *
 *   published by the Free Software Foundation, either version 2.1 of the  *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful, but        *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with FreeCAD. If not, see                               *
 *   <https://www.gnu.org/licenses/>.                                      *
 *                                                                         *
 **************************************************************************/

#include "PreCompiled.h"
#ifndef _PreComp_
# include <memory>
# include <GeomAbs_Shape.hxx>
# include <gp_Pnt.hxx>
# include <TopoDS.hxx>
# include <TopoDS_Face.hxx>
#endif

#include <Base/PyWrapParseTupleAndKeywords.h>
#include <Base/VectorPy.h>

#include "BRepOffsetAPI/MakeOffsetPy.h"
#include "BRepOffsetAPI/MakeOffsetPy.cpp"
#include "TopoShapeWirePy.h"
#include "TopoShapeFacePy.h"


using namespace Part;

PyObject *MakeOffsetPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of MakeOffsetPy
    return new MakeOffsetPy(nullptr);
}

// constructor method
int MakeOffsetPy::PyInit(PyObject* args, PyObject* kwds)
{
    int jointype {};
    PyObject* openres {};

    static const std::array<const char *, 3> keywords{"Join", "IsOpenResult", nullptr};
    if (Base::Wrapped_ParseTupleAndKeywords(args, kwds, "iO!", keywords,
                                            &jointype, &PyBool_Type, &openres)) {
        std::unique_ptr<BRepOffsetAPI_MakeOffsetFix> ptr(new BRepOffsetAPI_MakeOffsetFix(
                                                         static_cast<GeomAbs_JoinType>(jointype),
                                                         Base::asBoolean(openres)));
        setTwinPointer(ptr.release());
        return 0;
    }

    PyErr_Clear();
    if (PyArg_ParseTuple(args, "")) {
        std::unique_ptr<BRepOffsetAPI_MakeOffsetFix> ptr(new BRepOffsetAPI_MakeOffsetFix());
        setTwinPointer(ptr.release());
        return 0;
    }

    PyErr_SetString(PyExc_TypeError, "supported signatures:\n"
                    "MakeOffset()\n"
                    "MakeOffset(Join [GeomAbs_JoinType], IsOpenResult [bool])\n");
    return -1;
}

// returns a string which represents the object e.g. when printed in python
std::string MakeOffsetPy::representation() const
{
    return {"<BRepOffsetAPI_MakeOffset object>"};
}

PyObject* MakeOffsetPy::init(PyObject *args,  PyObject* kwds)
{
    int jointype {};
    PyObject* spine {};
    PyObject* openres = Py_False;

    static const std::array<const char *, 4> keywords_s{"Spine", "Join", "IsOpenResult", nullptr};
    if (Base::Wrapped_ParseTupleAndKeywords(args, kwds, "O!|iO!", keywords_s,
                                            &TopoShapeFacePy::Type, &spine,
                                            &jointype,
                                            &PyBool_Type, &openres)) {
        const auto& shape = static_cast<TopoShapeFacePy*>(spine)->getTopoShapePtr()->getShape();
        getBRepOffsetAPI_MakeOffsetFixPtr()->Init(TopoDS::Face(shape),
                                                  static_cast<GeomAbs_JoinType>(jointype),
                                                  Base::asBoolean(openres));
        Py_Return;
    }

    PyErr_Clear();
    static const std::array<const char *, 3> keywords{"Join", "IsOpenResult", nullptr};
    if (Base::Wrapped_ParseTupleAndKeywords(args, kwds, "|iO!", keywords,
                                            &jointype, &PyBool_Type, &openres)) {
        getBRepOffsetAPI_MakeOffsetFixPtr()->Init(static_cast<GeomAbs_JoinType>(jointype),
                                                  Base::asBoolean(openres));
        Py_Return;
    }

    PyErr_SetString(PyExc_TypeError, "supported signatures:\n"
                    "init(Spine:Face, Join:GeomAbs_JoinType = GeomAbs_Arc, IsOpenResult:bool = False)\n"
                    "init(Join:GeomAbs_JoinType = GeomAbs_Arc, IsOpenResult:bool = False)\n");
    return nullptr;
}

PyObject* MakeOffsetPy::addWire(PyObject *args)
{
    PyObject* wire {};
    if (!PyArg_ParseTuple(args, "O!", &Part::TopoShapeWirePy::Type, &wire)) {
        return nullptr;
    }

    try {
        const auto& shape = static_cast<TopoShapeWirePy*>(wire)->getTopoShapePtr()->getShape();
        getBRepOffsetAPI_MakeOffsetFixPtr()->AddWire(TopoDS::Wire(shape));
        Py_Return;
    }
    catch (const Standard_Failure& e) {
        PyErr_SetString(PyExc_RuntimeError, e.GetMessageString());
        return nullptr;
    }
}

PyObject* MakeOffsetPy::perform(PyObject *args, PyObject* kwds)
{
    double offset {};
    double alt {};
    static const std::array<const char *, 3> keywords{"Offset", "Alt", nullptr};
    if (!Base::Wrapped_ParseTupleAndKeywords(args, kwds, "d|d", keywords, &offset, &alt)) {
        return nullptr;
    }

    try {
        getBRepOffsetAPI_MakeOffsetFixPtr()->Perform(offset, alt);
        Py_Return;
    }
    catch (const Standard_Failure& e) {
        PyErr_SetString(PyExc_RuntimeError, e.GetMessageString());
        return nullptr;
    }
}

PyObject* MakeOffsetPy::build(PyObject *args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    try {
        getBRepOffsetAPI_MakeOffsetFixPtr()->Build();
        Py_Return;
    }
    catch (const Standard_Failure& e) {
        PyErr_SetString(PyExc_RuntimeError, e.GetMessageString());
        return nullptr;
    }
}

PyObject* MakeOffsetPy::isDone(PyObject *args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    Py::Boolean res(getBRepOffsetAPI_MakeOffsetFixPtr()->IsDone());
    return Py::new_reference_to(res);
}

PyObject* MakeOffsetPy::shape(PyObject *args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    try {
        TopoShape shape(getBRepOffsetAPI_MakeOffsetFixPtr()->Shape());
        return shape.getPyObject();
    }
    catch (const Standard_Failure& e) {
        PyErr_SetString(PyExc_RuntimeError, e.GetMessageString());
        return nullptr;
    }
}

PyObject *MakeOffsetPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int MakeOffsetPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
