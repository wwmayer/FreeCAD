// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Werner Mayer <wmayer[at]users.sourceforge.net>
// SPDX-FileNotice: Part of the xwzCAD project.

#ifndef TopTools_DataMapOfShapeReal_HeaderFile
#define TopTools_DataMapOfShapeReal_HeaderFile

#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>

using TopTools_DataMapOfShapeReal = NCollection_DataMap<TopoDS_Shape, double, TopTools_ShapeMapHasher>;
using TopTools_DataMapIteratorOfDataMapOfShapeReal = TopTools_DataMapOfShapeReal::Iterator;


#endif
