// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Werner Mayer <wmayer[at]users.sourceforge.net>
// SPDX-FileNotice: Part of the xwzCAD project.

#ifndef TopTools_DataMapOfShapeShape_HeaderFile
#define TopTools_DataMapOfShapeShape_HeaderFile

#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>

using TopTools_DataMapOfShapeShape = NCollection_DataMap<TopoDS_Shape,TopoDS_Shape,TopTools_ShapeMapHasher>;
using TopTools_DataMapIteratorOfDataMapOfShapeShape = TopTools_DataMapOfShapeShape::Iterator;


#endif
