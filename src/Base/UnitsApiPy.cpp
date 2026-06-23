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


#include "PreCompiled.h"

#ifndef _PreComp_
#include <memory>
#endif

#include <CXX/Objects.hxx>

#include "UnitsApi.h"
#include "UnitsApiPy.h"
#include "Quantity.h"
#include "QuantityPy.h"


using namespace Base;

//**************************************************************************
// Python stuff of UnitsApi

PyObject* UnitsApiPy::createModule()
{
    static struct PyModuleDef UnitsModuleDef = {
        PyModuleDef_HEAD_INIT,
        "Units", "The Unit API", -1,
        Base::UnitsApiPy::Methods,
        nullptr, nullptr, nullptr, nullptr
    };

    return PyModule_Create(&UnitsModuleDef);
}

double UnitsApiPy::toDouble(PyObject* args, const Base::Unit& u)
{
    if (PyUnicode_Check(args)) {
        std::string str(PyUnicode_AsUTF8(args));
        // Parse the string
        Quantity q = Quantity::parse(str);
        if (q.getUnit() == u) {
            return q.getValue();
        }
        throw Base::UnitsMismatchError("Wrong unit type!");
    }

    if (PyFloat_Check(args)) {
        return PyFloat_AsDouble(args);
    }
    if (PyLong_Check(args)) {
        return static_cast<double>(PyLong_AsLong(args));
    }

    throw Base::UnitsMismatchError("Wrong parameter type!");
}

Quantity UnitsApiPy::toQuantity(PyObject* args, const Base::Unit& u)
{
    double d {};
    if (PyUnicode_Check(args)) {
        std::string str(PyUnicode_AsUTF8(args));
        // Parse the string
        Quantity q = Quantity::parse(str);
        d = q.getValue();
    }
    else if (PyFloat_Check(args)) {
        d = PyFloat_AsDouble(args);
    }
    else if (PyLong_Check(args)) {
        d = static_cast<double>(PyLong_AsLong(args));
    }
    else {
        throw Base::UnitsMismatchError("Wrong parameter type!");
    }

    return Quantity(d, u);
}

// UnitsApi Methods
PyMethodDef UnitsApiPy::Methods[] = {
    {"parseQuantity",
     UnitsApiPy::sParseQuantity,
     METH_VARARGS,
     "parseQuantity(string) -> Base.Quantity()\n\n"
     "calculate a mathematical expression with units to a quantity object. \n"
     "can be used for simple unit translation like: \n"
     "parseQuantity('10m')\n"
     "or for more complex espressions:\n"
     "parseQuantity('sin(pi)/50.0 m/s^2')\n"},
    {"listSchemas",
     UnitsApiPy::sListSchemas,
     METH_VARARGS,
     "listSchemas() -> a tuple of schemas\n\n"
     "listSchemas(int) -> description of the given schema\n\n"},
    {"getSchema",
     UnitsApiPy::sGetSchema,
     METH_VARARGS,
     "getSchema() -> int\n\n"
     "The int is the position of the tuple returned by listSchemas"},
    {"setSchema",
     UnitsApiPy::sSetSchema,
     METH_VARARGS,
     "setSchema(int) -> None\n\n"
     "Sets the current schema to the given number, if possible"},
    {"schemaTranslate",
     UnitsApiPy::sSchemaTranslate,
     METH_VARARGS,
     "schemaTranslate(Quantity, int) -> tuple\n\n"
     "Translate a quantity to a given schema"},
    {"toNumber",
     UnitsApiPy::sToNumber,
     METH_VARARGS,
     "toNumber(Quantity or float, [format='g', decimals=-1]) -> str\n\n"
     "Convert a quantity or float to a string"},

    {nullptr, nullptr, 0, nullptr} /* Sentinel */
};

PyObject* UnitsApiPy::sParseQuantity(PyObject* /*self*/, PyObject* args)
{
    char* pstr {};
    if (!PyArg_ParseTuple(args, "et", "utf-8", &pstr)) {
        return nullptr;
    }

    Quantity rtn;
    std::string str(pstr);
    PyMem_Free(pstr);
    try {
        rtn = Quantity::parse(str);
    }
    catch (const Base::ParserError&) {
        PyErr_Format(PyExc_ValueError, "invalid unit expression \n");
        return nullptr;
    }

    return new QuantityPy(new Quantity(rtn));
}

