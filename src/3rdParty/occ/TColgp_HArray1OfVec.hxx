// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Werner Mayer <wmayer[at]users.sourceforge.net>
// SPDX-FileNotice: Part of the xwzCAD project.

#ifndef TColgp_HArray1OfVec_HeaderFile
#define TColgp_HArray1OfVec_HeaderFile

#include <TColgp_Array1OfVec.hxx>
#include <Standard_Version.hxx>
#if OCC_VERSION_HEX < 0x080000
#include <NCollection_DefineHArray1.hxx>

DEFINE_HARRAY1(TColgp_HArray1OfVec, TColgp_Array1OfVec)

#else
#include <gp_Vec.hxx>
#include <NCollection_HArray1.hxx>

using TColgp_HArray1OfVec = NCollection_HArray1<gp_Vec>;
#endif

#endif
