// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Werner Mayer <wmayer[at]users.sourceforge.net>
// SPDX-FileNotice: Part of the xwzCAD project.

#ifndef TopTools_DataMapOfIntegerListOfShape_HeaderFile
#define TopTools_DataMapOfIntegerListOfShape_HeaderFile

#include <TopTools_ListOfShape.hxx>
#include <NCollection_DataMap.hxx>

using TopTools_DataMapOfIntegerListOfShape = NCollection_DataMap<int,TopTools_ListOfShape>;
using TopTools_DataMapIteratorOfDataMapOfIntegerListOfShape = TopTools_DataMapOfIntegerListOfShape::Iterator;


#endif