PyObject* UnitsApiPy::sListSchemas(PyObject* /*self*/, PyObject* args)
{
    if (PyArg_ParseTuple(args, "")) {
        int num = static_cast<int>(UnitSystem::NumUnitSystemTypes);
        Py::Tuple tuple(num);
        for (int i = 0; i < num; i++) {
            const auto description {
                UnitsApi::getDescription(static_cast<UnitSystem>(i)).toStdString()};
            tuple.setItem(i, Py::String(description.c_str()));
        }

        return Py::new_reference_to(tuple);
    }

    PyErr_Clear();
    int index {};
    if (PyArg_ParseTuple(args, "i", &index)) {
        int num = static_cast<int>(UnitSystem::NumUnitSystemTypes);
        if (index < 0 || index >= num) {
            PyErr_SetString(PyExc_ValueError, "invalid schema value");
            return nullptr;
        }

        const auto description {
            UnitsApi::getDescription(static_cast<UnitSystem>(index)).toStdString()};
        return Py_BuildValue("s", description.c_str());
    }

    PyErr_SetString(PyExc_TypeError, "int or empty argument list expected");
    return nullptr;
}

PyObject* UnitsApiPy::sGetSchema(PyObject* /*self*/, PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    return Py_BuildValue("i", static_cast<int>(UnitsApi::getSchema()));
}

PyObject* UnitsApiPy::sSetSchema(PyObject* /*self*/, PyObject* args)
{
    PyErr_Clear();
    int index {};
    if (PyArg_ParseTuple(args, "i", &index)) {
        int num = static_cast<int>(UnitSystem::NumUnitSystemTypes);
        if (index < 0 || index >= num) {
            PyErr_SetString(PyExc_ValueError, "invalid schema value");
            return nullptr;
        }
        UnitsApi::setSchema(static_cast<UnitSystem>(index));
    }
    Py_Return;
}

PyObject* UnitsApiPy::sSchemaTranslate(PyObject* /*self*/, PyObject* args)
{
    PyObject* py {};
    int index {};
    if (!PyArg_ParseTuple(args, "O!i", &(QuantityPy::Type), &py, &index)) {
        return nullptr;
    }

    Quantity quant;
    quant = *static_cast<Base::QuantityPy*>(py)->getQuantityPtr();

    std::unique_ptr<UnitsSchema> schema(UnitsApi::createSchema(static_cast<UnitSystem>(index)));
    if (!schema) {
        PyErr_SetString(PyExc_ValueError, "invalid schema value");
        return nullptr;
    }

    double factor {};
    std::string uus;
    std::string uss = schema->schemaTranslate(quant, factor, uus);

    Py::Tuple res(3);
    res[0] = Py::String(uss, "utf-8");
    res[1] = Py::Float(factor);
    res[2] = Py::String(uus, "utf-8");

    return Py::new_reference_to(res);
}

PyObject* UnitsApiPy::sToNumber(PyObject* /*self*/, PyObject* args)
{
    double value {};
    const char* format = "g";
    int decimals {};
    do {
        PyObject* py {};
        if (PyArg_ParseTuple(args, "O!|si", &(QuantityPy::Type), &py, &format, &decimals)) {
            value = static_cast<QuantityPy*>(py)->getQuantityPtr()->getValue();
            break;
        }

        PyErr_Clear();
        if (PyArg_ParseTuple(args, "d|si", &value, &format, &decimals)) {
            break;
        }

        PyErr_SetString(PyExc_TypeError, "toNumber(Quantity or float, [format='g', decimals=-1])");
        return nullptr;
    } while (false);

    if (strlen(format) != 1) {
        PyErr_SetString(PyExc_ValueError, "Format string hasn't length of 1");
        return nullptr;
    }

    bool ok {};
    QuantityFormat qf;
    qf.format = QuantityFormat::toFormat(format[0], &ok);
    qf.precision = decimals;

    if (!ok) {
        PyErr_SetString(PyExc_ValueError, "Invalid format string");
        return nullptr;
    }

    return Py::new_reference_to(Py::String(UnitsApi::toNumber(value, qf)));
}
