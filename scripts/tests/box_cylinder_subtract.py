# iterative subtraction of cylinder from box, from Dave Curtis
# produces nearly coincident geometry, causes problems when
# probe variable is adjusted, likely due to concident/degenerate
# geometry.  
import pyPolyCSG as csg

probe = 0.05;
#probe = 0.9 # works
#probe = 1.0 # works
#probe = 1.1 # fails
#probe = 2.0 # fails

block = csg.box(6,2,4,True)
cyl = csg.cylinder(1,4.5,True)
cyl = cyl.translate(0,probe,0)

offset = 4
i = 0 # keep track of which iteration failed
while offset > -0.4:
    print i
    i += 1
    tool = cyl.translate(offset,0,0)
    block -= tool
    offset -= 0.5
    block.save_mesh( 'tmp_%04d.obj' % i )

print offset
block.save_mesh('tmp.obj')