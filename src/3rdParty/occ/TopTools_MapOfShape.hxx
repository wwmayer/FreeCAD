// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Werner Mayer <wmayer[at]users.sourceforge.net>
// SPDX-FileNotice: Part of the xwzCAD project.

#ifndef TopTools_MapOfShape_HeaderFile
#define TopTools_MapOfShape_HeaderFile

#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>

using TopTools_MapOfShape = NCollection_Map<TopoDS_Shape,TopTools_ShapeMapHasher>;
using TopTools_MapIteratorOfMapOfShape = TopTools_MapOfShape::Iterator;


#endif
