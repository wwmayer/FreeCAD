# ***************************************************************************
# *   Copyright (c) 2025 Werner Mayer <wmayer[at]users.sourceforge.net>     *
# *                                                                         *
# *   This file is part of FreeCAD.                                         *
# *                                                                         *
# *   FreeCAD is free software: you can redistribute it and/or modify it    *
# *   under the terms of the GNU Lesser General Public License as           *
# *   published by the Free Software Foundation, either version 2.1 of the  *
# *   License, or (at your option) any later version.                       *
# *                                                                         *
# *   FreeCAD is distributed in the hope that it will be useful, but        *
# *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      *
# *   Lesser General Public License for more details.                       *
# *                                                                         *
# *   You should have received a copy of the GNU Lesser General Public      *
# *   License along with FreeCAD. If not, see                               *
# *   <https://www.gnu.org/licenses/>.                                      *
# ***************************************************************************/


import FreeCAD
from FreeCAD import Base
import FreeCADGui
import Part
import Sketcher
import unittest


class TestSketchMoveConstraint(unittest.TestCase):
    def setUp(self):
        self.Doc = FreeCAD.newDocument("SketchGuiTest")

    def testIssue_19566(self):
        body = self.Doc.addObject("PartDesign::Body", "Body")
        sketch = body.newObject("Sketcher::SketchObject", "Sketch")
        sketch.AttachmentSupport = (self.Doc.getObject("XY_Plane"), [""])
        sketch.MapMode = "FlatFace"
        self.Doc.recompute()

        geoList = []
        geoList.append(
            Part.LineSegment(
                Base.Vector(-71.011909, -13.809516, 0.000000),
                Base.Vector(-29.583338, 14.880962, 0.000000),
            )
        )
        sketch.addGeometry(geoList, False)
        del geoList

        constraintList = []
        sketch.addConstraint(Sketcher.Constraint("DistanceX", 0, 1, 0, 2, 41.428571))
        self.Doc.recompute()
        FreeCADGui.getDocument(self.Doc).setEdit(sketch)
        sketch.toggleDriving(0)
        sketch.setVirtualSpace([0], True)
        sketch.moveGeometries([(0, 0)], Base.Vector(23.2963, 1.11149, 0), True)

    def tearDown(self):
        # closing doc
        FreeCAD.closeDocument("SketchGuiTest")
