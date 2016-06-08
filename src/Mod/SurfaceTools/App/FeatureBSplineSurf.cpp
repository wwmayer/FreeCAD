/***************************************************************************
 *   Copyright (c) 2014 Nathan Miller         <Nathan.A.Mill[at]gmail.com> *
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
#include <BRepBuilderAPI_MakeFace.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Precision.hxx>
#endif

#include "FeatureBSplineSurf.h"
#include <GeomFill.hxx>
#include <GeomFill_BSplineCurves.hxx>
#include <ShapeFix_Wire.hxx>
#include <ShapeExtend_WireData.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <Base/Tools.h>
#include <Base/Exception.h>
#include <TopExp_Explorer.hxx>

using namespace SurfaceTools;

const char* BSplineSurf::FillTypeEnums[]= {"Stretch","Coons","Curved",NULL};

PROPERTY_SOURCE(SurfaceTools::BSplineSurf, Part::Feature)

//Initial values

BSplineSurf::BSplineSurf()
{
    ADD_PROPERTY(BSplineList,(0));
    ADD_PROPERTY(FillType,(1));
    FillType.setEnums(FillTypeEnums);
}

short BSplineSurf::mustExecute() const
{
    if (BSplineList.isTouched() ||
        FillType.isTouched())
        return 1;
    return 0;
}

App::DocumentObjectExecReturn *BSplineSurf::execute(void)
{
    //Set Variables
    int ftype = FillType.getValue();

    //Begin Construction
    try {
        //Identify filling style

        GeomFill_FillingStyle fstyle;

        if (ftype==1) {
            fstyle = GeomFill_StretchStyle;
        }
        else if (ftype==2) {
            fstyle = GeomFill_CoonsStyle;
        }
        else if (ftype==3) {
            fstyle = GeomFill_CurvedStyle;
        }
        else {
            return new App::DocumentObjectExecReturn("Filling style must be 1 (Stretch), 2 (Coons), or 3 (Curved).");
        }

        //Create BSpline Surface

        GeomFill_BSplineCurves aSurfBuilder; //Create Surface Builder
        TopoDS_Wire aWire; //Create empty wire
        std::vector<App::PropertyLinkSubList::SubSet> aBSplineList = BSplineList.getSubListValues();

        //Get BSpline Curves from edges and initialize the builder

        Standard_Real u0 = 0.;
        Standard_Real u1 = 1.;

        Handle(ShapeFix_Wire) aShFW = new ShapeFix_Wire;
        Handle(ShapeExtend_WireData) aWD = new ShapeExtend_WireData;

        if(aBSplineList.size()>4){
            Standard_Failure::Raise("Only 2-4 continuous BSpline Curves are allowed");
        }
        if(aBSplineList.size()<2){
            Standard_Failure::Raise("Only 2-4 continuous BSpline Curves are allowed");
        }

        for(std::size_t i=0; i<aBSplineList.size(); i++){
            Part::TopoShape ts; //Curve TopoShape
            TopoDS_Shape sub;   //Curve TopoDS_Shape


            //Get Edge
            App::PropertyLinkSubList::SubSet set = aBSplineList[i];

            if(set.first->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId())) {
                ts = static_cast<Part::Feature*>(set.first)->Shape.getShape();

                //we want only the subshape which is linked
                for (auto jt: set.second) {
                    sub = ts.getSubShape(jt.c_str());

                    if (sub.ShapeType() == TopAbs_EDGE) {
                        aWD->Add(TopoDS::Edge(sub));
                    }
                    else {
                        Standard_Failure::Raise("Curves must be type TopoDS_Edge");
                    }
                }
            }
            else {
                Standard_Failure::Raise("Curve not from Part::Feature");
            }
        }

        //Reorder the curves and fix the wire if required

        aShFW->Load(aWD); //Load in the wire
        aShFW->FixReorder(); //Fix the order of the edges if required
        aShFW->ClosedWireMode() = Standard_True; //Enables closed wire mode
        aShFW->FixConnected(); //Fix connection between wires
        aShFW->FixSelfIntersection(); //Fix Self Intersection
        aShFW->Perform(); //Perform the fixes

        aWire = aShFW->Wire(); //Healed Wire

        if(aWire.IsNull()) {
            Standard_Failure::Raise("Wire unable to be constructed");
        }

        //Create BSpline Surface

        std::vector<Handle_Geom_BSplineCurve> bcrv;
        TopExp_Explorer anExp (aWire, TopAbs_EDGE);
        int it = 0;
        for (; anExp.More(); anExp.Next()) {
            const TopoDS_Edge hedge = TopoDS::Edge (anExp.Current());
            TopLoc_Location heloc = hedge.Location();
            Handle_Geom_Curve c_geom = BRep_Tool::Curve(hedge,heloc,u0,u1); //The geometric curve
            Handle_Geom_BSplineCurve b_geom = Handle_Geom_BSplineCurve::DownCast(c_geom); //Try to get BSpline curve

            if (!b_geom.IsNull()) {
                //Store Underlying Geometry
                bcrv.push_back(b_geom);
            }
            else {
                Standard_Failure::Raise("Curve not a BSpline Curve");
            }

            it++;
        }

        std::size_t ncrv = bcrv.size();

        if (ncrv==2) {
            aSurfBuilder.Init(bcrv[0],bcrv[1],fstyle);
        }
        else if (ncrv==3) {
            aSurfBuilder.Init(bcrv[0],bcrv[1],bcrv[2],fstyle);
        }
        else if (ncrv==4) {
            aSurfBuilder.Init(bcrv[0],bcrv[1],bcrv[2],bcrv[3],fstyle);
        }

        //Create the surface
        const Handle_Geom_BSplineSurface aSurface = aSurfBuilder.Surface();

        BRepBuilderAPI_MakeFace aFaceBuilder(aSurface,aWire,Standard_True); //Create Face Builder
//        Standard_Real u0 = 0.;
//        Standard_Real u1 = 2.;
//        Standard_Real v0 = 0.;
//        Standard_Real v1 = 2.;
//        aFaceBuilder.Init(aSurface,u0,u1,v0,v1,Precision::Confusion());

        TopoDS_Face aFace = aFaceBuilder.Face(); //Returned Face
        if (!aFaceBuilder.IsDone()) {
            return new App::DocumentObjectExecReturn("Face unable to be constructed");
        }

        if (aFace.IsNull()){
            return new App::DocumentObjectExecReturn("Resulting Face is null");
        }

        this->Shape.setValue(aFace);
        return App::DocumentObject::StdReturn;
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        return new App::DocumentObjectExecReturn(e->GetMessageString());
    }
}
