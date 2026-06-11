// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Werner Mayer <wmayer[at]users.sourceforge.net>
// SPDX-FileNotice: Part of the xwzCAD project.

#ifndef TDF_IDList_HeaderFile
#define TDF_IDList_HeaderFile

#include <Standard_GUID.hxx>
#include <NCollection_List.hxx>

using TDF_IDList = NCollection_List<Standard_GUID>;
using TDF_ListIteratorOfIDList = TDF_IDList::Iterator;


#endif
