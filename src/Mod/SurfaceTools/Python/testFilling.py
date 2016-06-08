import SurfaceTools

docname = "TestFilling"

App.newDocument(docname)
App.setActiveDocument(docname)

mydoc = App.getDocument(docname)

### Add Test Feature ###

test = mydoc.addObject('SurfaceTools::Filling', 'test')

Gui.activateWorkbench("DraftWorkbench")
import Draft
import Part

### Set Borders ###

p1 = [FreeCAD.Vector(-4.87768125534,0.0214878562838,0.0),FreeCAD.Vector(-1.95536983013,1.67603135109,0.0),FreeCAD.Vector(0.193388029933,-1.09586656094,0.0),FreeCAD.Vector(6.57519960403,0.558676898479,0.0)]
p2 = [FreeCAD.Vector(-4.87768125534,0.0214878562838,0.0),FreeCAD.Vector(-5.03769858432,2.76758429426,-2.33862645643),FreeCAD.Vector(-2.44906384298,1.8259977924,-3.86730853514),FreeCAD.Vector(-1.0573142973,2.17442159038,-9.54524130253)]
p3 = [FreeCAD.Vector(-1.0573142973,2.17442159038,-9.54524130253),FreeCAD.Vector(0.775084755748,5.78349983017,-5.16924116351),FreeCAD.Vector(3.02625924459,2.9648286752,-2.4917114848),FreeCAD.Vector(7.71687369128,2.76289241967,-2.02728113839),FreeCAD.Vector(6.57519960403,0.558676898479,0.0)]

bs1 = Draft.makeBSpline(p1,closed=False,face=False,support=None)
bs2 = Draft.makeBSpline(p2,closed=False,face=False,support=None)
bs3 = Draft.makeBSpline(p3,closed=False,face=False,support=None)

### Extra Curves ###

pc1=[FreeCAD.Vector(0.775084755748,5.78349983017,-5.16924116351),FreeCAD.Vector(-0.704247651183,2.83133722738,-3.23983306109),FreeCAD.Vector(-5.03769858432,2.76758429426,-2.33862645643)]

bsc1 = Draft.makeBSpline(pc1,closed=False,face=False,support=None)

### Points ###

pt1 = Draft.makePoint(2.00706976847,3.05149504267,-0.595598276906)

### Add border constraints ###

mydoc.test.Border = [(bs1, 'Edge1'), (bs2, 'Edge1'), (bs3, 'Edge1')]

### Set order of border constraints ###

mydoc.test.orderB = [0,0,0]

### Add additional curve constraints ###

mydoc.test.Curves = [(bsc1, 'Edge1')]

### Set order of curve constraints ###

mydoc.test.orderC = [0]

### Add point constraints ###
mydoc.test.Points = [(pt1, 'Vertex1')]

### Set number of iterations ###
mydoc.test.NbIter = 10

mydoc.recompute()
Gui.SendMsgToActiveView("ViewFit")

