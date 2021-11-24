import os

VISIT_ROOT = os.getenv('VISIT_ROOT')

# Turn off some annotations
annot=AnnotationAttributes()
annot.userInfoFlag=0
SetAnnotationAttributes(annot)

# Open a database and create a plot
OpenDatabase("%s/data/curv2d.silo" % VISIT_ROOT)
AddPlot("Pseudocolor", "d")
DrawPlots()

# Save the image
swa = SaveWindowAttributes()
swa.family = 0
swa.fileName = "visit0000"
swa.resConstraint = swa.NoConstraint
swa.width = 600
swa.height = 400
SetSaveWindowAttributes(swa)
SaveWindow()

quit()
