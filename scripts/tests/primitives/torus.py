# example of torus primitive
# James Gregson (james.gregson@gmail.com), March 2013

# import module
import pyPolyCSG as csg

# create a box
torus = csg.torus( 2.0, 0.5, True, 10, 6 );

# save result as box.obj
torus.save_mesh( "output/torus.obj" )
