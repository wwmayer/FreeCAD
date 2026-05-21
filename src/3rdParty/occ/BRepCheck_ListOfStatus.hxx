// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Werner Mayer <wmayer[at]users.sourceforge.net>
// SPDX-FileNotice: Part of the xwzCAD project.

#ifndef BRepCheck_ListOfStatus_HeaderFile
#define BRepCheck_ListOfStatus_HeaderFile

#include <BRepCheck_Status.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Shared.hxx>

using BRepCheck_ListOfStatus = NCollection_List<BRepCheck_Status>;
using BRepCheck_ListIteratorOfListOfStatus = BRepCheck_ListOfStatus::Iterator;
using BRepCheck_HListOfStatus = NCollection_Shared<BRepCheck_ListOfStatus>;


#endif
