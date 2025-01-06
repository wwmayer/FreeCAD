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

#include "PreCompiled.h"
#ifndef _PreComp_
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#endif

#include <Base/Tools.h>

#include "PartFeature.h"
#include "PhysicalExtension.h"


using namespace Part;

EXTENSION_PROPERTY_SOURCE(Part::PhysicalExtension, App::DocumentObjectExtension)

constexpr const int MaterialPrecision = 6;

PhysicalExtension::PhysicalExtension()
{
    // clang-format off
    Base::QuantityFormat format(Base::QuantityFormat::NumberFormat::Default, MaterialPrecision);

    // Read only properties based on the material
    static const char* group = "PhysicalProperties";
    EXTENSION_ADD_PROPERTY_TYPE(MaterialName, (""), group,
        static_cast<App::PropertyType>(App::Prop_ReadOnly | App::Prop_Output |
                                       App::Prop_NoRecompute | App::Prop_NoPersist),
        "Feature material");
    EXTENSION_ADD_PROPERTY_TYPE(Density, (0.0), group,
        static_cast<App::PropertyType>(App::Prop_ReadOnly | App::Prop_Output |
                                       App::Prop_NoRecompute | App::Prop_NoPersist),
        "Feature density");
    Density.setFormat(format);
    EXTENSION_ADD_PROPERTY_TYPE(Mass, (0.0), group,
        static_cast<App::PropertyType>(App::Prop_ReadOnly | App::Prop_Output |
                                       App::Prop_NoRecompute | App::Prop_NoPersist),
        "Feature mass");
    Mass.setFormat(format);
    EXTENSION_ADD_PROPERTY_TYPE(Volume, (1.0), group,
         static_cast<App::PropertyType>(App::Prop_ReadOnly | App::Prop_Output |
                                        App::Prop_NoRecompute | App::Prop_NoPersist),
         "Feature volume");
    Volume.setFormat(format);
    // clang-format on

    initExtensionType(PhysicalExtension::getExtensionClassTypeId());
}

App::DocumentObjectExecReturn *PhysicalExtension::extensionExecute()
{
    if (auto feature = dynamic_cast<Feature*>(getExtendedContainer())) {
        updatePhysicalProperties(feature);
    }
    return App::DocumentObjectExtension::extensionExecute();
}

void PhysicalExtension::updatePhysicalProperties(Feature* feature)
{
    MaterialName.setValue(feature->ShapeMaterial.getValue().getName().toStdString());
    if (feature->ShapeMaterial.getValue().hasPhysicalProperty(QString::fromLatin1("Density"))) {
        Density.setValue(feature->ShapeMaterial.getValue()
                             .getPhysicalQuantity(QString::fromLatin1("Density"))
                             .getValue());
    }
    else {
        Base::Console().Log("Density is undefined\n");
        Density.setValue(0.0);
    }

    auto topoShape = feature->Shape.getValue();
    if (!topoShape.IsNull()) {
        GProp_GProps props;
        BRepGProp::VolumeProperties(topoShape, props);
        Volume.setValue(props.Mass());
        Mass.setValue(Volume.getValue() * Density.getValue());
    }
    else {
        // No shape
        Volume.setValue(0.0);
        Mass.setValue(0.0);
    }
}

namespace App
{
    /// @cond DOXERR
    EXTENSION_PROPERTY_SOURCE_TEMPLATE(Part::PhysicalExtensionPython, Part::PhysicalExtension)
    /// @endcond

    // explicit template instantiation
    template class PartExport ExtensionPythonT<Part::PhysicalExtension>;
}
