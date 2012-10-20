from pyPolyCSG import *

def inch_to_mm( inches ):
    return inches*25.4

def mm_to_inch( mm ):
    return mm/25.4

def hole_compensation( diameter ):
    return diameter+1.0

mounting_hole_radius = 0.5*hole_compensation( inch_to_mm( 5.0/16.0 ) )

def shaft_support_8mm():
    obj = box( inch_to_mm( 1.5 ), inch_to_mm( 1.5 ), inch_to_mm( 0.5 ), True )

    screw_hole = cylinder( mounting_hole_radius, inch_to_mm( 2.0 ), True, 20 ) 
    shaft_hole = cylinder( hole_compensation(8.0)/2.0, inch_to_mm( 1.0 ), True, 20 ).rotate( 90.0, 0.0, 0.0 )
    notch = box( inch_to_mm( 1.5 ), 2.0, inch_to_mm( 1.0 ), True )
    obj = obj - (screw_hole.translate( inch_to_mm( 0.5 ),0.0,0.0) + screw_hole.translate(-inch_to_mm( 0.5 ),0.0,0.0) + shaft_hole.translate( 0.0, inch_to_mm(0.25), 0.0 ) )
    obj = obj - notch.translate( inch_to_mm(0.75), inch_to_mm(0.25), 0.0 )
    return obj

def bearing_mount_lm8uu():
    obj = box( inch_to_mm( 1.5 ), inch_to_mm( 1.5 ), inch_to_mm( 1.0 ), True )
    screw_hole = cylinder( mounting_hole_radius, inch_to_mm( 2.0 ), True, 20 )
    bearing_hole = cylinder( hole_compensation(15.0)/2.0, inch_to_mm( 1.5 ), True, 20 ).rotate( 90.0, 0.0, 0.0 )
    notch = box( inch_to_mm( 1.5 ), 2.0, inch_to_mm( 1.0 ), True )
    obj = obj - (screw_hole.translate( inch_to_mm( 0.5 ),0.0,0.0) + screw_hole.translate(-inch_to_mm( 0.5 ),0.0,0.0) + bearing_hole.translate( 0.0, inch_to_mm(0.25), 0.0 ) )
    obj = obj - notch.translate( inch_to_mm(0.75), inch_to_mm(0.25), 0.0 )
    return obj

def bearing_mount_608():
    obj = box( inch_to_mm( 1.5 ), inch_to_mm( 1.5 ), inch_to_mm( 1.0 ), True )
    screw_hole = cylinder( mounting_hole_radius, inch_to_mm( 2.0 ), True, 20 )
    
    bearing_hole = cylinder( hole_compensation(22.0)/2.0, inch_to_mm( 0.25 ), True, 40 ).rotate( 90.0, 0.0, 0.0 )
    shaft_hole = cylinder( hole_compensation(14.0)/2.0, inch_to_mm( 1.0 ), True, 20 ).rotate( 90.0, 0.0, 0.0 )

    obj = obj - (screw_hole.translate( inch_to_mm( 0.5 ),0.0,0.0) + screw_hole.translate(-inch_to_mm( 0.5 ),0.0,0.0) + bearing_hole.translate( 0.0, inch_to_mm(0.25), inch_to_mm(3.0/8.0) ) + bearing_hole.translate( 0.0, inch_to_mm(0.25), inch_to_mm(-3.0/8.0) ) + shaft_hole.translate( 0.0, inch_to_mm(0.25), 0.0 ) )
    return obj

def spacer_quarter_inch():
    obj = box( inch_to_mm( 1.5 ), inch_to_mm( 1.0 ), inch_to_mm( 0.25 ), True )
    screw_hole = cylinder( mounting_hole_radius, inch_to_mm( 2.0 ), True, 20 ).rotate( 90.0, 0.0, 0.0 )
    obj = obj - (screw_hole.translate( inch_to_mm( 0.5 ),0.0,0.0) + screw_hole.translate(-inch_to_mm( 0.5 ),0.0,0.0) )
    return obj

def belt_clamp_base():
    obj = box( inch_to_mm( 1.5 ), inch_to_mm( 1.0 ), inch_to_mm( 0.75 ), True )
    screw_hole = cylinder( mounting_hole_radius, inch_to_mm( 2.0 ), True, 20 )
    obj = obj - (screw_hole.translate( inch_to_mm( 0.5 ),0.0,0.0) + screw_hole.translate(-inch_to_mm( 0.5 ),0.0,0.0) )
    return obj

