// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Werner Mayer <wmayer[at]users.sourceforge.net>
// SPDX-FileNotice: Part of the xwzCAD project.

#ifndef MeshVS_HArray1OfSequenceOfInteger_HeaderFile
#define MeshVS_HArray1OfSequenceOfInteger_HeaderFile

#include <MeshVS_Array1OfSequenceOfInteger.hxx>
#include <Standard_Version.hxx>
#if OCC_VERSION_HEX < 0x080000
#include <NCollection_DefineHArray1.hxx>

DEFINE_HARRAY1(MeshVS_HArray1OfSequenceOfInteger, MeshVS_Array1OfSequenceOfInteger)

#else
#include <TColStd_SequenceOfInteger.hxx>
#include <NCollection_HArray1.hxx>

using MeshVS_HArray1OfSequenceOfInteger = NCollection_HArray1<NCollection_Sequence<int>>;
#endif

#endif
