// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Werner Mayer <wmayer[at]users.sourceforge.net>
// SPDX-FileNotice: Part of the xwzCAD project.

#ifndef TopTools_IndexedMapOfShape_HeaderFile
#define TopTools_IndexedMapOfShape_HeaderFile

#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>

using TopTools_IndexedMapOfShape = NCollection_IndexedMap<TopoDS_Shape,TopTools_ShapeMapHasher>;


#endif
