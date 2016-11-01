/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <wmayer[at]users.sourceforge.net>     *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"
#ifndef _PreComp_
# include <BRepAlgoAPI_Section.hxx>
# include <TopTools_ListOfShape.hxx>
# include <Standard_Version.hxx>
#endif

#include "FeaturePartSection.h"

#include <Base/Exception.h>

using namespace Part;

PROPERTY_SOURCE(Part::Section, Part::Boolean)


Section::Section(void)
{
}

BRepAlgoAPI_BooleanOperation* Section::makeOperation(const TopoDS_Shape& base, const TopoDS_Shape& tool) const
{
#if OCC_VERSION_HEX <= 0x060800
    // Let's call algorithm computing a section operation:
    return new BRepAlgoAPI_Section(base, tool);
#else
    TopTools_ListOfShape shapeArguments,shapeTools;
    shapeArguments.Append(base);
    shapeTools.Append(tool);

    BRepAlgoAPI_Section* mkSection = new BRepAlgoAPI_Section();
    mkSection->SetArguments(shapeArguments);
    mkSection->SetTools(shapeTools);
    mkSection->SetRunParallel(true);
    mkSection->Build();
    return mkSection;
#endif
}
