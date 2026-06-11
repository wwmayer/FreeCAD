// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Werner Mayer <wmayer[at]users.sourceforge.net>
// SPDX-FileNotice: Part of the xwzCAD project.

#ifndef TopTools_DataMapOfShapeListOfShape_HeaderFile
#define TopTools_DataMapOfShapeListOfShape_HeaderFile

#include <TopoDS_Shape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>

using TopTools_DataMapOfShapeListOfShape = NCollection_DataMap<TopoDS_Shape,TopTools_ListOfShape,TopTools_ShapeMapHasher>;
using TopTools_DataMapIteratorOfDataMapOfShapeListOfShape = TopTools_DataMapOfShapeListOfShape::Iterator;


#endif
