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
#endif

#include "ElementResolver.h"
#include "GeoFeature.h"


using namespace App;

ElementResolver::ElementResolver(DocumentObject* obj, const std::string& name)
    : object{obj}
    , element{name}
{
    perform();
}

void ElementResolver::perform()
{
    std::pair<std::string, std::string> elementName;
    if (auto resolveObj = App::GeoFeature::resolveElement(object, element.c_str(), elementName)) {
        object = resolveObj;
        if (!elementName.second.empty()) {
            element = elementName.second;
        }
    }
}

DocumentObject* ElementResolver::getObject() const
{
    return object;
}

std::string ElementResolver::getElementName() const
{
    return element;
}
