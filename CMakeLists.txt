cmake_minimum_required( VERSION 2.6 )

# create a basic project
project( pyPolyCSG )

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

find_package( CARVE )
IF( CARVE_FOUND )
  message( STATUS "Found system Carve installation!" )
  set( INCLUDE_DIRS ${INCLUDE_DIRS} ${CARVE_INCLUDE_DIRS} )
  set( LIBS ${LIBS} ${CARVE_LIBRARIES} )
  add_definitions( -DCSG_USE_CARVE )
ENDIF( CARVE_FOUND )

if( NOT CSG_NO_CGAL )
  find_package( CGAL )
  IF( CGAL_FOUND )
    message( STATUS "Found system CGAL installation!" )
    set( INCLUDE_DIRS ${INCLUDE_DIRS} ${CGAL_INCLUDE_DIRS} )
    set( LIBS ${LIBS} ${CGAL_LIBRARIES} )
    include( ${CGAL_USE_FILE} )
    add_definitions( -DCSG_USE_CGAL )
  ENDIF( CGAL_FOUND )
ENDIF( NOT CSG_NO_CGAL )

IF( NOT CARVE_FOUND AND NOT CGAL_FOUND )
  message( STATUS "No system Carve or CGAL installation found, falling back to Carve in third_party/" )
  SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/third_party )
  find_package( CARVE )
  IF( CARVE_FOUND )
    message( STATUS "Found third_party/ Carve installation!" )
    set( INCLUDE_DIRS ${INCLUDE_DIRS} ${CARVE_INCLUDE_DIRS} )
    set( LIBS ${LIBS} ${CARVE_LIBRARIES} )
    add_definitions( -DCSG_USE_CARVE )
  ELSE(CARVE_FOUND)
    message( ERROR "One of CGAL or Carve must be installed to build pyPolyCSG!" )
  ENDIF(CARVE_FOUND)
ENDIF( NOT CARVE_FOUND AND NOT CGAL_FOUND )

find_package( PythonLibs REQUIRED )
IF( PYTHONLIBS_FOUND )
  set( INCLUDE_DIRS ${INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS} )
  set( LIBS ${LIBS} ${PYTHON_LIBRARIES} )
ENDIF( PYTHONLIBS_FOUND )

find_package( Boost COMPONENTS python thread REQUIRED )
IF( Boost_FOUND )
  SET( INCLUDE_DIRS ${INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} )
  SET( LIBS ${LIBS} ${Boost_LIBRARIES} )
ENDIF( Boost_FOUND )

IF( PYTHONLIBS_FOUND AND Boost_FOUND )
  set( BOOLEAN_SOURCES ${BOOLEAN_SOURCES} source/python_wrapper.cpp )
ENDIF( PYTHONLIBS_FOUND AND Boost_FOUND )

include_directories( include ${INCLUDE_DIRS} )
add_library( pyPolyCSG SHARED ${BOOLEAN_SOURCES} ${BOOLEAN_HEADERS} )
target_link_libraries( pyPolyCSG ${LIBS} )
IF( APPLE )
  # OS-X specific library naming
  #set_target_properties( pyPolyCSG PROPERTIES PREFIX "" )
  #set_target_properties( pyPolyCSG PROPERTIES SUFFIX .so )
ELSEIF( UNIX )
  # Unix specific library naming
ELSEIF( WIN32 )
  # Windows specific library naming
ENDIF( APPLE )
set_target_properties( pyPolyCSG PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY ..
  LIBRARY_OUTPUT_DIRECTORY_DEBUG ..
  LIBRARY_OUTPUT_DIRECTORY_RELEASE ..
)


#add_executable( pyPolyCSG_test source/boolean_test.cpp )
#target_link_libraries( pyPolyCSG_test pyPolyCSG )
