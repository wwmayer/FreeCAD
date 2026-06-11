// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Werner Mayer <wmayer[at]users.sourceforge.net>
// SPDX-FileNotice: Part of the xwzCAD project.

#ifndef TColStd_ListOfTransient_HeaderFile
#define TColStd_ListOfTransient_HeaderFile

#include <Standard_Transient.hxx>
#include <NCollection_List.hxx>

using TColStd_ListOfTransient = NCollection_List<Handle(Standard_Transient)>;
using TColStd_ListIteratorOfListOfTransient =  TColStd_ListOfTransient::Iterator;


#endif
