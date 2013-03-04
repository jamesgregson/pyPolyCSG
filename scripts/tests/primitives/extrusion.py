# example of extrusion primitive
# James Gregson (james.gregson@gmail.com), March 2013

# import module
import pyPolyCSG as csg

# create the poly-line describing the extrusion contour
coords = []
coords.append( (0.0, 0.0) )
coords.append( (1.0, 0.0) )
coords.append( (1.0, 1.0) )

# extrude the contour by one unit
extr = csg.extrusion( coords, 1.0 )

# save the output mesh
extr.save_mesh("output/extrusion.obj");