def axis_end():
    obj = box( inch_to_mm( 5.5 ), inch_to_mm( 1.75 ), inch_to_mm( 0.75 ), True )
    
    screw_hole = cylinder( mounting_hole_radius, inch_to_mm( 3.0 ), True, 20 )
    
    shaft_hole = cylinder( 0.5*hole_compensation( inch_to_mm( 0.5 ) ), inch_to_mm(1.0), True, 20 ).rotate( 90.0, 0.0, 0.0 )

    center_hole = cylinder( 0.5*hole_compensation( inch_to_mm( 1.0 ) ), inch_to_mm(1.0), True, 20 ).rotate( 90.0, 0.0, 0.0 )
    mount_hole = cylinder( 0.5*hole_compensation( 4.0), inch_to_mm(1.0), True, 10 ).rotate( 90.0, 0.0, 0.0 )
    
    notch = box( inch_to_mm( 1.5 ), 2.0, inch_to_mm( 1.0 ), True )

    
    obj = obj - ( shaft_hole.translate( inch_to_mm( 2.0 ), 0.0, 0.0 ) + shaft_hole.translate( inch_to_mm( -2.0 ), 0.0, 0.0 ) )
    obj = obj - ( notch.translate( inch_to_mm( 2.75 ), 0.0, 0.0 ) + notch.translate( inch_to_mm( -2.75 ), 0.0, 0.0 ) )
    obj = obj - ( center_hole + mount_hole.translate( -15.5, -15.5, 0.0 ) + mount_hole.translate(  15.5, -15.5, 0.0 ) + mount_hole.translate( 15.5, 15.5, 0.0 ) + mount_hole.translate( -15.5, 15.5, 0.0 ) ) 
    
    obj = obj - ( screw_hole.translate( inch_to_mm(1.5), 0.0, 0.0 ) + screw_hole.translate( inch_to_mm(-1.5), 0.0, 0.0 ) )
    obj = obj - ( screw_hole.translate( inch_to_mm(2.5), 0.0, 0.0 ) + screw_hole.translate( inch_to_mm(-2.5), 0.0, 0.0 ) )
    
    return obj


def bearing_plate_608():
    obj = box( inch_to_mm(1.75), inch_to_mm(1.75), inch_to_mm(0.125), True )
    mount_hole = cylinder(0.5*hole_compensation( 4.0), inch_to_mm( 1.0 ), True ).rotate(90.0,0.0,0.0)
    thru_hole = cylinder(0.5*hole_compensation(14), inch_to_mm(1.0),True).rotate(90.0,0.0,0.0)
    boss = cylinder(0.5*inch_to_mm(1.25), inch_to_mm(0.125),True,50).rotate(90.0,0.0,0.0).translate(0.0,0.0,inch_to_mm(0.1245))
    bearing_hole = cylinder(0.5*hole_compensation(22.0),inch_to_mm(0.125),True).rotate(90.0,0.0,0.0).translate(0.0,0.0,inch_to_mm(0.125))
    obj = (obj + boss) - (bearing_hole+thru_hole);
    obj = obj - (mount_hole.translate(-15.5,-15.5,0.0) + mount_hole.translate(15.5,-15.5,0.0) + mount_hole.translate(15.5,15.5,0.0) + mount_hole.translate(-15.5,15.5,0.0) )
    return obj

def carriage():
    obj = box( inch_to_mm( 5 ), inch_to_mm( 5 ), inch_to_mm( 1.0 ), True )
    shaft_hole = cylinder( inch_to_mm( 0.75 )/2.0, inch_to_mm( 5.5 ), True )
    screw_hole = cylinder( inch_to_mm( 0.5 )/2.0, inch_to_mm( 5.5 ), True )
    
    leadnut_hole = cylinder( inch_to_mm(0.25)*0.5, inch_to_mm( 1.0 ), True );
    leadnut_access = box( inch_to_mm( 1.5 ), inch_to_mm( 3.0/8.0 ), inch_to_mm( 1.0 ), True )
    
    
    mhole = cylinder( mounting_hole_radius, inch_to_mm( 2.0 ), True ).rotate( 90.0, 0.0, 0.0 )
    
    obj = obj - ( shaft_hole.translate( inch_to_mm( 1.5 ), 0.0, 0.0 ) + shaft_hole.translate( inch_to_mm( -1.5 ), 0.0, 0.0 ) + screw_hole )
    obj = obj - ( leadnut_hole.translate( inch_to_mm( 0.5 ), inch_to_mm( -2.5 ), 0.0 ) + leadnut_hole.translate( inch_to_mm( -0.5 ), inch_to_mm( -2.5 ), 0.0 ) + leadnut_access.translate( 0.0, inch_to_mm( -2.0 ), inch_to_mm( 0.2 ) ) )
    
    for i in range( -2, 3 ):
        for j in range( -2, 3 ):
            if i != 0 and j != 0:
                obj = obj - ( mhole.translate( inch_to_mm( 1.0*i ), inch_to_mm( 1.0*j ), 0.0 ) )
    return obj

