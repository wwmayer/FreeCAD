/****************************************************************************
 *   Copyright (c) 2018 Zheng Lei (realthunder) <realthunder.dev@gmail.com> *
 *                                                                          *
 *   This file is part of the FreeCAD CAx development system.               *
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Library General Public            *
 *   License as published by the Free Software Foundation; either           *
 *   version 2 of the License, or (at your option) any later version.       *
 *                                                                          *
 *   This library  is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU Library General Public License for more details.                   *
 *                                                                          *
 *   You should have received a copy of the GNU Library General Public      *
 *   License along with this library; see the file COPYING.LIB. If not,     *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,          *
 *   Suite 330, Boston, MA  02111-1307, USA                                 *
 *                                                                          *
 ****************************************************************************/

#include "PreCompiled.h"
#include <App/Application.h>
#include "ViewParams.h"

using namespace Gui;

void ViewParams::setup()
{
    // NOLINTBEGIN
    addParameter("UseNewSelection", Bool{true});
    addParameter("UseSelectionRoot", Bool{true});
    addParameter("EnableSelection", Bool{true});
    addParameter("EnablePreselection", Bool{true});
    addParameter("RenderCache", Int{0});
    addParameter("RandomColor", Bool{false});
    addParameter("BoundingBoxColor", Unsigned{4294967295UL});
    addParameter("AnnotationTextColor", Unsigned{4294967295UL});
    addParameter("MarkerSize", Int{9});
    addParameter("DefaultLinkColor", Unsigned{0x66FFFF00});
    addParameter("DefaultShapeLineColor", Unsigned{421075455UL});
    addParameter("DefaultShapeVertexColor", Unsigned{421075455UL});
    addParameter("DefaultShapeColor", Unsigned{0xCCCCCC00});
    addParameter("DefaultShapeTransparency", Int{0});
    addParameter("DefaultShapeLineWidth", Int{2});
    addParameter("DefaultShapePointSize", Int{2});
    addParameter("CoinCycleCheck", Bool{true});
    addParameter("EnablePropertyViewForInactiveDocument", Bool{true});
    addParameter("ShowSelectionBoundingBox", Bool{false});
    addParameter("PropertyViewTimer", Unsigned{100});
    addParameter("AxisXColor", Unsigned{0xCC333300});
    addParameter("AxisYColor", Unsigned{0x33CC3300});
    addParameter("AxisZColor", Unsigned{0x3333CC00});
    addParameter("DraggerScale", Double{0.03});
    // NOLINTEND
}

ViewParams::ViewParams()
{
    attachToParameter(App::GetApplication().GetParameterGroupByPath(
        "User parameter:BaseApp/Preferences/View"));
    setup();
    initParameters();
}

ViewParams* ViewParams::instance()
{
    static ViewParams view;
    return &view;
}

FC_PARAM_GETSET_IMP(ViewParams, UseNewSelection, bool)
FC_PARAM_GETSET_IMP(ViewParams, UseSelectionRoot, bool)
FC_PARAM_GETSET_IMP(ViewParams, EnableSelection, bool)
FC_PARAM_GETSET_IMP(ViewParams, EnablePreselection, bool)
FC_PARAM_GETSET_IMP(ViewParams, RenderCache, long)
FC_PARAM_GETSET_IMP(ViewParams, RandomColor, bool)
FC_PARAM_GETSET_IMP(ViewParams, BoundingBoxColor, unsigned long)
FC_PARAM_GETSET_IMP(ViewParams, AnnotationTextColor, unsigned long)
FC_PARAM_GETSET_IMP(ViewParams, MarkerSize, long)
FC_PARAM_GETSET_IMP(ViewParams, DefaultLinkColor, unsigned long)
FC_PARAM_GETSET_IMP(ViewParams, DefaultShapeLineColor, unsigned long)
FC_PARAM_GETSET_IMP(ViewParams, DefaultShapeVertexColor, unsigned long)
FC_PARAM_GETSET_IMP(ViewParams, DefaultShapeColor, unsigned long)
FC_PARAM_GETSET_IMP(ViewParams, DefaultShapeTransparency, long)
FC_PARAM_GETSET_IMP(ViewParams, DefaultShapeLineWidth, long)
FC_PARAM_GETSET_IMP(ViewParams, DefaultShapePointSize, long)
FC_PARAM_GETSET_IMP(ViewParams, CoinCycleCheck, bool)
FC_PARAM_GETSET_IMP(ViewParams, EnablePropertyViewForInactiveDocument, bool)
FC_PARAM_GETSET_IMP(ViewParams, ShowSelectionBoundingBox, bool)
FC_PARAM_GETSET_IMP(ViewParams, PropertyViewTimer, unsigned long)
FC_PARAM_GETSET_IMP(ViewParams, AxisXColor, unsigned long)
FC_PARAM_GETSET_IMP(ViewParams, AxisYColor, unsigned long)
FC_PARAM_GETSET_IMP(ViewParams, AxisZColor, unsigned long)
FC_PARAM_GETSET_IMP(ViewParams, DraggerScale, double)
