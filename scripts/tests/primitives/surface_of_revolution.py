# example of extrusion primitive
# James Gregson (james.gregson@gmail.com), March 2013

# import module
import pyPolyCSG as csg

# create the poly-line describing the extrusion contour
coords = []
coords.append( (0.0, 0.0) )
coords.append( (0.5, 0.5) )
coords.append( (2.0, 1.0) )
coords.append( (2.1, 0.0) )

# extrude the contour by one unit, revolved by 270 degrees
sor = csg.surface_of_revolution( coords, 270.0, 9 )

# save the output mesh
sor.save_mesh("output/surface_of_revolution_1.obj");

# create the same surface, but revolved by 360 degrees
sor = csg.surface_of_revolution( coords, 360, 12 )

# save the new output mesh
sor.save_mesh("output/surface_of_revolution_2.obj");

