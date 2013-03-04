import numpy
import pyPolyCSG as csg

coords = []
coords.append( (0.0, 0.0) )
coords.append( (1.0, 0.0) )
coords.append( (1.0, 1.0) )

A = csg.extrusion( coords, 1.0 )

print 'vertices:'
print A.get_vertices()

print 'faces:'
print A.get_triangles()
