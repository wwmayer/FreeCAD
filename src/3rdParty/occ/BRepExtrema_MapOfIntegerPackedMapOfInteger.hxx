// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Werner Mayer <wmayer[at]users.sourceforge.net>
// SPDX-FileNotice: Part of the xwzCAD project.

#ifndef _BRepExtrema_MapOfIntegerPackedMapOfInteger_HeaderFile
#define _BRepExtrema_MapOfIntegerPackedMapOfInteger_HeaderFile

#include <NCollection_DataMap.hxx>
#include <TColStd_PackedMapOfInteger.hxx>

//! Set of overlapped sub-shapes.
using BRepExtrema_MapOfIntegerPackedMapOfInteger = NCollection_DataMap<int, TColStd_PackedMapOfInteger>;

#endif // _BRepExtrema_MapOfIntegerPackedMapOfInteger_HeaderFile
