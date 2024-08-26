#***************************************************************************
#*   Copyright (c) 2024 Werner Mayer <wmayer[at]users.sourceforge.net>     *
#*                                                                         *
#*   This file is part of FreeCAD.                                         *
#*                                                                         *
#*   FreeCAD is free software: you can redistribute it and/or modify it    *
#*   under the terms of the GNU Lesser General Public License as           *
#*   published by the Free Software Foundation, either version 2.1 of the  *
#*   License, or (at your option) any later version.                       *
#*                                                                         *
#*   FreeCAD is distributed in the hope that it will be useful, but        *
#*   WITHOUT ANY WARRANTY; without even the implied warranty of            *
#*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      *
#*   Lesser General Public License for more details.                       *
#*                                                                         *
#*   You should have received a copy of the GNU Lesser General Public      *
#*   License along with FreeCAD. If not, see                               *
#*   <https://www.gnu.org/licenses/>.                                      *
#*                                                                         *
#***************************************************************************

import unittest

import FreeCAD
from FreeCAD import Base
import Part
import Sketcher

class TestDragDrop(unittest.TestCase):
    def setUp(self):
        self.doc = FreeCAD.newDocument("PartDesignTest")

    def testDragMoveDrop(self):
        body = self.doc.addObject('PartDesign::Body','Body')
        box = self.doc.addObject('PartDesign::AdditiveBox','Box')
        body.addObject(box)
        self.doc.recompute()

        sketch = body.newObject('Sketcher::SketchObject','Sketch')
        sketch.AttachmentSupport = (box, ['Face6',])
        sketch.MapMode = 'FlatFace'
        self.doc.recompute()

        geoList = []
        geoList.append(Part.LineSegment(Base.Vector(3.170245, 7.453843, 0.000000), Base.Vector(3.170245, 4.598174, 0.000000)))
        geoList.append(Part.LineSegment(Base.Vector(3.170245, 4.598174, 0.000000), Base.Vector(5.687107, 4.598174, 0.000000)))
        geoList.append(Part.LineSegment(Base.Vector(5.687107, 4.598174, 0.000000), Base.Vector(5.687107, 7.453843, 0.000000)))
        geoList.append(Part.LineSegment(Base.Vector(5.687107, 7.453843, 0.000000), Base.Vector(3.170245, 7.453843, 0.000000)))
        sketch.addGeometry(geoList, False)
        del geoList

        constraintList = []
        constraintList.append(Sketcher.Constraint('Coincident', 0, 2, 1, 1))
        constraintList.append(Sketcher.Constraint('Coincident', 1, 2, 2, 1))
        constraintList.append(Sketcher.Constraint('Coincident', 2, 2, 3, 1))
        constraintList.append(Sketcher.Constraint('Coincident', 3, 2, 0, 1))
        constraintList.append(Sketcher.Constraint('Vertical', 0))
        constraintList.append(Sketcher.Constraint('Vertical', 2))
        constraintList.append(Sketcher.Constraint('Horizontal', 1))
        constraintList.append(Sketcher.Constraint('Horizontal', 3))
        sketch.addConstraint(constraintList)
        del constraintList

        self.doc.recompute()

        pad = body.newObject('PartDesign::Pad','Pad')
        pad.Profile = (sketch, ['',])
        pad.Length = 10
        pad.ReferenceAxis = (sketch, ['N_Axis'])
        sketch.Visibility = False
        self.doc.recompute()

        body.removeObject(pad)
        self.doc.recompute()
        pad.adjustRelativeLinks(body)
        self.doc.recompute()
        body.ViewObject.dropObject(pad, None, '', [])
        self.doc.recompute(None, False, True)
        # a base feature shouldn't be created
        self.assertEqual(self.doc.getObject("BaseFeature"), None)

    def tearDown(self):
        FreeCAD.closeDocument("PartDesignTest")
