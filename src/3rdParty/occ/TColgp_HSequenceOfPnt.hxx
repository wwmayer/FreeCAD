// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Werner Mayer <wmayer[at]users.sourceforge.net>
// SPDX-FileNotice: Part of the xwzCAD project.

#ifndef TColgp_HSequenceOfPnt_HeaderFile
#define TColgp_HSequenceOfPnt_HeaderFile

#include <gp_Pnt.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <Standard_Version.hxx>
#if OCC_VERSION_HEX < 0x080000
#include <NCollection_DefineHSequence.hxx>

DEFINE_HSEQUENCE(TColgp_HSequenceOfPnt, TColgp_SequenceOfPnt)

#else
#include <NCollection_HSequence.hxx>

using TColgp_HSequenceOfPnt = NCollection_HSequence<gp_Pnt>;
#endif

#endif
