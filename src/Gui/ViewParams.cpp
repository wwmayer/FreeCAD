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
    // Assert correct types at compile time
    static_assert(std::is_same<Bool    ::value_type, decltype(getUseNewSelection())>::value,  "Mismatching returns types");
    static_assert(std::is_same<Bool    ::value_type, decltype(getUseSelectionRoot())>::value, "Mismatching returns types");
    static_assert(std::is_same<Bool    ::value_type, decltype(getEnableSelection())>::value, "Mismatching returns types");
    static_assert(std::is_same<Int     ::value_type, decltype(getRenderCache())>::value, "Mismatching returns types");
    static_assert(std::is_same<Bool    ::value_type, decltype(getRandomColor())>::value, "Mismatching returns types");
    static_assert(std::is_same<Unsigned::value_type, decltype(getBoundingBoxColor())>::value, "Mismatching returns types");
    static_assert(std::is_same<Unsigned::value_type, decltype(getAnnotationTextColor())>::value, "Mismatching returns types");
    static_assert(std::is_same<Int     ::value_type, decltype(getMarkerSize())>::value, "Mismatching returns types");
    static_assert(std::is_same<Unsigned::value_type, decltype(getDefaultLinkColor())>::value, "Mismatching returns types");
    static_assert(std::is_same<Unsigned::value_type, decltype(getDefaultShapeLineColor())>::value, "Mismatching returns types");
    static_assert(std::is_same<Unsigned::value_type, decltype(getDefaultShapeVertexColor())>::value, "Mismatching returns types");
    static_assert(std::is_same<Unsigned::value_type, decltype(getDefaultShapeColor())>::value, "Mismatching returns types");
    static_assert(std::is_same<Int     ::value_type, decltype(getDefaultShapeLineWidth())>::value, "Mismatching returns types");
    static_assert(std::is_same<Int     ::value_type, decltype(getDefaultShapePointSize())>::value, "Mismatching returns types");
    static_assert(std::is_same<Bool    ::value_type, decltype(getCoinCycleCheck())>::value, "Mismatching returns types");
    static_assert(std::is_same<Bool    ::value_type, decltype(getEnablePropertyViewForInactiveDocument())>::value, "Mismatching returns types");
    static_assert(std::is_same<Bool    ::value_type, decltype(getShowSelectionBoundingBox())>::value, "Mismatching returns types");
    static_assert(std::is_same<Int     ::value_type, decltype(getDefaultFontSize())>::value, "Mismatching returns types");


    addParameter("UseNewSelection", Bool{true});
    addParameter("UseSelectionRoot", Bool{true});
    addParameter("EnableSelection", Bool{true});
    addParameter("RenderCache", Int{0l});
    addParameter("RandomColor", Bool{false});
    addParameter("BoundingBoxColor", Unsigned{0xfffffffful});
    addParameter("AnnotationTextColor", Unsigned{0xfffffffful});
    addParameter("MarkerSize", Int{9l});
    addParameter("DefaultLinkColor", Unsigned{0x66FFFFFFul});
    addParameter("DefaultShapeLineColor", Unsigned{0x191919FFul});
    addParameter("DefaultShapeVertexColor", Unsigned{0x191919FFul});
    addParameter("DefaultShapeColor", Unsigned{0xCCCCCCFFul});
    addParameter("DefaultShapeLineWidth", Int{2l});
    addParameter("DefaultShapePointSize", Int{2l});
    addParameter("CoinCycleCheck", Bool{true});
    addParameter("EnablePropertyViewForInactiveDocument", Bool{true});
    addParameter("ShowSelectionBoundingBox", Bool{false});
    addParameter("DefaultFontSize", Int{0l});
}

ViewParams::ViewParams()
{
    handle = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/View");
    handle->Attach(this);
    setup();
    initParameters();
}

ViewParams* ViewParams::instance()
{
    static ViewParams view;
    return &view;
}

bool ViewParams::getUseNewSelection() const
{
    return getValue<bool>("UseNewSelection");
}

void ViewParams::setUseNewSelection(bool v)
{
    setValue("UseNewSelection", v);
}

bool ViewParams::getUseSelectionRoot() const
{
    return getValue<bool>("UseSelectionRoot");
}

void ViewParams::setUseSelectionRoot(bool v)
{
    setValue("UseSelectionRoot", v);
}

bool ViewParams::getEnableSelection() const
{
    return getValue<bool>("EnableSelection");
}

void ViewParams::setEnableSelection(bool v)
{
    setValue("EnableSelection", v);
}

long ViewParams::getRenderCache() const
{
    return getValue<long>("RenderCache");
}

