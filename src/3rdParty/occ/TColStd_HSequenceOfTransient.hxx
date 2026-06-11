// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Werner Mayer <wmayer[at]users.sourceforge.net>
// SPDX-FileNotice: Part of the xwzCAD project.

#ifndef TColStd_HSequenceOfTransient_HeaderFile
#define TColStd_HSequenceOfTransient_HeaderFile

#include <Standard_Transient.hxx>
#include <TColStd_SequenceOfTransient.hxx>
#include <Standard_Version.hxx>
#if OCC_VERSION_HEX < 0x080000
#include <NCollection_DefineHSequence.hxx>

DEFINE_HSEQUENCE(TColStd_HSequenceOfTransient, TColStd_SequenceOfTransient)

#else

#include <NCollection_HSequence.hxx>

using TColStd_HSequenceOfTransient = NCollection_HSequence<opencascade::handle<Standard_Transient>>;

#endif

#endif