def bushing_mount():
    obj = box( inch_to_mm( 1.5 ), inch_to_mm( 1.5 ), inch_to_mm( 1.0 ), True )
    mounting_hole = cylinder( mounting_hole_radius, inch_to_mm( 2.0 ), True ).rotate( 90.0, 0.0, 0.0 )
    bushing_hole = cylinder( hole_compensation( inch_to_mm( 0.75 ) )/2.0, inch_to_mm( 2.0 ), True ).rotate( 0.0, 0.0, 0.0 )

    obj = obj - bushing_hole
    obj = obj - ( mounting_hole.translate( -inch_to_mm( 0.5 ), -inch_to_mm(0.5), 0.0 ) + mounting_hole.translate( inch_to_mm( 0.5 ), -inch_to_mm( 0.5 ), 0.0 ) + mounting_hole.translate( inch_to_mm( 0.5 ), inch_to_mm( 0.5 ), 0.0 ) + mounting_hole.translate( inch_to_mm( -0.5 ), inch_to_mm( 0.5 ), 0.0 ) )
    return obj

def leadnut_mount():
    obj = box( inch_to_mm( 1.5 ), inch_to_mm( 1.5 ), inch_to_mm( 1.0 ), True )
    mounting_hole = cylinder( mounting_hole_radius, inch_to_mm( 2.0 ), True ).rotate( 90.0, 0.0, 0.0 )
    hex_nut_hole = cylinder( hole_compensation( inch_to_mm( 0.565 ) )/2.0, inch_to_mm(2.0), True, 6 )

    obj = obj - hex_nut_hole
    obj = obj - ( mounting_hole.translate( -inch_to_mm( 0.5 ), -inch_to_mm(0.5), 0.0 ) + mounting_hole.translate( inch_to_mm( 0.5 ), -inch_to_mm( 0.5 ), 0.0 ) + mounting_hole.translate( inch_to_mm( 0.5 ), inch_to_mm( 0.5 ), 0.0 ) + mounting_hole.translate( inch_to_mm( -0.5 ), inch_to_mm( 0.5 ), 0.0 ) )
    return obj

def chain_end():
    plate = box( inch_to_mm( .75 ), inch_to_mm( 1.5 ), 3, True );
    hole1 = cylinder( mounting_hole_radius, inch_to_mm( 2.0 ), True ).rotate( 90, 0.0, 0.0 )
    hole2 = cylinder( mounting_hole_radius/2, inch_to_mm( 2.0 ), True ).rotate( 90, 0.0, 0.0 )
    return plate - (hole1.translate( 0.0, inch_to_mm(0.5), 0.0 ) + hole2.translate( 0.0, inch_to_mm(-0.5), 0.0 ) )

shaft_support_8mm().save_mesh( "shaft_support_8mm.obj" )
bearing_mount_lm8uu().save_mesh( "bearing_mount_lm8uu.obj" )
bearing_mount_608().save_mesh( "bearing_mount_608.obj" )
spacer_quarter_inch().save_mesh( "spacer_quarter_inch.obj" )
belt_clamp_base().save_mesh( "belt_clamp_base.obj" )
axis_end().save_mesh("axis_end.obj" )
carriage().save_mesh("carriage.obj" )
bushing_mount().save_mesh("bushing_mount.obj" )
leadnut_mount().save_mesh("leadnut_mount.obj" )
bearing_plate_608().save_mesh("bearing_plate_608.obj")
chain_end().save_mesh("chain_end.obj")