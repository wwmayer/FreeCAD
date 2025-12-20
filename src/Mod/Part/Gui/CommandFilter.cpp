/***************************************************************************
 *   Copyright (c) 2002 Jürgen Riegel <juergen.riegel@web.de>              *
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
#include <QAction>
#include <QApplication>
#endif

#include <Gui/Action.h>
#include <Gui/BitmapFactory.h>
#include <Gui/Command.h>
#include <Gui/MainWindow.h>
#include <Gui/View3DInventor.h>


//===========================================================================
// Part_VertexSelection
//===========================================================================
DEF_3DV_CMD_C(PartCmdVertexSelection)

PartCmdVertexSelection::PartCmdVertexSelection()
  : Command("Part_VertexSelection")
{
    sGroup        = "Standard-View";
    sMenuText     = QT_TR_NOOP("Vertex selection");
    sToolTipText  = QT_TR_NOOP("Only allow the selection of vertices");
    sWhatsThis    = "Part_VertexSelection";
    sStatusTip    = QT_TR_NOOP("Only allow the selection of vertices");
    sPixmap       = "vertex-selection";
    sAccel        = "X, S";
    eType         = Alter3DView;
}

Gui::Action* PartCmdVertexSelection::createAction()
{
    Gui::Action* act = Command::createAction();
    act->setCheckable(true);
    return act;
}

void PartCmdVertexSelection::activated(int iMsg)
{
    std::array cmds {"Part_EdgeSelection",
                     "Part_FaceSelection"};
    Gui::CommandManager& cmdMgr = Gui::Application::Instance->commandManager();
    for (auto name : cmds) {
        if (Gui::Command* cmd = cmdMgr.getCommandByName(name)) {
            if (Gui::Action* act = cmd->getAction()) {
                act->setBlockedChecked(false);
            }
        }
    }

    if (iMsg > 0) {
        doCommand(Command::Gui,"Gui.Selection.addSelectionGate('SELECT Part::Feature SUBELEMENT Vertex')");
    }
    else {
        doCommand(Command::Gui,"Gui.Selection.removeSelectionGate()");
    }
}


//===========================================================================
// Part_EdgeSelection
//===========================================================================
DEF_3DV_CMD_C(PartCmdEdgeSelection)

PartCmdEdgeSelection::PartCmdEdgeSelection()
  : Command("Part_EdgeSelection")
{
    sGroup        = "Standard-View";
    sMenuText     = QT_TR_NOOP("Edge selection");
    sToolTipText  = QT_TR_NOOP("Only allow the selection of edges");
    sWhatsThis    = "Part_EdgeSelection";
    sStatusTip    = QT_TR_NOOP("Only allow the selection of edges");
    sPixmap       = "edge-selection";
    sAccel        = "E, S";
    eType         = Alter3DView;
}

Gui::Action* PartCmdEdgeSelection::createAction()
{
    Gui::Action* act = Command::createAction();
    act->setCheckable(true);
    return act;
}

void PartCmdEdgeSelection::activated(int iMsg)
{
    std::array cmds {"Part_VertexSelection",
                     "Part_FaceSelection"};
    Gui::CommandManager& cmdMgr = Gui::Application::Instance->commandManager();
    for (auto name : cmds) {
        if (Gui::Command* cmd = cmdMgr.getCommandByName(name)) {
            if (Gui::Action* act = cmd->getAction()) {
                act->setBlockedChecked(false);
            }
        }
    }

    if (iMsg > 0) {
        doCommand(Command::Gui,"Gui.Selection.addSelectionGate('SELECT Part::Feature SUBELEMENT Edge')");
    }
    else {
        doCommand(Command::Gui,"Gui.Selection.removeSelectionGate()");
    }
}


//===========================================================================
// Part_FaceSelection
//===========================================================================
DEF_3DV_CMD_C(PartCmdFaceSelection)

PartCmdFaceSelection::PartCmdFaceSelection()
  : Command("Part_FaceSelection")
{
    sGroup        = "Standard-View";
    sMenuText     = QT_TR_NOOP("Face selection");
    sToolTipText  = QT_TR_NOOP("Only allow the selection of faces");
    sWhatsThis    = "Part_FaceSelection";
    sStatusTip    = QT_TR_NOOP("Only allow the selection of faces");
    sPixmap       = "face-selection";
    sAccel        = "F, S";
    eType         = Alter3DView;
}

Gui::Action* PartCmdFaceSelection::createAction()
{
    Gui::Action* act = Command::createAction();
    act->setCheckable(true);
    return act;
}

void PartCmdFaceSelection::activated(int iMsg)
{
    std::array cmds {"Part_VertexSelection",
                     "Part_EdgeSelection"};
    Gui::CommandManager& cmdMgr = Gui::Application::Instance->commandManager();
    for (auto name : cmds) {
        if (Gui::Command* cmd = cmdMgr.getCommandByName(name)) {
            if (Gui::Action* act = cmd->getAction()) {
                act->setBlockedChecked(false);
            }
        }
    }

    if (iMsg > 0) {
        doCommand(Command::Gui,"Gui.Selection.addSelectionGate('SELECT Part::Feature SUBELEMENT Face')");
    }
    else {
        doCommand(Command::Gui,"Gui.Selection.removeSelectionGate()");
    }
}


//===========================================================================
// Part_RemoveSelectionGate
//===========================================================================
DEF_3DV_CMD(PartCmdRemoveSelectionGate)

PartCmdRemoveSelectionGate::PartCmdRemoveSelectionGate()
  : Command("Part_RemoveSelectionGate")
{
    sGroup        = "Standard-View";
    sMenuText     = QT_TR_NOOP("All selection filters cleared");
    sToolTipText  = QT_TR_NOOP("All selection filters cleared");
    sWhatsThis    = "Part_RemoveSelectionGate";
    sStatusTip    = QT_TR_NOOP("All selection filters cleared");
    sPixmap       = "clear-selection";
    sAccel        = "C, S";
    eType         = Alter3DView;
}

void PartCmdRemoveSelectionGate::activated(int iMsg)
{
    std::array cmds {"Part_VertexSelection",
                     "Part_EdgeSelection",
                     "Part_FaceSelection"};
    Gui::CommandManager& cmdMgr = Gui::Application::Instance->commandManager();
    for (auto name : cmds) {
        if (Gui::Command* cmd = cmdMgr.getCommandByName(name)) {
            if (Gui::Action* act = cmd->getAction()) {
                act->setBlockedChecked(false);
            }
        }
    }

    Q_UNUSED(iMsg);
    doCommand(Command::Gui,"Gui.Selection.removeSelectionGate()");
}

void CreatePartSelectCommands()
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();
    // NOLINTBEGIN
    rcCmdMgr.addCommand(new PartCmdVertexSelection());
    rcCmdMgr.addCommand(new PartCmdEdgeSelection());
    rcCmdMgr.addCommand(new PartCmdFaceSelection());
    rcCmdMgr.addCommand(new PartCmdRemoveSelectionGate());
    // NOLINTEND
}
