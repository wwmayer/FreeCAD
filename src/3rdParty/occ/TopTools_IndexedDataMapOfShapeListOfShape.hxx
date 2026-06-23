// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Werner Mayer <wmayer[at]users.sourceforge.net>
// SPDX-FileNotice: Part of the xwzCAD project.

#ifndef TopTools_IndexedDataMapOfShapeListOfShape_HeaderFile
#define TopTools_IndexedDataMapOfShapeListOfShape_HeaderFile

#include <TopoDS_Shape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>

using TopTools_IndexedDataMapOfShapeListOfShape = NCollection_IndexedDataMap<TopoDS_Shape,TopTools_ListOfShape,TopTools_ShapeMapHasher>;


#endif
