cmake_minimum_required( VERSION 2.6 )

# create a basic project
project( boolean )

set( CARVE_INCLUDE_DIRS third_party/carve/include third_party/carve_build/include/carve )
set( CARVE_LIBRARY_DIRS third_party/carve_build/lib/Release )
set( CARVE_LIBRARIES carve )
add_definitions( -DCSG_USE_CARVE )
link_directories( ${CARVE_LIBRARY_DIRS} )

#set( CGAL_INCLUDE_DIRS /usr/local/include )
#set( CGAL_LIBRARY_DIRS /usr/local/lib/CGAL )
#set( CGAL_LIBRARIES CGAL_Core CGAL gmp mpfr )
add_definitions( -DCSG_USE_CARVE )
add_definitions( -frounding-math )
link_directories( ${CGAL_LIBRARY_DIRS} )

find_package( Boost REQUIRED )
IF( Boost_FOUND )
  SET( Boost_USE_STATIC_LIBS OFF )
  SET( Boost_USE_MULTITHREADED ON )
  SET( Boost_USE_STATIC_RUNTIME OFF )
  FIND_PACKAGE( Boost COMPONENTS python thread )
ENDIF()

SET( PYTHON_INCLUDE_DIRS /usr/include/python2.7 )
SET( PYTHON_LIBRARIES    /usr/local/lib/python )  

set( INCLUDE_DIRS 
	include ${Boost_INCLUDE_DIRS} 
	${CARVE_INCLUDE_DIRS} 
	${PYTHON_INCLUDE_DIRS}
)
set( LIBS 
	${CARVE_LIBRARIES} 
	${Boost_LIBRARIES} 
	${PYTHON_LIBRARIES}
)

set( BOOLEAN_SOURCES 
	source/mesh_functions.cpp 
	source/mesh_io.cpp 
	source/polyhedron_binary_op.cpp 
	source/polyhedron_unary_op.cpp 
	source/polyhedron.cpp 
	source/triangulate.cpp 
        source/python_wrapper.cpp
)

set( BOOLEAN_HEADERS
	include/mesh_functions.h
	include/mesh_io.h
	include/polyhedron_binary_op.h
	include/polyhedron_unary_op.h
	include/polyhedron.h
	include/triangulate.h 
)

include_directories( ${INCLUDE_DIRS} )

add_executable( boolean_test 
	source/boolean_test.cpp 
	${BOOLEAN_SOURCES} 
	${BOOLEAN_HEADERS} 
)
target_link_libraries( boolean_test ${LIBS} )
