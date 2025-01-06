// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2023 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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

#ifndef PART_PHYSICAL_EXTENSION_H
#define PART_PHYSICAL_EXTENSION_H

#include <App/DocumentObjectExtension.h>
#include <App/ExtensionPython.h>
#include <App/PropertyUnits.h>
#include <Mod/Part/PartGlobal.h>


namespace Part
{

class Feature;

class PartExport PhysicalExtension: public App::DocumentObjectExtension
{
    EXTENSION_PROPERTY_HEADER_WITH_OVERRIDE(Part::PhysicalExtension);

public:
    PhysicalExtension();

    App::PropertyString MaterialName;
    App::PropertyDensity Density;
    App::PropertyMass Mass;
    App::PropertyVolume Volume;

    App::DocumentObjectExecReturn *extensionExecute() override;

private:
    void updatePhysicalProperties(Feature* feature);
};

using PhysicalExtensionPython = App::ExtensionPythonT<PhysicalExtension>;

} // namespace Part

#endif // PART_PHYSICAL_EXTENSION_H
