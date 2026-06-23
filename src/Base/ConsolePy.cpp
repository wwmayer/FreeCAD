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

#include "PreCompiled.h"

#ifndef _PreComp_
#include <cstring>
#include <functional>
#endif

#include "ConsolePy.h"
#include "Console.h"
#include "PyObjectBase.h"


using namespace Base;


//**************************************************************************
// Python stuff

PyDoc_STRVAR(Console_doc,
    "FreeCAD Console module.\n\n"
    "The Console module contains functions to manage log entries, messages,\n"
    "warnings and errors.\n"
    "There are also functions to get/set the status of the observers used as\n"
    "logging interfaces."
);

PyObject* ConsolePy::createModule()
{
    static struct PyModuleDef ConsoleModuleDef = {
        PyModuleDef_HEAD_INIT,
        "__FreeCADConsole__", Console_doc, -1,
        Base::ConsolePy::Methods,
        nullptr, nullptr, nullptr, nullptr
    };

    return PyModule_Create(&ConsoleModuleDef);
}

// ConsoleSingleton Methods structure
PyMethodDef ConsolePy::Methods[] = {
    {"PrintMessage",
     ConsolePy::sPyMessage,
     METH_VARARGS,
     "PrintMessage(obj) -> None\n\n"
     "Print a message to the output.\n\n"
     "obj : object\n    The string representation is printed."},
    {"PrintLog",
     ConsolePy::sPyLog,
     METH_VARARGS,
     "PrintLog(obj) -> None\n\n"
     "Print a log message to the output.\n\n"
     "obj : object\n    The string representation is printed."},
    {"PrintError",
     ConsolePy::sPyError,
     METH_VARARGS,
     "PrintError(obj) -> None\n\n"
     "Print an error message to the output.\n\n"
     "obj : object\n    The string representation is printed."},
    {"PrintDeveloperError",
     ConsolePy::sPyDeveloperError,
     METH_VARARGS,
     "PrintDeveloperError(obj) -> None\n\n"
     "Print an error message intended only for Developers to the output.\n\n"
     "obj : object\n    The string representation is printed."},
    {"PrintUserError",
     ConsolePy::sPyUserError,
     METH_VARARGS,
     "PrintUserError(obj) -> None\n\n"
     "Print an error message intended only for the User to the output.\n\n"
     "obj : object\n    The string representation is printed."},
    {"PrintTranslatedUserError",
     ConsolePy::sPyTranslatedUserError,
     METH_VARARGS,
     "PrintTranslatedUserError(obj) -> None\n\n"
     "Print an already translated error message intended only for the User to the output.\n\n"
     "obj : object\n    The string representation is printed."},
    {"PrintWarning",
     ConsolePy::sPyWarning,
     METH_VARARGS,
     "PrintWarning(obj) -> None\n\n"
     "Print a warning message to the output.\n\n"
     "obj : object\n    The string representation is printed."},
    {"PrintDeveloperWarning",
     ConsolePy::sPyDeveloperWarning,
     METH_VARARGS,
     "PrintDeveloperWarning(obj) -> None\n\n"
     "Print an warning message intended only for Developers to the output.\n\n"
     "obj : object\n    The string representation is printed."},
    {"PrintUserWarning",
     ConsolePy::sPyUserWarning,
     METH_VARARGS,
     "PrintUserWarning(obj) -> None\n\n"
     "Print a warning message intended only for the User to the output.\n\n"
     "obj : object\n    The string representation is printed."},
    {"PrintTranslatedUserWarning",
     ConsolePy::sPyTranslatedUserWarning,
     METH_VARARGS,
     "PrintTranslatedUserWarning(obj) -> None\n\n"
     "Print an already translated warning message intended only for the User to the output.\n\n"
     "obj : object\n    The string representation is printed."},
    {"PrintCritical",
     ConsolePy::sPyCritical,
     METH_VARARGS,
     "PrintCritical(obj) -> None\n\n"
     "Print a critical message to the output.\n\n"
     "obj : object\n    The string representation is printed."},
    {"PrintNotification",
     ConsolePy::sPyNotification,
     METH_VARARGS,
     "PrintNotification(obj) -> None\n\n"
     "Print a user notification to the output.\n\n"
     "obj : object\n    The string representation is printed."},
    {"PrintTranslatedNotification",
     ConsolePy::sPyTranslatedNotification,
     METH_VARARGS,
     "PrintTranslatedNotification(obj) -> None\n\n"
     "Print an already translated notification to the output.\n\n"
     "obj : object\n    The string representation is printed."},
    {"SetStatus",
     ConsolePy::sPySetStatus,
     METH_VARARGS,
     "SetStatus(observer, type, status) -> None\n\n"
     "Set the status for either 'Log', 'Msg', 'Wrn' or 'Error' for an observer.\n\n"
     "observer : str\n    Logging interface name.\n"
     "type : str\n    Message type.\n"
     "status : bool"},
    {"GetStatus",
     ConsolePy::sPyGetStatus,
     METH_VARARGS,
     "GetStatus(observer, type) -> bool or None\n\n"
     "Get the status for either 'Log', 'Msg', 'Wrn' or 'Error' for an observer.\n"
     "Returns None if the specified observer doesn't exist.\n\n"
     "observer : str\n    Logging interface name.\n"
     "type : str\n    Message type."},
    {"GetObservers",
     ConsolePy::sPyGetObservers,
     METH_VARARGS,
     "GetObservers() -> list of str\n\n"
     "Get the names of the current logging interfaces."},
    {nullptr, nullptr, 0, nullptr} /* Sentinel */
};

