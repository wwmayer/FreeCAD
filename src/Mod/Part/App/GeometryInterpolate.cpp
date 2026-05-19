// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2026 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
#include <memory>
#include <GeomAPI_Interpolate.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#endif

#include "GeometryInterpolate.h"

using namespace Part;

GeometryInterpolate::GeometryInterpolate()
    : tol3d{Precision::Approximation()}
    , periodic{false}
    , doScale{false}
{}

GeometryInterpolate::GeometryInterpolate(double tol3d, bool periodic)
    : tol3d{tol3d}
    , periodic{periodic}
    , doScale{false}
{}

void GeometryInterpolate::setTolerance(double value)
{
    tol3d = value;
}

void GeometryInterpolate::setPeriodic(bool value)
{
    periodic = value;
}

void GeometryInterpolate::setPoints(const std::vector<gp_Pnt>& pts)
{
    pnts = pts;
}

void GeometryInterpolate::setTangents(const std::vector<gp_Vec>& tts, bool scale)
{
    tnts = tts;
    loadts.assign(tts.size(), true);
    doScale = scale;
}

void GeometryInterpolate::setTangents(const std::vector<gp_Vec>& tts,
                                      const std::vector<bool>& flags,
                                      bool scale)
{
    if (tts.size() != flags.size()) {
        throw Standard_ConstructionError("Number of tangents doesn't match with number of flags");
    }

    tnts = tts;
    loadts = flags;
    doScale = scale;
}

void GeometryInterpolate::setTangents(const gp_Vec& initialTangent,
                                      const gp_Vec& finalTangent,
                                      bool scale)
{
    tnts.clear();
    loadts.clear();
    tnts.push_back(initialTangent);
    tnts.push_back(finalTangent);
    doScale = scale;
}

void GeometryInterpolate::setParameters(const std::vector<double>& values)
{
    prms = values;
}

bool GeometryInterpolate::hasTangents() const
{
    return !tnts.empty();
}

bool GeometryInterpolate::useTangentPerPoint() const
{
    return tnts.size() > 2;
}

bool GeometryInterpolate::hasInitialAndFinalTangent() const
{
    return tnts.size() == 2;
}

bool GeometryInterpolate::useOffset(int num) const
{
    return periodic && !hasTangents() && num > 2;
}

Handle(TColgp_HArray1OfPnt) GeometryInterpolate::getPoints(int& offset) const
{
    offset = 0;
    Handle(TColgp_HArray1OfPnt) pts;
    int numPts = static_cast<int>(pnts.size());
    if (useOffset(numPts)) {
        offset = numPts;
        pts = new TColgp_HArray1OfPnt(1, 3 * numPts);
        for (int i = 0; i < numPts; i++) {
            pts->SetValue(i + 1, pnts[i]);
            pts->SetValue(numPts + i + 1, pnts[i]);
            pts->SetValue(2 * numPts + i + 1, pnts[i]);
        }
    }
    else {
        pts = new TColgp_HArray1OfPnt(1, numPts);
        for (int i = 0; i < numPts; i++) {
            pts->SetValue(i + 1, pnts[i]);
        }
    }

    return pts;
}

TColgp_Array1OfVec GeometryInterpolate::getTangents() const
{
    TColgp_Array1OfVec tgs(1, static_cast<int>(tnts.size()));
    for (std::size_t i = 0; i < tnts.size(); i++) {
        tgs.SetValue(static_cast<int>(i + 1), tnts[i]);
    }
    return tgs;
}

Handle(TColStd_HArray1OfBoolean) GeometryInterpolate::useTangents() const
{
    Handle(TColStd_HArray1OfBoolean) fgs = new TColStd_HArray1OfBoolean(1, static_cast<int>(loadts.size()));
    for (std::size_t i = 0; i < loadts.size(); i++) {
        fgs->SetValue(static_cast<int>(i + 1), loadts[i]);
    }

    return fgs;
}

bool GeometryInterpolate::hasParameters() const
{
    return !prms.empty();
}

Handle(TColStd_HArray1OfReal) GeometryInterpolate::getParameters() const
{
    Handle(TColStd_HArray1OfReal) parameters;
    int numPar = static_cast<int>(prms.size());
    if (useOffset(numPar - 1)) {
        // According to GeomAPI_Interpolate the number of parameters must be higher by one than
        // the number of points if a periodic curve is required.
        // So the size of the array must be 3 * (numPar - 1) + 1
        int numPar_1 = numPar - 1;
        parameters = new TColStd_HArray1OfReal(1, 3 * numPar_1 + 1);
        for (int i = 0; i < numPar_1; i++) {
            parameters->SetValue(i + 1, prms[i]);
            parameters->SetValue(numPar_1 + i + 1, prms[i] + prms[numPar - 1]);
            parameters->SetValue(2 * numPar_1 + i + 1, prms[i] + 2 * prms[numPar - 1]);
        }

        parameters->SetValue(3 * numPar_1 + 1, 3 * prms[numPar - 1]);
    }
    else {
        parameters = new TColStd_HArray1OfReal(1, numPar);
        for (int i = 0; i < numPar; i++) {
            parameters->SetValue(i + 1, prms[i]);
        }
    }

    return parameters;
}

Handle(Geom_BSplineCurve) GeometryInterpolate::getSpline(Handle(Geom_BSplineCurve) spline,
                                                         int offset) const
{
    if (!periodic || offset == 0) {
        return spline;
    }

    TColgp_Array1OfPnt poles = spline->Poles();
    TColStd_Array1OfReal knots = spline->Knots();
    TColStd_Array1OfInteger mults = spline->Multiplicities();
    int degree = spline->Degree();

    TColgp_Array1OfPnt npoles(1, offset);
    for (int index = npoles.Lower(); index <= npoles.Upper(); ++index) {
        npoles(index) = poles(index + offset);
    }
    TColStd_Array1OfReal nknots(1, offset + 1);
    for (int index = nknots.Lower(); index <= nknots.Upper(); ++index) {
        nknots(index) = knots(index + offset);
    }
    TColStd_Array1OfInteger nmults(1, offset + 1);
    for (int index = nmults.Lower(); index <= nmults.Upper(); ++index) {
        nmults(index) = mults(index + offset);
    }

    return new Geom_BSplineCurve(npoles, nknots, nmults, degree, periodic);
}

Handle(Geom_BSplineCurve) GeometryInterpolate::perform() const
{
    if (pnts.size() < 2) {
        throw Standard_ConstructionError("Too few points for spline interpolation");
    }

    const bool tangentsPnt = useTangentPerPoint();

    if (tangentsPnt && tnts.size() != pnts.size()) {
        throw Standard_ConstructionError("Number of tangents doesn't match with number of points");
    }

    int offset = 0;
    std::unique_ptr<GeomAPI_Interpolate> interpolate;
    if (hasParameters()) {
        interpolate = std::make_unique<GeomAPI_Interpolate>(getPoints(offset),
                                                            getParameters(), periodic, tol3d);
    }
    else {
        interpolate = std::make_unique<GeomAPI_Interpolate>(getPoints(offset), periodic, tol3d);
    }

    if (tangentsPnt) {
        interpolate->Load(getTangents(), useTangents(), doScale);
    }
    else if (hasInitialAndFinalTangent()) {
        interpolate->Load(tnts.front(), tnts.back(), doScale);
    }

    interpolate->Perform();
    if (!interpolate->IsDone()) {
        throw Standard_ConstructionError("Failed to interpolate points");
    }

    return getSpline(interpolate->Curve(), offset);
}
