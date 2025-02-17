/***************************************************************************
 *   Copyright (c) 2010 Juergen Riegel <FreeCAD@juergen-riegel.net>        *
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
# include <gp_Lin.hxx>
# include <Precision.hxx>
# include <TopExp_Explorer.hxx>
# include <TopoDS.hxx>
#endif

#include <Base/Exception.h>
#include <Base/Tools.h>

#include "FeatureRevolved.h"
#include "Mod/Part/App/TopoShapeOpCode.h"

using namespace PartDesign;

namespace PartDesign {

/* TRANSLATOR PartDesign::Revolved */

PROPERTY_SOURCE_ABSTRACT(PartDesign::Revolved, PartDesign::ProfileBased)  // NOLINT

const App::PropertyAngle::Constraints Revolved::floatAngle = { 0.0, 360.0, 1.0 };

Revolved::Revolved()
{
    Angle.setConstraints(&floatAngle);
}

short Revolved::mustExecute() const
{
    if (Placement.isTouched() ||
        ReferenceAxis.isTouched() ||
        Axis.isTouched() ||
        Base.isTouched() ||
        UpToFace.isTouched() ||
        Angle.isTouched() ||
        Angle2.isTouched()) {
        return 1;
    }
    return ProfileBased::mustExecute();
}

App::DocumentObjectExecReturn* Revolved::executeRevolved(Part::RevolMode revolMode)
{
    try {
        return tryExecuteRevolved(revolMode);
    }
    catch (const Standard_Failure& e) {
        if (std::string(e.GetMessageString()) == "TopoDS::Face") {
            return new App::DocumentObjectExecReturn(
                QT_TRANSLATE_NOOP("Exception",
                                  "Could not create face from sketch.\n"
                                  "Intersecting sketch entities in a sketch are not allowed."));
        }

        return new App::DocumentObjectExecReturn(e.GetMessageString());
    }
    catch (const Base::Exception& e) {
        return new App::DocumentObjectExecReturn(e.what());
    }
}

App::DocumentObjectExecReturn *Revolved::tryExecuteRevolved(Part::RevolMode revolMode)
{
    if (onlyHasToRefine()){
        TopoShape result = refineShapeIfActive(rawShape);
        Shape.setValue(result);
        return App::DocumentObject::StdReturn;
    }

    constexpr double maxDegree = 360.0;
    auto method = methodFromString(Type.getValueAsString());

    // Validate parameters
    double angleDeg = Angle.getValue();
    if (angleDeg > maxDegree) {
        return new App::DocumentObjectExecReturn(
            QT_TRANSLATE_NOOP("Exception", "Angle of revolution too large"));
    }

    auto angle = Base::toRadians<double>(angleDeg);
    if (angle < Precision::Angular() && method == RevolMethod::Angle) {
        return new App::DocumentObjectExecReturn(
            QT_TRANSLATE_NOOP("Exception", "Angle of revolution too small"));
    }

    double angle2 = Base::toRadians(Angle2.getValue());
    if (std::fabs(angle + angle2) < Precision::Angular() && method == RevolMethod::TwoAngles) {
        return new App::DocumentObjectExecReturn(
            QT_TRANSLATE_NOOP("Exception", "Angles of revolution nullify each other"));
    }

    TopoShape sketchshape = getTopoShapeVerifiedFace();

    // if the Base property has a valid shape, fuse the AddShape into it
    TopoShape base = tryGetBaseShape();

    // update Axis from ReferenceAxis
    updateAxis();

    // get revolve axis
    Base::Vector3d b = Base.getValue();
    gp_Pnt pnt(b.x, b.y, b.z);
    Base::Vector3d v = Axis.getValue();

    if (v.IsNull()) {
        return new App::DocumentObjectExecReturn(
            QT_TRANSLATE_NOOP("Exception", "Reference axis is invalid"));
    }

    gp_Dir dir(v.x, v.y, v.z);

    if (sketchshape.isNull()) {
        return new App::DocumentObjectExecReturn(
            QT_TRANSLATE_NOOP("Exception", "Creating a face from sketch failed"));
    }

    this->positionByPrevious();
    auto invObjLoc = getLocation().Inverted();
    pnt.Transform(invObjLoc.Transformation());
    dir.Transform(invObjLoc.Transformation());
    base.move(invObjLoc);
    sketchshape.move(invObjLoc);

    // Check distance between sketchshape and axis - to avoid failures and crashes
    TopExp_Explorer xp;
    xp.Init(sketchshape.getShape(), TopAbs_FACE);
    for (; xp.More(); xp.Next()) {
        if (checkLineCrossesFace(gp_Lin(pnt, dir), TopoDS::Face(xp.Current()))) {
            return new App::DocumentObjectExecReturn(
                QT_TRANSLATE_NOOP("Exception", "Revolve axis intersects the sketch"));
        }
    }

    // Create a fresh support even when base exists so that it can be used for patterns
    TopoShape result(0);
    TopoShape supportface = tryGetSupportShape();

    supportface.move(invObjLoc);

    if (method == RevolMethod::ToFirst) {
        throw Base::RuntimeError("ProfileBased: Revolution up to first/last is not yet supported");
    }

    if (method == RevolMethod::ToFace) {
        result = tryToRevolveToFace(invObjLoc, pnt, dir, base, supportface, sketchshape, revolMode);
    }
    else {
        bool midplane = Midplane.getValue();
        bool reversed = Reversed.getValue();
        generateRevolution(result,
                           sketchshape.getShape(),
                           gp_Ax1(pnt, dir),
                           angle,
                           angle2,
                           midplane,
                           reversed,
                           method);
    }

    setResult(base, result);

    // eventually disable some settings that are not valid for the current method
    updateProperties(method);

    return App::DocumentObject::StdReturn;
}

