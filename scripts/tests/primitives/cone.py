# example of cone primitive
# James Gregson (james.gregson@gmail.com), March 2013

# import module
import pyPolyCSG as csg

# create a cone with base radius 1.0, height 2.0, centered on origin with 6 sides
cone = csg.cone( 1.0, 2.0, True, 6 );

# save result as box.obj
cone.save_mesh( "output/cone.obj" )
