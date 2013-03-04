# example of box primitive
# James Gregson (james.gregson@gmail.com), March 2013

# import module
import pyPolyCSG as csg

# create a box, arguments are dimensions in x, y and z,
# True centers on origin
box = csg.box( 1.0, 2.0, 3.0, True );

# save result as box.obj
box.save_mesh( "output/box.obj" )
