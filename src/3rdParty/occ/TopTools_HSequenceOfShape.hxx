
#ifndef TopTools_HSequenceOfShape_HeaderFile
#define TopTools_HSequenceOfShape_HeaderFile

#include <TopoDS_Shape.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <Standard_Version.hxx>
#if OCC_VERSION_HEX < 0x080000
#include <NCollection_DefineHSequence.hxx>

DEFINE_HSEQUENCE(TopTools_HSequenceOfShape, TopTools_SequenceOfShape)

#else
#include <NCollection_HSequence.hxx>

using TopTools_HSequenceOfShape = NCollection_HSequence<TopoDS_Shape>;

#endif

#endif
