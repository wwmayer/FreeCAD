import SurfaceTools

docname = "TestBSplineSurf"
ftp = 1 ### Change to test different fill types (1, 2, or 3 accepted)

App.newDocument(docname)
App.setActiveDocument(docname)

mydoc = App.getDocument(docname)

import Draft
import Part

### Set Borders ###

p1a = [FreeCAD.Vector(0.0,0.0,3.0),FreeCAD.Vector(3.0,1.0,1.5),FreeCAD.Vector(7.0,0.5,0.50),FreeCAD.Vector(10.0,0.0,0.0)]
p2a = [FreeCAD.Vector(0.0,0.0,3.0),FreeCAD.Vector(4.0,-1.3,-2),FreeCAD.Vector(6.0,0.75,-0.50),FreeCAD.Vector(10.0,0.0,0.0)]

p1b = [FreeCAD.Vector(0.0,0.0,3.0),FreeCAD.Vector(3.0,0.0,3.0),FreeCAD.Vector(7.0,0.0,-0.50),FreeCAD.Vector(10.0,0.0,0.0)]
p2b = [FreeCAD.Vector(10.0,0.0,0.0), FreeCAD.Vector(10.0,6.0,0.0), FreeCAD.Vector(3.3,9.0,0.0), FreeCAD.Vector(0.0,9.0,0.0)]
p3b = [FreeCAD.Vector(0.0,9.0,0.0),FreeCAD.Vector(0.0,6.0,-0.25),FreeCAD.Vector(0.0,4.0,3.0),FreeCAD.Vector(0.0,0.0,3.0)]

shft = FreeCAD.Vector(11,0,0)

for i in range(0,len(p1b)):
 p1b[i] = p1b[i]+shft

for i in range(0,len(p2b)):
 p2b[i] = p2b[i]+shft

for i in range(0,len(p3b)):
 p3b[i] = p3b[i]+shft

p1c = [FreeCAD.Vector(0.0,0.0,0.0),FreeCAD.Vector(3.0,0.0,3.0),FreeCAD.Vector(6.0,0.0,-2.50),FreeCAD.Vector(10.0,0.0,0.0)]
p2c = [FreeCAD.Vector(0.0,0.0,0.0),FreeCAD.Vector(0.0,2.7,4.1),FreeCAD.Vector(3.0,6.2,3.0),FreeCAD.Vector(0.0,7.0,0.0)]
p3c = [FreeCAD.Vector(0.0,7.0,0.0),FreeCAD.Vector(2.1,8.3,-1.0),FreeCAD.Vector(4.0,7.2,-3.0),FreeCAD.Vector(5.0,8.0,0.0)]
p4c = [FreeCAD.Vector(5.0,8.0,0.0),FreeCAD.Vector(6.2,5.3,1.0),FreeCAD.Vector(7.9,1.0,0.50),FreeCAD.Vector(10.0,0.0,0.0)]

for i in range(0,len(p1c)):
 p1c[i] = p1c[i]+shft*2

for i in range(0,len(p2c)):
 p2c[i] = p2c[i]+shft*2

for i in range(0,len(p3c)):
 p3c[i] = p3c[i]+shft*2

for i in range(0,len(p4c)):
 p4c[i] = p4c[i]+shft*2

bs1a = Draft.makeBSpline(p1a,closed=False,face=False,support=None)
bs2a = Draft.makeBSpline(p2a,closed=False,face=False,support=None)

bs1b = Draft.makeBSpline(p1b,closed=False,face=False,support=None)
bs2b = Draft.makeBSpline(p2b,closed=False,face=False,support=None)
bs3b = Draft.makeBSpline(p3b,closed=False,face=False,support=None)

bs1c = Draft.makeBSpline(p1c,closed=False,face=False,support=None)
bs2c = Draft.makeBSpline(p2c,closed=False,face=False,support=None)
bs3c = Draft.makeBSpline(p3c,closed=False,face=False,support=None)
bs4c = Draft.makeBSpline(p4c,closed=False,face=False,support=None)

mydoc.recompute()

### Add Test Feature Two sides ###

test1 = mydoc.addObject('SurfaceTools::BSplineSurf', 'two_curves')

### Add BSpline Curves to test ###

#mydoc.test.Border = [(bs1, 'Edge1'), (bw2, 'Edge1'), (bw3, 'Edge1'), (bs4, 'Edge1')]
test1.aBSplineList = [(bs1a, 'Edge1'), (bs2a, 'Edge1')]

### Set fill type ###
test1.filltype = ftp

#**********************************#

### Add Test Feature Three sides ###

test2 = mydoc.addObject('SurfaceTools::BSplineSurf', 'three_curves')

### Add BSpline Curves to test ###

#mydoc.test.Border = [(bs1, 'Edge1'), (bw2, 'Edge1'), (bw3, 'Edge1'), (bs4, 'Edge1')]
test2.aBSplineList = [(bs1b, 'Edge1'), (bs2b, 'Edge1'), (bs3b, 'Edge1')]

### Set fill type ###
test2.filltype = ftp

#**********************************#

### Add Test Feature Three sides ###

test3 = mydoc.addObject('SurfaceTools::BSplineSurf', 'four_curves')

### Add BSpline Curves to test ###

#mydoc.test.Border = [(bs1, 'Edge1'), (bw2, 'Edge1'), (bw3, 'Edge1'), (bs4, 'Edge1')]
test3.aBSplineList = [(bs1c, 'Edge1'), (bs2c, 'Edge1'), (bs3c, 'Edge1'), (bs4c, 'Edge1')]

### Set fill type ###
test3.filltype = ftp


mydoc.recompute()
Gui.SendMsgToActiveView("ViewFit")