namespace
{
PyObject* FC_PYCONSOLE_MSG(std::function<void(const char*, const char*)> func, PyObject* args)
{
    PyObject* output {};
    PyObject* notifier {};

    const char* notifierStr = "";

    auto retrieveString = [](PyObject* pystr) {
        PyObject* unicode = nullptr;

        const char* outstr = nullptr;

        if (PyUnicode_Check(pystr)) {
            outstr = PyUnicode_AsUTF8(pystr);
        }
        else {
            unicode = PyObject_Str(pystr);
            if (unicode) {
                outstr = PyUnicode_AsUTF8(unicode);
            }
        }

        Py_XDECREF(unicode);

        return outstr;
    };


    if (!PyArg_ParseTuple(args, "OO", &notifier, &output)) {
        PyErr_Clear();
        if (!PyArg_ParseTuple(args, "O", &output)) {
            return nullptr;
        }
    }
    else {  // retrieve notifier
        PY_TRY
        {
            notifierStr = retrieveString(notifier);
        }
        PY_CATCH
    }

    PY_TRY
    {
        const char* string = retrieveString(output);

        if (string) {
            func(notifierStr, string); /*process message*/
        }
    }
    PY_CATCH
    Py_Return;
}
}  // namespace

PyObject* ConsolePy::sPyMessage(PyObject* /*self*/, PyObject* args)
{
    return FC_PYCONSOLE_MSG(
        [](const std::string& notifier, const char* msg) {
            Console()
                .Send<Base::LogStyle::Message,
                      Base::IntendedRecipient::Developer,
                      Base::ContentType::Untranslatable>(notifier, "%s", msg);
        },
        args);
}

PyObject* ConsolePy::sPyWarning(PyObject* /*self*/, PyObject* args)
{
    return FC_PYCONSOLE_MSG(
        [](const std::string& notifier, const char* msg) {
            Console().Warning(notifier, "%s", msg);
        },
        args);
}

PyObject* ConsolePy::sPyDeveloperWarning(PyObject* /*self*/, PyObject* args)
{
    return FC_PYCONSOLE_MSG(
        [](const std::string& notifier, const char* msg) {
            Console()
                .Send<Base::LogStyle::Warning,
                      Base::IntendedRecipient::Developer,
                      Base::ContentType::Untranslatable>(notifier, "%s", msg);
        },
        args);
}

PyObject* ConsolePy::sPyUserWarning(PyObject* /*self*/, PyObject* args)
{
    return FC_PYCONSOLE_MSG(
        [](const std::string& notifier, const char* msg) {
            Console()
                .Send<Base::LogStyle::Warning,
                      Base::IntendedRecipient::User,
                      Base::ContentType::Untranslated>(notifier, "%s", msg);
        },
        args);
}

PyObject* ConsolePy::sPyTranslatedUserWarning(PyObject* /*self*/, PyObject* args)
{
    return FC_PYCONSOLE_MSG(
        [](const std::string& notifier, const char* msg) {
            Console()
                .Send<Base::LogStyle::Warning,
                      Base::IntendedRecipient::User,
                      Base::ContentType::Translated>(notifier, "%s", msg);
        },
        args);
}

PyObject* ConsolePy::sPyError(PyObject* /*self*/, PyObject* args)
{
    return FC_PYCONSOLE_MSG(
        [](const std::string& notifier, const char* msg) {
            Console()
                .Send<Base::LogStyle::Error,
                      Base::IntendedRecipient::All,
                      Base::ContentType::Untranslated>(notifier, "%s", msg);
        },
        args);
}

PyObject* ConsolePy::sPyDeveloperError(PyObject* /*self*/, PyObject* args)
{
    return FC_PYCONSOLE_MSG(
        [](const std::string& notifier, const char* msg) {
            Console()
                .Send<Base::LogStyle::Error,
                      Base::IntendedRecipient::Developer,
                      Base::ContentType::Untranslatable>(notifier, "%s", msg);
        },
        args);
}

PyObject* ConsolePy::sPyUserError(PyObject* /*self*/, PyObject* args)
{
    return FC_PYCONSOLE_MSG(
        [](const std::string& notifier, const char* msg) {
            Console()
                .Send<Base::LogStyle::Error,
                      Base::IntendedRecipient::User,
                      Base::ContentType::Untranslated>(notifier, "%s", msg);
        },
        args);
}

