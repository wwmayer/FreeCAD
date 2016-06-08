import SurfaceTools
import Draft
import Part

docname = "TestCut2"

App.newDocument(docname)
App.setActiveDocument(docname)

mydoc = App.getDocument(docname)

### Define Points and Curves ###

pl            = FreeCAD.Placement()
pl.Rotation.Q = (0.0,-0.0,-0.0,1.0)
pl.Base       = FreeCAD.Vector(-2,1,0.0)
rec           = Draft.makeRectangle(length=4,height=-2.5,placement=pl,face=True,support=None)

p1=[FreeCAD.Vector(1.39520406723,1.66792106628,0.0),FreeCAD.Vector(0.375479996204,1.38334679604,0.0),FreeCAD.Vector(0.256907105446,-0.32409837842,0.0)]
p2=[FreeCAD.Vector(0.256907105446,-0.32409837842,0.0),FreeCAD.Vector(-0.707483053207,-0.332003176212,0.0),FreeCAD.Vector(-1.56120562553,-1.83392262459,0.0)]

c1 = Draft.makeBSpline(p1,closed=False,face=False,support=None)
c2 = Draft.makeBSpline(p2,closed=False,face=False,support=None)

### Create Wire ###

wire  = mydoc.addObject("Part::Feature","Wire")
topow = Part.Wire([c1.Shape.Edges[0], c2.Shape.Edges[0]])
wire.Shape = topow

mydoc.recompute()

### Extrude from Wire ###

Ext1 = mydoc.addObject("Part::Extrusion","Extrude")
Ext1.Base = wire
Ext1.Dir = (0,0,1)
Ext1.Solid = (False)
Ext1.TaperAngle = (0)
c1.ViewObject.Visibility = False
Ext1.Label = 'Extrude'

mydoc.recompute()

### Add Test Feature ###

cut = mydoc.addObject('SurfaceTools::Cut', 'Cut')
cut.aShapeList = [(rec,"Face1"),(Ext1,"Shell1")]

mydoc.recompute()
Gui.SendMsgToActiveView("ViewFit")