void Revolved::setResult(const TopoShape& base, const TopoShape& revolved)
{
    if (!revolved.isNull()) {
        // store shape before refinement
        this->rawShape = revolved;
        TopoShape result = refineShapeIfActive(revolved);
        // set the additive shape property for later usage in e.g. pattern
        this->AddSubShape.setValue(result);

        if (!base.isNull()) {
            result = makeShape(base, result);
            // store shape before refinement
            this->rawShape = result;
            result = refineShapeIfActive(result);
        }
        if (!isSingleSolidRuleSatisfied(result.getShape())) {
            throw Base::RuntimeError(
                QT_TRANSLATE_NOOP("Exception",
                                  "Result has multiple solids: that is not currently supported."));
        }
        result = getSolid(result);
        this->Shape.setValue(result);
    }
    else {
        throw Base::RuntimeError(QT_TRANSLATE_NOOP("Exception", "Could not revolve the sketch!"));
    }
}

TopoShape Revolved::tryToRevolveToFace(const TopLoc_Location& invObjLoc,
                                       gp_Pnt pnt,
                                       gp_Dir dir,
                                       TopoShape& base,
                                       TopoShape& supportface,
                                       const TopoShape& sketchshape,
                                       Part::RevolMode revolMode) const
{
    TopoShape upToFace;
    getUpToFaceFromLinkSub(upToFace, UpToFace);
    upToFace.move(invObjLoc);

    if (Reversed.getValue()) {
        dir.Reverse();
    }

    TopExp_Explorer Ex(supportface.getShape(), TopAbs_WIRE);
    if (!Ex.More()) {
        supportface = TopoDS_Face();
    }

    try {
        return base.makeElementRevolution(base,
                            TopoDS::Face(sketchshape.getShape()),
                            gp_Ax1(pnt, dir),
                            TopoDS::Face(supportface.getShape()),
                            TopoDS::Face(upToFace.getShape()),
                            nullptr,
                            revolMode,
                            Standard_True);
    }
    catch (const Standard_Failure&) {
        throw Base::RuntimeError("Could not revolve the sketch!");
    }
}

TopoShape Revolved::tryGetBaseShape() const
{
    TopoShape base;
    try {
        base = getBaseTopoShape();
    }
    catch (const Base::Exception&) {
        // fall back to support (for legacy features)
    }

    return base;
}

TopoShape Revolved::tryGetSupportShape() const
{
    TopoShape supportface(0);
    try {
        supportface = getSupportFace();
    }
    catch (...) {
        //do nothing, null shape is handle below
    }

    return supportface;
}

bool Revolved::suggestReversed()
{
    try {
        updateAxis();
    }
    catch (const Base::Exception&) {
        return false;
    }

    double angle = ProfileBased::getReversedAngle(Base.getValue(), Axis.getValue());
    return suggestReversedAngle(angle);
}

void Revolved::updateAxis()
{
    App::DocumentObject *pcReferenceAxis = ReferenceAxis.getValue();
    const std::vector<std::string> &subReferenceAxis = ReferenceAxis.getSubValues();
    Base::Vector3d base;
    Base::Vector3d dir;
    getAxis(pcReferenceAxis, subReferenceAxis, base, dir, ForbiddenAxis::NotParallelWithNormal);

    Base.setValue(base.x,base.y,base.z);
    Axis.setValue(dir.x,dir.y,dir.z);
}

