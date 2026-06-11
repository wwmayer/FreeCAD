// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Werner Mayer <wmayer[at]users.sourceforge.net>
// SPDX-FileNotice: Part of the xwzCAD project.

#ifndef TColStd_HArray1OfReal_HeaderFile
#define TColStd_HArray1OfReal_HeaderFile

#include <TColStd_Array1OfReal.hxx>
#include <Standard_Version.hxx>
#if OCC_VERSION_HEX < 0x080000
#include <NCollection_DefineHArray1.hxx>

DEFINE_HARRAY1(TColStd_HArray1OfReal, TColStd_Array1OfReal)

#else
#include <NCollection_HArray1.hxx>

using TColStd_HArray1OfReal = NCollection_HArray1<double>;

#endif

#endif
