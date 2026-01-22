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

#ifndef PART_GEOMETRY_INTERPOLATE_H
#define PART_GEOMETRY_INTERPOLATE_H

#include <Geom_BSplineCurve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColStd_HArray1OfBoolean.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <vector>
#include <Mod/Part/PartGlobal.h>

namespace Part {

class PartExport GeometryInterpolate
{
public:
    GeometryInterpolate();
    explicit GeometryInterpolate(double tol3d, bool periodic);
    void setTolerance(double value);
    void setPeriodic(bool value);
    void setPoints(const std::vector<gp_Pnt>& pts);
    void setTangents(const std::vector<gp_Vec>& tts, bool scale = true);
    void setTangents(const std::vector<gp_Vec>& tts,
                     const std::vector<bool>& flags,
                     bool scale = true);
    void setTangents(const gp_Vec& initialTangent, const gp_Vec& finalTangent, bool scale = true);
    void setParameters(const std::vector<double>& values);
    Handle(Geom_BSplineCurve) perform() const;

private:
    bool useTangentPerPoint() const;
    bool hasInitialAndFinalTangent() const;
    Handle(TColgp_HArray1OfPnt) getPoints() const;
    TColgp_Array1OfVec getTangents() const;
    Handle(TColStd_HArray1OfBoolean) useTangents() const;
    bool hasParameters() const;
    Handle(TColStd_HArray1OfReal) getParameters() const;
    Handle(Geom_BSplineCurve) getSpline(Handle(Geom_BSplineCurve) spline) const;

private:
    double tol3d;
    bool periodic;
    bool doScale;
    std::vector<gp_Pnt> pnts;
    std::vector<gp_Vec> tnts;
    std::vector<bool> loadts;
    std::vector<double> prms;
};

}

#endif // PART_GEOMETRY_INTERPOLATE_H
