### Test Heel Problem ###

### Import required modules ###
import Draft
import SurfaceTools

### Define mydoc as the active document ###
mydoc = FreeCAD.ActiveDocument

### Create curves ###
p1  = [FreeCAD.Vector(-8.0,0.0,50),FreeCAD.Vector(0.0,8.0,50),FreeCAD.Vector(8.0,0.0,50)]
pf1 = Draft.makeBSpline(p1,closed=False,face=False,support=None)

p2  = [FreeCAD.Vector(-9.0,0.0,20),FreeCAD.Vector(0.0,15.0,20),FreeCAD.Vector(9.0,0.0,20)]
pf2 = Draft.makeBSpline(p2,closed=False,face=False,support=None)

p3  = [FreeCAD.Vector(-13.0,0.0,0.0),FreeCAD.Vector(0.0,40.0,0.0),FreeCAD.Vector(13.0,0.0,0.0)]
pf3 = Draft.makeBSpline(p3,closed=False,face=False,support=None)

p4  = [p1[0],p2[0],p3[0]]
pf4 = Draft.makeBSpline(p4,closed=False,face=False,support=None)

p5  = [p1[-1],p2[-1],p3[-1]]
pf5 = Draft.makeBSpline(p5,closed=False,face=False,support=None)

p6  = [p1[1],p2[1],p3[1]]
pf6 = Draft.makeBSpline(p6,closed=False,face=False,support=None)

### Create a filling object ###
heel = mydoc.addObject('SurfaceTools::Filling', 'heel')

### Define the borders ###
heel.Border = [(pf1,'Edge1'),(pf3,'Edge1'),(pf4,'Edge1'),(pf5,'Edge1')]
heel.orderB = [0]*len(heel.Border)

### Define additional curves ###
heel.Curves = [(pf2,'Edge1'),(pf6,'Edge1')]
heel.orderC = [0]*len(heel.Curves)

### Set number of iterations ###
heel.NbIter = 2

### Recompute Document ###
mydoc.recompute()

### Fit all in View ###
Gui.SendMsgToActiveView("ViewFit")
