cmake_minimum_required( VERSION 2.6 )

# create a basic project
project( boolean )

set( CARVE_INCLUDE_DIRS third_party/carve-1.4.0/include third_party/carve-1.4.0-build/include/carve )
set( CARVE_LIBRARY_DIRS third_party/carve-1.4.0-build/lib )
set( CARVE_LINK_LIBRARIES carve )
LINK_DIRECTORIES( ${CARVE_LIBRARY_DIRS} )


include_directories( /usr/local/include )

find_package(boost)

set( BOOLEAN_SOURCES 
	source/mesh_functions.cpp 
	source/mesh_io.cpp 
	source/polyhedron_binary_op.cpp 
	source/polyhedron_unary_op.cpp 
	source/polyhedron.cpp 
	source/triangulate.cpp 
)

set( BOOLEAN_HEADERS
	include/mesh_functions.h
	include/mesh_io.h
	include/polyhedron_binary_op.h
	include/polyhedron_unary_op.h
	include/polyhedron.h
	include/triangulate.h 
)

add_executable( boolean_test source/boolean_test.cpp ${BOOLEAN_SOURCES} ${BOOLEAN_HEADERS} )

include_directories( ./include ${CARVE_INCLUDE_DIRS} )
target_link_libraries( boolean_test ${CARVE_LINK_LIBRARIES} )

if( BOOST_FOUND )
  include_directories( ${BOOST_INCLUDE_DIRS} )
  target_link_libraries (boolean_test ${BOOST_LIBRARIES})
endif( BOOST_FOUND )  