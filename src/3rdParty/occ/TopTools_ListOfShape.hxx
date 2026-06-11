// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Werner Mayer <wmayer[at]users.sourceforge.net>
// SPDX-FileNotice: Part of the xwzCAD project.

#ifndef TopTools_ListOfShape_HeaderFile
#define TopTools_ListOfShape_HeaderFile

#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>

using TopTools_ListOfShape = NCollection_List<TopoDS_Shape>;
using TopTools_ListIteratorOfListOfShape = TopTools_ListOfShape::Iterator;


#endif
