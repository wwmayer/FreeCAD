/***************************************************************************
 *   Copyright (c) 2017 Stefan Tröger <stefantroeger@gmx.net>              *
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
#include <QApplication>
#endif

#include <App/GroupExtension.h>
#include <App/Document.h>

#include "Command.h"
#include "ActiveObjectList.h"
#include "Application.h"
#include "Document.h"
#include "ViewProviderDocumentObject.h"
#include "Selection.h"

using namespace Gui;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// Std_Part
//===========================================================================
DEF_STD_CMD_A(StdCmdPart)

StdCmdPart::StdCmdPart()
    : Command("Std_Part")
{
    sGroup = "Structure";
    sMenuText = QT_TR_NOOP("Create part");
    sToolTipText = QT_TR_NOOP(
        "A Part is a general purpose container to keep together a group of objects so that they "
        "act as a unit in the 3D view. It is meant to arrange objects that have a Part "
        "TopoShape, like Part Primitives, PartDesign Bodies, and other Parts.");
    sWhatsThis = "Std_Part";
    sStatusTip = sToolTipText;
    sPixmap = "Geofeaturegroup";
}

void StdCmdPart::activated(int iMsg)
{
    Q_UNUSED(iMsg);

    openCommand(QT_TRANSLATE_NOOP("Command", "Add a part"));
    std::string FeatName = getUniqueObjectName("Part");

    std::string PartName;
    PartName = getUniqueObjectName("Part");
    doCommand(Doc,
              "App.activeDocument().Tip = App.activeDocument().addObject('App::Part','%s')",
              PartName.c_str());
    // TODO We really must set label ourselves? (2015-08-17, Fat-Zer)
    doCommand(Doc,
              "App.activeDocument().%s.Label = '%s'",
              PartName.c_str(),
              QObject::tr(PartName.c_str()).toUtf8().data());
    doCommand(Gui::Command::Gui,
              "Gui.activateView('Gui::View3DInventor', True)\n"
              "Gui.activeView().setActiveObject('%s', App.activeDocument().%s)",
              PARTKEY,
              PartName.c_str());

    updateActive();
}

bool StdCmdPart::isActive()
{
    return hasActiveDocument();
}

//===========================================================================
// Std_Group
//===========================================================================
DEF_STD_CMD_A(StdCmdGroup)

StdCmdGroup::StdCmdGroup()
    : Command("Std_Group")
{
    sGroup = "Structure";
    sMenuText = QT_TR_NOOP("Create group");
    sToolTipText =
        QT_TR_NOOP("A Group is a general purpose container to group objects in the "
                   "Tree view, regardless of their data type. It is a simple folder to organize "
                   "the objects in a model.");
    sWhatsThis = "Std_Group";
    sStatusTip = sToolTipText;
    sPixmap = "folder";
}

void StdCmdGroup::activated(int iMsg)
{
    Q_UNUSED(iMsg);

    openCommand(QT_TRANSLATE_NOOP("Command", "Add a group"));

    std::string GroupName;
    GroupName = getUniqueObjectName("Group");
    QString label = QApplication::translate("Std_Group", "Group");
    doCommand(Doc,
              "App.activeDocument().Tip = "
              "App.activeDocument().addObject('App::DocumentObjectGroup','%s')",
              GroupName.c_str());
    doCommand(Doc,
              "App.activeDocument().%s.Label = '%s'",
              GroupName.c_str(),
              label.toUtf8().data());
    commitCommand();

    Gui::Document* gui = Application::Instance->activeDocument();
    App::Document* app = gui->getDocument();
    ViewProvider* vp = gui->getViewProvider(app->getActiveObject());
    if (vp && vp->isDerivedFrom<ViewProviderDocumentObject>()) {
        gui->signalScrollToObject(*static_cast<ViewProviderDocumentObject*>(vp));
    }
}

bool StdCmdGroup::isActive()
{
    return hasActiveDocument();
}

//===========================================================================
// Std_VarSet
//===========================================================================
DEF_STD_CMD_A(StdCmdVarSet)

StdCmdVarSet::StdCmdVarSet()
    : Command("Std_VarSet")
{
    sGroup = "Structure";
    sMenuText = QT_TR_NOOP("Create a variable set");
    sToolTipText =
        QT_TR_NOOP("A Variable Set is an object that maintains a set of properties to be used as "
                   "variables.");
    sWhatsThis = "Std_VarSet";
    sStatusTip = sToolTipText;
    sPixmap = "VarSet";
}

void StdCmdVarSet::activated(int iMsg)
{
    Q_UNUSED(iMsg);

    openCommand(QT_TRANSLATE_NOOP("Command", "Add a variable set"));

    std::string VarSetName;
    VarSetName = getUniqueObjectName("VarSet");
    doCommand(Doc, "App.activeDocument().addObject('App::VarSet','%s')", VarSetName.c_str());

    // add the varset to a group if it is selected
    auto sels = Selection().getSelectionEx(nullptr,
                                           App::DocumentObject::getClassTypeId(),
                                           ResolveMode::OldStyleElement,
                                           true);
    if (sels.size() == 1) {
        App::DocumentObject* obj = sels[0].getObject();
        auto group = obj->getExtension<App::GroupExtension>();
        if (group) {
            Gui::Document* docGui = Application::Instance->activeDocument();
            App::Document* doc = docGui->getDocument();
            group->addObject(doc->getObject(VarSetName.c_str()));
        }
    }
    commitCommand();

    doCommand(Doc,
              "App.ActiveDocument.getObject('%s').ViewObject.doubleClicked()",
              VarSetName.c_str());
}

bool StdCmdVarSet::isActive()
{
    return hasActiveDocument();
}

namespace Gui
{

void CreateStructureCommands()
{
    CommandManager& rcCmdMgr = Application::Instance->commandManager();

    rcCmdMgr.addCommand(new StdCmdPart());
    rcCmdMgr.addCommand(new StdCmdGroup());
    rcCmdMgr.addCommand(new StdCmdVarSet());
}

}  // namespace Gui
