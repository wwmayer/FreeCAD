// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Werner Mayer <wmayer[at]users.sourceforge.net>
// SPDX-FileNotice: Part of the xwzCAD project.

#ifndef TColgp_HArray1OfPnt_HeaderFile
#define TColgp_HArray1OfPnt_HeaderFile

#include <gp_Pnt.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <Standard_Version.hxx>
#if OCC_VERSION_HEX < 0x080000

#include <NCollection_DefineHArray1.hxx>

DEFINE_HARRAY1(TColgp_HArray1OfPnt, TColgp_Array1OfPnt)

#else
#include <NCollection_HArray1.hxx>

using TColgp_HArray1OfPnt = NCollection_HArray1<gp_Pnt>;
#endif

#endif
