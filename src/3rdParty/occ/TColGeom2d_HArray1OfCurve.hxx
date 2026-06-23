// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Werner Mayer <wmayer[at]users.sourceforge.net>
// SPDX-FileNotice: Part of the xwzCAD project.

#ifndef TColGeom2d_HArray1OfCurve_HeaderFile
#define TColGeom2d_HArray1OfCurve_HeaderFile

#include <TColGeom2d_Array1OfCurve.hxx>
#include <Standard_Version.hxx>
#if OCC_VERSION_HEX < 0x080000
#include <NCollection_DefineHArray1.hxx>

DEFINE_HARRAY1(TColGeom2d_HArray1OfCurve, TColGeom2d_Array1OfCurve)

#else
#include <NCollection_HArray1.hxx>

using TColGeom2d_HArray1OfCurve = NCollection_HArray1<opencascade::handle<Geom2d_Curve>>;
#endif

#endif
