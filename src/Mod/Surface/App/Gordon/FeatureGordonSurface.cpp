// SPDX-License-Identifier: LGPL-2.1-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2025 Andrew Shkolik <shkolik@gmail.com>                  *
 *                                                                          *
 *   This file is part of FreeCAD.                                          *
 *                                                                          *
 *   FreeCAD is free software: you can redistribute it and/or modify it     *
 *   under the terms of the GNU Lesser General Public License as            *
 *   published by the Free Software Foundation, either version 2.1 of the   *
 *   License, or (at your option) any later version.                        *
 *                                                                          *
 *   FreeCAD is distributed in the hope that it will be useful, but         *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of             *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU       *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU Lesser General Public       *
 *   License along with FreeCAD. If not, see                                *
 *   <https://www.gnu.org/licenses/>.                                       *
 *                                                                          *
 ***************************************************************************/

#include "PreCompiled.h"
#ifndef _PreComp_
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <Convert_ParameterisationType.hxx>
#include <GeomConvert.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <ShapeConstruct_Curve.hxx>
#include <Standard_Version.hxx>
#include <TopoDS.hxx>
#include <gp_Pnt.hxx>
#endif

#include "FeatureGordonSurface.h"
#include "occ_gordon/src/occ_gordon/occ_gordon.h"


using namespace Surface;

PROPERTY_SOURCE(Surface::GordonSurface, Part::Spline)

GordonSurface::GordonSurface()
{
    ADD_PROPERTY_TYPE(ProfileEdges, (nullptr, ""), "GordonSurface", App::Prop_None, "Profiles edges.");
    ADD_PROPERTY_TYPE(GuideEdges, (nullptr, ""), "GordonSurface", App::Prop_None, "Guide edges.");
    ADD_PROPERTY_TYPE(Tolerance, (Precision::Approximation()), "GordonSurface", App::Prop_None, "Tolerance");

    ProfileEdges.setScope(App::LinkScope::Global);
    GuideEdges.setScope(App::LinkScope::Global);

    ProfileEdges.setSize(0);
    GuideEdges.setSize(0);
    Tolerance.setValue(Precision::Approximation());
}

short GordonSurface::mustExecute() const
{
    if (ProfileEdges.isTouched() || GuideEdges.isTouched() || Tolerance.isTouched()) {
        return 1;
    }
    return 0;
}

std::string pointStr(Handle(Geom_BSplineCurve) curve)
{
    auto firstPoint = curve->Value(curve->FirstParameter());
    auto lastPoint = curve->Value(curve->LastParameter());
    std::string strF = "First point: (" + std::to_string(firstPoint.X()) + ", "
        + std::to_string(firstPoint.Y())
        + ", " + std::to_string(firstPoint.Z()) + ") "; 
    std::string strL = "Last point: (" + std::to_string(lastPoint.X()) + ", "
        + std::to_string(lastPoint.Y()) + ", " + std::to_string(lastPoint.Z()) + ") "; 
    return strF + strL;
}

std::vector<Handle(Geom_BSplineCurve)> getCurves(const App::PropertyLinkSubList& edges)
{
    std::vector<Handle(Geom_BSplineCurve)> curves;

    auto objects = edges.getValues();
    auto subNames = edges.getSubValues();

    for (std::size_t i = 0; i < objects.size(); i++) {
        App::DocumentObject* obj = objects[i];
        std::string sub = subNames[i];
        if (obj && obj->isDerivedFrom<Part::Feature>()) {
            // get the sub-edge of the part's shape and copy it to nat make changes to original geometry
            const Part::TopoShape& shape = static_cast<Part::Feature*>(obj)->Shape.getShape().makeElementCopy();
            TopoDS_Shape edgeShape = shape.getSubShape(sub.c_str());
            if (!edgeShape.IsNull() && edgeShape.ShapeType() == TopAbs_EDGE) {   
                Standard_Real u1, u2;
                const TopoDS_Edge& edge = TopoDS::Edge(edgeShape);
                TopLoc_Location heloc;  // this will be output
                Handle(Geom_Curve) c_geom = BRep_Tool::Curve(edge, heloc, u1, u2);  // The geometric curve
                Handle(Geom_BSplineCurve) bspline = Handle(Geom_BSplineCurve)::DownCast(c_geom);  // Try to get BSpline curve

                gp_Trsf transf = heloc.Transformation();                
                if (!bspline.IsNull()) {
                    bspline->Segment(u1, u2);     // DownCast(c_geom) will not trim spline - do it manually
                }
                else {
                    // try to convert it into a B-spline
                    Handle(Geom_TrimmedCurve) trim = new Geom_TrimmedCurve(c_geom, u1, u2);
                    // Approximate the curve to non-rational polynomial BSpline
                    // to avoid C0 continuity in output surface
                    GeomConvert conv;
                    Convert_ParameterisationType paratype = Convert_Polynomial;
                    bspline = conv.CurveToBSplineCurve(trim, paratype);
                    if (bspline.IsNull()) {
                        // GeomConvert failed, try ShapeConstruct_Curve now
                        ShapeConstruct_Curve scc;
                        bspline = scc.ConvertToBSpline(c_geom, u1, u2, Precision::Confusion());
                        if (bspline.IsNull()) {
                            Standard_Failure::Raise(
                                "A curve was not a B-spline and could not be converted into one.");
                        }                  
                    }
                }
                bspline->Transform(transf);  // apply original transformation to control points
                curves.emplace_back(bspline);
            }
            else {
                Standard_Failure::Raise("Sub-shape is not an edge");
            }
        }
    }

    return curves;
}
    
App::DocumentObjectExecReturn* GordonSurface::execute()
{
    try {
        if ((ProfileEdges.getSize()) < 2) {
            return new App::DocumentObjectExecReturn("Provide at least 2 profiles.");
        }
        if ((GuideEdges.getSize()) < 2) {
            return new App::DocumentObjectExecReturn("Provide at least 2 guides.");
        }

        std::vector<Handle(Geom_BSplineCurve)> vcurves, ucurves;
        
        // Create a Gordon surface
        ucurves = getCurves(ProfileEdges);
        vcurves = getCurves(GuideEdges);

        // there is no reason to go under 1e-7 precision
        double tol = Tolerance.getValue() < Precision::Confusion() ? Precision::Confusion() : Tolerance.getValue();
        auto surface = occ_gordon::interpolate_curve_network(ucurves, vcurves, tol);

        if (surface.IsNull()) {
            return new App::DocumentObjectExecReturn("Failed to create a Gordon surface.");
        }

        // Create a face from the BSpline surface
        BRepBuilderAPI_MakeFace faceMaker(surface, Precision::Confusion());
        if (!faceMaker.IsDone()) {
            return new App::DocumentObjectExecReturn(
                "Failed to create a face from the BSpline surface.");
        }

        this->Shape.setValue(faceMaker.Face());
        return App::DocumentObject::StdReturn;
    }
    catch (const Standard_Failure& e) {
        return new App::DocumentObjectExecReturn(e.GetMessageString());
    }
}
