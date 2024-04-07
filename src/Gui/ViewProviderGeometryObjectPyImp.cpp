// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2024 Werner Mayer <wmayer[at]users.sourceforge.net>     *
 *                                                                         *
 *   This file is part of FreeCAD.                                         *
 *                                                                         *
 *   FreeCAD is free software: you can redistribute it and/or modify it    *
 *   under the terms of the GNU Lesser General Public License as           *
 *   published by the Free Software Foundation, either version 2.1 of the  *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful, but        *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with FreeCAD. If not, see                               *
 *   <https://www.gnu.org/licenses/>.                                      *
 *                                                                         *
 **************************************************************************/

#include "PreCompiled.h"

#ifndef _PreComp_
#include <sstream>
#endif

#include <App/GeoFeature.h>
#include <App/PropertyStandard.h>

#include "ViewProviderGeometryObjectPy.h"

#include "ViewProviderGeometryObjectPy.cpp"


using namespace Gui;

// returns a string which represents the object e.g. when printed in python
std::string ViewProviderGeometryObjectPy::representation() const
{
    std::stringstream str;
    str << "<View provider geometry object at " << getViewProviderGeometryObjectPtr() << ">";

    return str.str();
}

PyObject* ViewProviderGeometryObjectPy::getCustomAttributes(const char* attr) const
{
    (void)attr;
#if 0
    ViewProviderGeometryObject* vp = getViewProviderGeometryObjectPtr();
    if (strcmp(attr, "ShapeColor") == 0) {
        // ...
    }
    if (strcmp(attr, "ShapeMaterial") == 0) {
        // ...
    }
#endif
    return nullptr;
}

int ViewProviderGeometryObjectPy::setCustomAttributes(const char* attr, PyObject* obj)
{
    (void)attr;
    (void)obj;
#if 0
    ViewProviderGeometryObject* vp = getViewProviderGeometryObjectPtr();
    if (strcmp(attr, "ShapeColor") == 0) {
        // ...
        return 1;
    }
    if (strcmp(attr, "ShapeMaterial") == 0) {
        // ...
        return 1;
    }
#endif
    return 0;
}
