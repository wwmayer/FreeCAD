// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Werner Mayer <wmayer[at]users.sourceforge.net>
// SPDX-FileNotice: Part of the xwzCAD project.

#ifndef TColgp_HArray2OfPnt_HeaderFile
#define TColgp_HArray2OfPnt_HeaderFile

#include <gp_Pnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <Standard_Version.hxx>
#if OCC_VERSION_HEX < 0x080000

#include <NCollection_DefineHArray2.hxx>

DEFINE_HARRAY2(TColgp_HArray2OfPnt, TColgp_Array2OfPnt)

#else
#include <NCollection_HArray2.hxx>

using TColgp_HArray2OfPnt = NCollection_HArray2<gp_Pnt>;
#endif

#endif
