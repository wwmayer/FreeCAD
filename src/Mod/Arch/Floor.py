#***************************************************************************
#*                                                                         *
#*   Copyright (c) 2011                                                    *  
#*   Yorik van Havre <yorik@uncreated.net>                                 *  
#*                                                                         *
#*   This program is free software; you can redistribute it and/or modify  *
#*   it under the terms of the GNU General Public License (GPL)            *
#*   as published by the Free Software Foundation; either version 2 of     *
#*   the License, or (at your option) any later version.                   *
#*   for detail see the LICENCE text file.                                 *
#*                                                                         *
#*   This program is distributed in the hope that it will be useful,       *
#*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
#*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
#*   GNU Library General Public License for more details.                  *
#*                                                                         *
#*   You should have received a copy of the GNU Library General Public     *
#*   License along with this program; if not, write to the Free Software   *
#*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
#*   USA                                                                   *
#*                                                                         *
#***************************************************************************

import Cell,FreeCAD,FreeCADGui,Draft,Commands
from PyQt4 import QtCore

__title__="FreeCAD Arch Floor"
__author__ = "Yorik van Havre"
__url__ = "http://free-cad.sourceforge.net"

def makeFloor(objectslist=None,join=True,name="Floor"):
    '''makeFloor(objectslist,[joinmode]): creates a floor including the
    objects from the given list. If joinmode is False, components will
    not be joined.'''
    obj = FreeCAD.ActiveDocument.addObject("Part::FeaturePython",name)
    _Floor(obj)
    _ViewProviderFloor(obj.ViewObject)
    if objectslist:
        obj.Components = objectslist
    for comp in obj.Components:
        comp.ViewObject.hide()
    obj.JoinMode = join
    return obj

class _CommandFloor:
    "the Arch Cell command definition"
    def GetResources(self):
        return {'Pixmap'  : 'Arch_Floor',
                'MenuText': QtCore.QT_TRANSLATE_NOOP("Arch_Floor","Floor"),
                'Accel': "F, R",
                'ToolTip': QtCore.QT_TRANSLATE_NOOP("Arch_Floor","Creates a floor object including selected objects")}
        
    def Activated(self):
        sel = FreeCADGui.Selection.getSelection()
        transmode = True
        if not sel:
            transmode = False
        for obj in sel:
            if not (Draft.getType(obj) in ["Cell","Site","Building"]):
                transmode = False
        if transmode:
            FreeCAD.ActiveDocument.openTransaction("Type conversion")
            for obj in sel:
                nobj = makeFloor()
                Commands.copyProperties(obj,nobj)
                FreeCAD.ActiveDocument.removeObject(obj.Name)
            FreeCAD.ActiveDocument.commitTransaction()
        else:
            FreeCAD.ActiveDocument.openTransaction("Floor")
            makeFloor(sel)
            FreeCAD.ActiveDocument.commitTransaction()
        FreeCAD.ActiveDocument.recompute()
        
class _Floor(Cell._Cell):
    "The Cell object"
    def __init__(self,obj):
        Cell._Cell.__init__(self,obj)
        obj.addProperty("App::PropertyLength","Height","Base",
                        "The height of this floor")
        self.Type = "Floor"
        
class _ViewProviderFloor(Cell._ViewProviderCell):
    "A View Provider for the Cell object"
    def __init__(self,vobj):
        Cell._ViewProviderCell.__init__(self,vobj)

    def getIcon(self):
        return ":/icons/Arch_Floor_Tree.svg"

FreeCADGui.addCommand('Arch_Floor',_CommandFloor())
