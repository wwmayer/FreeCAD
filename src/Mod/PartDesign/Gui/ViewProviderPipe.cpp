/***************************************************************************
 *   Copyright (c) 2015 Stefan Tröger <stefantroeger@gmx.net>              *
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
# include <QMessageBox>
# include <QMenu>
# include <TopExp.hxx>
# include <TopTools_IndexedMapOfShape.hxx>
#endif

#include "Utils.h"
#include "ViewProviderPipe.h"
//#include "TaskPipeParameters.h"
#include "TaskPipeParameters.h"
#include <Mod/PartDesign/App/Body.h>
#include <Mod/PartDesign/App/FeaturePipe.h>
#include <Mod/Sketcher/App/SketchObject.h>
#include <Gui/Control.h>
#include <Gui/Command.h>
#include <Gui/Application.h>
#include <Gui/BitmapFactory.h>


using namespace PartDesignGui;

PROPERTY_SOURCE(PartDesignGui::ViewProviderPipe,PartDesignGui::ViewProvider)

ViewProviderPipe::ViewProviderPipe()
{
}

ViewProviderPipe::~ViewProviderPipe()
{
}

std::vector<App::DocumentObject*> ViewProviderPipe::claimChildren(void)const
{
    std::vector<App::DocumentObject*> temp;

    PartDesign::Pipe* pcPipe = static_cast<PartDesign::Pipe*>(getObject());

    App::DocumentObject* sketch = pcPipe->getVerifiedSketch(true);
    if (sketch != nullptr)
        temp.push_back(sketch);

    for(App::DocumentObject* obj : pcPipe->Sections.getValues()) {
        if (obj != nullptr && obj->isDerivedFrom(Part::Part2DObject::getClassTypeId()))
            temp.push_back(obj);
    }

    App::DocumentObject* spine = pcPipe->Spine.getValue();
    if (spine != nullptr && spine->isDerivedFrom(Part::Part2DObject::getClassTypeId()))
        temp.push_back(spine);

    App::DocumentObject* auxspine = pcPipe->AuxillerySpine.getValue();
    if (auxspine != nullptr && auxspine->isDerivedFrom(Part::Part2DObject::getClassTypeId()))
        temp.push_back(auxspine);

    return temp;
}

void ViewProviderPipe::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    QAction* act;
    act = menu->addAction(QObject::tr("Edit pipe"), receiver, member);
    act->setData(QVariant((int)ViewProvider::Default));
    PartDesignGui::ViewProvider::setupContextMenu(menu, receiver, member);
}

bool ViewProviderPipe::doubleClicked(void)
{
    return PartDesignGui::setEdit(pcObject);
}

bool ViewProviderPipe::setEdit(int ModNum) {
    if (ModNum == ViewProvider::Default )
        setPreviewDisplayMode(true);

    return PartDesignGui::ViewProvider::setEdit(ModNum);
}

void ViewProviderPipe::unsetEdit(int ModNum) {
    setPreviewDisplayMode(false);
    PartDesignGui::ViewProvider::unsetEdit(ModNum);
}


TaskDlgFeatureParameters* ViewProviderPipe::getEditDialog() {
    return new TaskDlgPipeParameters(this, false);
}

bool ViewProviderPipe::onDelete(const std::vector<std::string> &s)
{/*
    PartDesign::Pipe* pcPipe = static_cast<PartDesign::Pipe*>(getObject());

    // get the Sketch
    Sketcher::SketchObject *pcSketch = 0;
    if (pcPipe->Sketch.getValue())
        pcSketch = static_cast<Sketcher::SketchObject*>(pcPipe->Sketch.getValue());

    // if abort command deleted the object the sketch is visible again
    if (pcSketch && Gui::Application::Instance->getViewProvider(pcSketch))
        Gui::Application::Instance->getViewProvider(pcSketch)->show();
*/
    return ViewProvider::onDelete(s);
}



void ViewProviderPipe::highlightReferences(ViewProviderPipe::Reference mode, bool on)
{
    PartDesign::Pipe* pcPipe = static_cast<PartDesign::Pipe*>(getObject());

    switch (mode) {
    case Spine:
        highlightReferences(dynamic_cast<Part::Feature*>(pcPipe->Spine.getValue()),
                            pcPipe->Spine.getSubValuesStartsWith("Edge"), on);
        break;
    case AuxiliarySpine:
        highlightReferences(dynamic_cast<Part::Feature*>(pcPipe->AuxillerySpine.getValue()),
                            pcPipe->AuxillerySpine.getSubValuesStartsWith("Edge"), on);
        break;
    case Profile:
        highlightReferences(dynamic_cast<Part::Feature*>(pcPipe->Profile.getValue()),
                            pcPipe->Profile.getSubValuesStartsWith("Edge"), on);
        break;
    case Section:
        {
            std::vector<App::DocumentObject*> sections = pcPipe->Sections.getValues();
            for (auto it : sections) {
                highlightReferences(dynamic_cast<Part::Feature*>(it),
                                    std::vector<std::string>(), on);
            }
        }
        break;
    default:
        break;
    }
}

void ViewProviderPipe::highlightReferences(Part::Feature* base, const std::vector<std::string>& edges, bool on) {

    PartGui::ViewProviderPart* svp = dynamic_cast<PartGui::ViewProviderPart*>(
                Gui::Application::Instance->getViewProvider(base));
    if (svp == nullptr)
        return;

    std::vector<App::Color>& edgeColors = originalLineColors[base->getID()];

    if (on) {
         if (edgeColors.empty()) {
            TopTools_IndexedMapOfShape eMap;
            TopExp::MapShapes(base->Shape.getValue(), TopAbs_EDGE, eMap);
            edgeColors = svp->LineColorArray.getValues();
            std::vector<App::Color> colors = edgeColors;
            colors.resize(eMap.Extent(), svp->LineColor.getValue());

            if (!edges.empty()) {
                for (std::string e : edges) {
                    int idx = std::stoi(e.substr(4)) - 1;
                    assert ( idx >= 0 );
                    if ( idx < (ssize_t) colors.size() )
                        colors[idx] = App::Color(1.0f,0.0f,1.0f); // magenta
                }
            }
            else {
                std::fill(colors.begin(), colors.end(), App::Color(0.6f,0.0f,1.0f)); // purple
            }
            svp->LineColorArray.setValues(colors);
        }
    } else {
        if (!edgeColors.empty()) {
            svp->LineColorArray.setValues(edgeColors);
            edgeColors.clear();
        }
    }
}

QIcon ViewProviderPipe::getIcon(void) const {
    QString str = QString::fromLatin1("PartDesign_");
    auto* prim = static_cast<PartDesign::Pipe*>(getObject());
    if(prim->getAddSubType() == PartDesign::FeatureAddSub::Additive)
        str += QString::fromLatin1("Additive");
    else
        str += QString::fromLatin1("Subtractive");

    str += QString::fromLatin1("Pipe.svg");
    return PartDesignGui::ViewProvider::mergeGreyableOverlayIcons(Gui::BitmapFactory().pixmap(str.toStdString().c_str()));
}

