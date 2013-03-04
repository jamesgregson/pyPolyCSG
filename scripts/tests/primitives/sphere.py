# example of sphere primitive
# James Gregson (james.gregson@gmail.com), March 2013

# import module
import pyPolyCSG as csg

# create a sphere of radius 1.0, centered on origin with 10
# horizontal segments and 6 vertical
sphere = csg.sphere( 1.0, True, 10, 6 );

# save result as box.obj
sphere.save_mesh( "output/sphere.obj" )
