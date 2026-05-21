// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Werner Mayer <wmayer[at]users.sourceforge.net>
// SPDX-FileNotice: Part of the xwzCAD project.

#ifndef TColStd_IndexedDataMapOfStringString_HeaderFile
#define TColStd_IndexedDataMapOfStringString_HeaderFile

#include <TCollection_AsciiString.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <Standard_Version.hxx>

#if OCC_VERSION_HEX < 0x070800
using TColStd_IndexedDataMapOfStringString = NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString, TCollection_AsciiString>;
#else
using TColStd_IndexedDataMapOfStringString = NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString>;
#endif

#endif // TColStd_IndexedDataMapOfStringString_HeaderFile
