// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Werner Mayer <wmayer[at]users.sourceforge.net>
// SPDX-FileNotice: Part of the xwzCAD project.

#ifndef TColStd_HSequenceOfReal_HeaderFile
#define TColStd_HSequenceOfReal_HeaderFile

#include <TColStd_SequenceOfReal.hxx>
#include <Standard_Version.hxx>
#if OCC_VERSION_HEX < 0x080000
#include <NCollection_DefineHSequence.hxx>

DEFINE_HSEQUENCE(TColStd_HSequenceOfReal, TColStd_SequenceOfReal)
#else
#include <NCollection_HSequence.hxx>
using TColStd_HSequenceOfReal = NCollection_HSequence<double>;
#endif


#endif
