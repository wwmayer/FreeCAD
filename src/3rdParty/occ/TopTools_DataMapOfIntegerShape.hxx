// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Werner Mayer <wmayer[at]users.sourceforge.net>
// SPDX-FileNotice: Part of the xwzCAD project.

#ifndef TopTools_DataMapOfIntegerShape_HeaderFile
#define TopTools_DataMapOfIntegerShape_HeaderFile

#include <TopoDS_Shape.hxx>
#include <NCollection_DataMap.hxx>

using TopTools_DataMapOfIntegerShape = NCollection_DataMap<int,TopoDS_Shape>;
using TopTools_DataMapIteratorOfDataMapOfIntegerShape = TopTools_DataMapOfIntegerShape::Iterator;


#endif
