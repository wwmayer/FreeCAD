import SurfaceTools
import Draft
import Part

docname = "TestCut1"

App.newDocument(docname)
App.setActiveDocument(docname)

mydoc = App.getDocument(docname)

### Define Points and Curves ###

p1 = [FreeCAD.Vector(-9.18136119843,0.156611263752,0.0),FreeCAD.Vector(-3.58249354362,4.54173898697,0.0),FreeCAD.Vector(4.9136929512,-4.07190418243,0.0),FreeCAD.Vector(12.705124855,0.078305631876,0.0)]
p2 = [FreeCAD.Vector(3.19096398354,7.86973762512,0.0),FreeCAD.Vector(-1.50738739967,5.63802146912,0.0),FreeCAD.Vector(-3.30842256546,-4.85496282578,0.0)]

c1 = Draft.makeBSpline(p1,closed=False,face=False,support=None)
c2 = Draft.makeBSpline(p2,closed=False,face=False,support=None)

### Extrude from Surface ###

Ext1 = mydoc.addObject("Part::Extrusion","Extrude")
Ext1.Base = c1
Ext1.Dir = (0,0,1)
Ext1.Solid = (False)
Ext1.TaperAngle = (0)
c1.ViewObject.Visibility = False
Ext1.Label = 'Extrude'

Ext2 = mydoc.addObject("Part::Extrusion","Extrude")
Ext2.Base = c2
Ext2.Dir = (0,0,1)
Ext2.Solid = (False)
Ext2.TaperAngle = (0)
c2.ViewObject.Visibility = False
Ext2.Label = 'Extrude'

mydoc.recompute()

### Add Test Feature ###

cut = mydoc.addObject('SurfaceTools::Cut', 'Cut')
cut.aShapeList = [(Ext1,"Face1"),(Ext2,"Face1")]

mydoc.recompute()
Gui.SendMsgToActiveView("ViewFit")