Revolved::RevolMethod Revolved::methodFromString(const std::string& methodStr)
{
    // clang-format off
    static constexpr auto numMethods {6};
    static constexpr std::array<std::tuple<std::string_view, Revolved::RevolMethod>, numMethods> methods {{
        {"Angle",      RevolMethod::Angle},
        {"UpToLast",   RevolMethod::ToLast},
        {"ThroughAll", RevolMethod::ThroughAll},
        {"UpToFirst",  RevolMethod::ToFirst},
        {"UpToFace",   RevolMethod::ToFace},
        {"TwoAngles",  RevolMethod::TwoAngles},
    }};
    // clang-format on

    auto it = std::find_if(methods.begin(), methods.end(), [methodStr](const auto& mode) {
        return std::get<0>(mode) == methodStr;
    });
    if (it != methods.end()) {
        return std::get<1>(*it);
    }

    throw Base::ValueError("Revolved: No such method");
}

void Revolved::generateRevolution(TopoShape& revol,
                                  const TopoShape& sketchshape,
                                  const gp_Ax1& axis,
                                  double angle,
                                  double angle2,
                                  bool midplane,
                                  bool reversed,
                                  RevolMethod method)
{
    if (method == RevolMethod::Angle ||
        method == RevolMethod::TwoAngles ||
        method == RevolMethod::ThroughAll) {
        double angleTotal = angle;
        double angleOffset = 0.;

        if (method == RevolMethod::TwoAngles) {
            // Rotate the face by `angle2`/`angle` to get "second" angle
            angleTotal += angle2;
            angleOffset = angle2 * -1.0;
        }
        else if (method == RevolMethod::ThroughAll) {
            angleTotal = 2 * M_PI;
        }
        else if (midplane) {
            // Rotate the face by half the angle to get Revolution symmetric to sketch plane
            angleOffset = -angle / 2;
        }

        if (std::fabs(angleTotal) < Precision::Angular()) {
            throw Base::ValueError("Cannot create a revolution with zero angle.");
        }

        gp_Ax1 revolAx(axis);
        if (reversed) {
            revolAx.Reverse();
        }

        TopoShape from = sketchshape;
        if (method == RevolMethod::TwoAngles || midplane) {
            gp_Trsf mov;
            mov.SetRotation(revolAx, angleOffset);
            TopLoc_Location loc(mov);
            from.move(loc);
        }

        // revolve the face to a solid
        // BRepPrimAPI is the only option that allows use of this shape for patterns.
        // See https://forum.freecadweb.org/viewtopic.php?f=8&t=70185&p=611673#p611673.
        revol = from;
        revol = revol.makeElementRevolve(revolAx,angleTotal);
        revol.Tag = -getID();
    }
    else {
        std::stringstream str;
        str << "ProfileBased: Internal error: Unknown method for generateRevolution()";
        throw Base::RuntimeError(str.str());
    }
}

void Revolved::generateRevolution(TopoShape& revol,
                                  const TopoShape& baseshape,
                                  const TopoDS_Shape& profileshape,
                                  const TopoDS_Face& supportface,
                                  const TopoDS_Face& uptoface,
                                  const gp_Ax1& axis,
                                  RevolMethod method,
                                  Part::RevolMode Mode,
                                  Standard_Boolean Modify)
{
    if (method == RevolMethod::ToFirst || method == RevolMethod::ToFace || method == RevolMethod::ToLast) {
        revol = revol.makeElementRevolution(baseshape, profileshape, axis, supportface, uptoface, nullptr,
                                            Mode, Modify, nullptr);
    }
    else {
        std::stringstream str;
        str << "ProfileBased: Internal error: Unknown method for generateRevolution()";
        throw Base::RuntimeError(str.str());
    }
}

void Revolved::updateProperties(RevolMethod method)
{
    // disable settings that are not valid on the current method
    // disable everything unless we are sure we need it
    bool isAngleEnabled = false;
    bool isAngle2Enabled = false;
    bool isMidplaneEnabled = false;
    bool isReversedEnabled = false;
    bool isUpToFaceEnabled = false;
    if (method == RevolMethod::Angle) {
        isAngleEnabled = true;
        isMidplaneEnabled = true;
        isReversedEnabled = !Midplane.getValue();
    }
    else if (method == RevolMethod::ThroughAll) {
        isMidplaneEnabled = true;
        isReversedEnabled = !Midplane.getValue();
    }
    else if (method == RevolMethod::ToFirst) {
        isReversedEnabled = true;
    }
    else if (method == RevolMethod::ToFace) {
        isReversedEnabled = true;
        isUpToFaceEnabled = true;
    }
    else if (method == RevolMethod::TwoAngles) {
        isAngleEnabled = true;
        isAngle2Enabled = true;
        isReversedEnabled = true;
    }

    Angle.setReadOnly(!isAngleEnabled);
    Angle2.setReadOnly(!isAngle2Enabled);
    Midplane.setReadOnly(!isMidplaneEnabled);
    Reversed.setReadOnly(!isReversedEnabled);
    UpToFace.setReadOnly(!isUpToFaceEnabled);
}

}