void ViewParams::setRenderCache(long v)
{
    setValue("RenderCache", v);
}

bool ViewParams::getRandomColor() const
{
    return getValue<bool>("RandomColor");
}

void ViewParams::setRandomColor(bool v)
{
    setValue("RandomColor", v);
}

unsigned long ViewParams::getBoundingBoxColor() const
{
    return getValue<unsigned long>("BoundingBoxColor");
}

void ViewParams::setBoundingBoxColor(unsigned long v)
{
    setValue("BoundingBoxColor", v);
}

unsigned long ViewParams::getAnnotationTextColor() const
{
    return getValue<unsigned long>("AnnotationTextColor");
}

void ViewParams::setAnnotationTextColor(unsigned long v)
{
    setValue("AnnotationTextColor", v);
}

long ViewParams::getMarkerSize() const
{
    return getValue<long>("MarkerSize");
}

void ViewParams::setMarkerSize(unsigned long v)
{
    setValue("MarkerSize", v);
}

unsigned long ViewParams::getDefaultLinkColor() const
{
    return getValue<unsigned long>("DefaultLinkColor");
}

void ViewParams::setDefaultLinkColor(unsigned long v)
{
    setValue("DefaultLinkColor", v);
}

unsigned long ViewParams::getDefaultShapeLineColor() const
{
    return getValue<unsigned long>("DefaultShapeLineColor");
}

void ViewParams::setDefaultShapeLineColor(unsigned long v)
{
    setValue("DefaultShapeLineColor", v);
}

unsigned long ViewParams::getDefaultShapeVertexColor() const
{
    return getValue<unsigned long>("DefaultShapeVertexColor");
}

void ViewParams::setDefaultShapeVertexColor(unsigned long v)
{
    setValue("DefaultShapeVertexColor", v);
}

unsigned long ViewParams::getDefaultShapeColor() const
{
    return getValue<unsigned long>("DefaultShapeColor");
}

void ViewParams::setDefaultShapeColor(unsigned long v)
{
    setValue("DefaultShapeColor", v);
}

long ViewParams::getDefaultShapeLineWidth() const
{
    return getValue<long>("DefaultShapeLineWidth");
}

void ViewParams::setDefaultShapeLineWidth(long v)
{
    setValue("DefaultShapeLineWidth", v);
}

long ViewParams::getDefaultShapePointSize() const
{
    return getValue<long>("DefaultShapePointSize");
}

void ViewParams::setDefaultShapePointSize(long v)
{
    setValue("DefaultShapePointSize", v);
}

bool ViewParams::getCoinCycleCheck() const
{
    return getValue<bool>("CoinCycleCheck");
}

void ViewParams::setCoinCycleCheck(bool v)
{
    setValue("CoinCycleCheck", v);
}

bool ViewParams::getEnablePropertyViewForInactiveDocument() const
{
    return getValue<bool>("EnablePropertyViewForInactiveDocument");
}

void ViewParams::setEnablePropertyViewForInactiveDocument(bool v)
{
    setValue("EnablePropertyViewForInactiveDocument", v);
}

bool ViewParams::getShowSelectionBoundingBox() const
{
    return getValue<bool>("ShowSelectionBoundingBox");
}

void ViewParams::setShowSelectionBoundingBox(bool v)
{
    setValue("ShowSelectionBoundingBox", v);
}

long ViewParams::getDefaultFontSize() const
{
    return getValue<long>("DefaultFontSize");
}

void ViewParams::setDefaultFontSize(long v)
{
    setValue("DefaultFontSize", v);
}

#if 0
ViewParams::ViewParams() {
    handle = App::GetApplication().GetParameterGroupByPath(
            "User parameter:BaseApp/Preferences/View");
    handle->Attach(this);
#undef FC_VIEW_PARAM
#define FC_VIEW_PARAM(_name,_ctype,_type,_def) \
    _name = handle->Get##_type(#_name,_def);

    FC_VIEW_PARAMS
}

ViewParams::~ViewParams() {
}

void ViewParams::OnChange(Base::Subject<const char*> &, const char* sReason) {
    if(!sReason)
        return;
#undef FC_VIEW_PARAM
#define FC_VIEW_PARAM(_name,_ctype,_type,_def) \
    if(strcmp(sReason,#_name)==0) {\
        _name = handle->Get##_type(#_name,_def);\
        return;\
    }
    FC_VIEW_PARAMS
}

ViewParams *ViewParams::instance() {
    static ViewParams *inst;
    if(!inst)
        inst = new ViewParams;
    return inst;
}
#endif
