from pyPolyCSG import *

table = polyhedron()
table_top = polyhedron()
table_leg = polyhedron()

table_top.make_box( 1.0, 1.0, 1.0, True )
table_leg.make_sphere( 0.5, True )

table = table_top.scale( 4.0, 0.25, 3.0 )
table = table.union( table_leg.translate(-1.5,-0.4,-1.0 ) )
table = table.union( table_leg.translate( 1.5,-0.4,-1.0 ) )
table = table.union( table_leg.translate( 1.5,-0.4, 1.0 ) )
table = table.union( table_leg.translate(-1.5,-0.4, 1.0 ) )

torus = polyhedron();
torus.make_torus( 1.0, 0.25, True )
table = table.difference( torus.translate( 0.0, 0.25, 0.0 ) )

table.save_mesh("table.obj");