PyObject* ConsolePy::sPyTranslatedUserError(PyObject* /*self*/, PyObject* args)
{
    return FC_PYCONSOLE_MSG(
        [](const std::string& notifier, const char* msg) {
            Console()
                .Send<Base::LogStyle::Error,
                      Base::IntendedRecipient::User,
                      Base::ContentType::Translated>(notifier, "%s", msg);
        },
        args);
}

PyObject* ConsolePy::sPyLog(PyObject* /*self*/, PyObject* args)
{
    return FC_PYCONSOLE_MSG(
        [](const std::string& notifier, const char* msg) {
            Console()
                .Send<Base::LogStyle::Log,
                      Base::IntendedRecipient::Developer,
                      Base::ContentType::Untranslatable>(notifier, "%s", msg);
        },
        args);
}

PyObject* ConsolePy::sPyCritical(PyObject* /*self*/, PyObject* args)
{
    return FC_PYCONSOLE_MSG(
        [](const std::string& notifier, const char* msg) {
            Console()
                .Send<Base::LogStyle::Critical,
                      Base::IntendedRecipient::All,
                      Base::ContentType::Untranslated>(notifier, "%s", msg);
        },
        args);
}

PyObject* ConsolePy::sPyNotification(PyObject* /*self*/, PyObject* args)
{
    return FC_PYCONSOLE_MSG(
        [](const std::string& notifier, const char* msg) {
            Console()
                .Send<Base::LogStyle::Notification,
                      Base::IntendedRecipient::User,
                      Base::ContentType::Untranslated>(notifier, "%s", msg);
        },
        args);
}

PyObject* ConsolePy::sPyTranslatedNotification(PyObject* /*self*/, PyObject* args)
{
    return FC_PYCONSOLE_MSG(
        [](const std::string& notifier, const char* msg) {
            Console()
                .Send<Base::LogStyle::Notification,
                      Base::IntendedRecipient::User,
                      Base::ContentType::Translated>(notifier, "%s", msg);
        },
        args);
}

PyObject* ConsolePy::sPyGetStatus(PyObject* /*self*/, PyObject* args)
{
    char* pstr1 {};
    char* pstr2 {};
    if (!PyArg_ParseTuple(args, "ss", &pstr1, &pstr2)) {
        return nullptr;
    }

    PY_TRY
    {
        bool b = false;
        ILogger* pObs = Console().Get(pstr1);
        if (!pObs) {
            Py_Return;
        }

        if (strcmp(pstr2, "Log") == 0) {
            b = pObs->bLog;
        }
        else if (strcmp(pstr2, "Wrn") == 0) {
            b = pObs->bWrn;
        }
        else if (strcmp(pstr2, "Msg") == 0) {
            b = pObs->bMsg;
        }
        else if (strcmp(pstr2, "Err") == 0) {
            b = pObs->bErr;
        }
        else if (strcmp(pstr2, "Critical") == 0) {
            b = pObs->bCritical;
        }
        else if (strcmp(pstr2, "Notification") == 0) {
            b = pObs->bNotification;
        }
        else {
            Py_Error(Base::PyExc_FC_GeneralError,
                     "Unknown message type (use 'Log', 'Err', 'Wrn', 'Msg', 'Critical' or "
                     "'Notification')");
        }

        return PyBool_FromLong(b ? 1 : 0);
    }
    PY_CATCH;
}

PyObject* ConsolePy::sPySetStatus(PyObject* /*self*/, PyObject* args)
{
    char* pstr1 {};
    char* pstr2 {};
    PyObject* pyStatus {};
    if (!PyArg_ParseTuple(args, "ssO!", &pstr1, &pstr2, &PyBool_Type, &pyStatus)) {
        return nullptr;
    }

    PY_TRY
    {
        bool status = asBoolean(pyStatus);
        ILogger* pObs = Console().Get(pstr1);
        if (pObs) {
            if (strcmp(pstr2, "Log") == 0) {
                pObs->bLog = status;
            }
            else if (strcmp(pstr2, "Wrn") == 0) {
                pObs->bWrn = status;
            }
            else if (strcmp(pstr2, "Msg") == 0) {
                pObs->bMsg = status;
            }
            else if (strcmp(pstr2, "Err") == 0) {
                pObs->bErr = status;
            }
            else if (strcmp(pstr2, "Critical") == 0) {
                pObs->bCritical = status;
            }
            else if (strcmp(pstr2, "Notification") == 0) {
                pObs->bNotification = status;
            }
            else {
                Py_Error(Base::PyExc_FC_GeneralError,
                         "Unknown message type (use 'Log', 'Err', 'Wrn', 'Msg', 'Critical' or "
                         "'Notification')");
            }

            Py_Return;
        }

        Py_Error(Base::PyExc_FC_GeneralError, "Unknown logger type");
    }
    PY_CATCH;
}

PyObject* ConsolePy::sPyGetObservers(PyObject* /*self*/, PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    PY_TRY
    {
        Py::List list;
        for (auto i : Console()._aclObservers) {
            list.append(Py::String(i->Name() ? i->Name() : ""));
        }

        return Py::new_reference_to(list);
    }
    PY_CATCH
}
