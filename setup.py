from distutils.core import setup, Extension
# ==========================================================================
# INFORMATION ==============================================================
# ==========================================================================
#
# Installs Boost::python wrappers for the CSG library. See the README for
# license and install information 
#
# Install information is available in the README file in this directory.
# You will need to modify the paths in the following sections for the
# appropriate Boost install directory & library name.
#
# Required dependencies are Boost and Carve. CGAL support is planned but
# not implemented. VTK is optional and allows output of *.vtp files if
# used (by default it is disabled).
#
# The third_party/ subdirectory contains a modified version of
# Carve-1.4.0 that compiles and builds under XCode 4.5 (my development
# machine), fixing some minor compile errors that occur with LLVM.  GCC
# MSVC users *may* be able to use this modified version. If not, the
# official distribution for version 1.4.0 is available from:
# http://carve.googlecode.com/files/carve-1.4.0.tgz
#
# Once paths are setup correctly, install with:
#
# sudo python setup.py install
#
# Then see the examples in the scripts/ subdirectory.

# ==========================================================================
# Compiler paths and defines ===============================================
# ==========================================================================
defines        = [ ]
compiler_flags = [ ]
include_dirs   = [ 'include' ]
library_dirs   = [ ]
libraries      = [ ]

# ==========================================================================
# Setup for Boost ==========================================================
# ==========================================================================
boost_defines        = [ ]
boost_compiler_flags = [ ]
boost_include_dirs   = [ '/usr/local/include' ]
boost_library_dirs   = [ '/usr/local/lib' ]
boost_libraries      = [ 'boost_python-mt' ]

# ==========================================================================
# Setup for Carve ==========================================================
# ==========================================================================
use_carve = True
carve_defines        = [ ('CSG_USE_CARVE', '1') ]
carve_compiler_flags = []
carve_include_paths  = [ './third_party/carve/include', './third_party/carve_build/include' ]
carve_library_paths  = [ './third_party/carve_build/lib' ]
carve_libraries      = [ 'carve' ]

# ==========================================================================
# Setup for CGAL ===========================================================
# ==========================================================================
use_cgal = False
cgal_defines        = [ ('CSG_USE_CGAL','1') ]
cgal_compiler_flags = [ '-frounding-math' ]
cgal_include_paths  = [ '/usr/local/include' ]
cgal_library_paths  = [ '/usr/local/lib' ]
cgal_libraries      = [ 'cgal' ]

# ==========================================================================
# Setup for VTK ============================================================
# ==========================================================================
use_vtk = False
vtk_defines        = [ ('CSG_USE_VTK','1') ]
vtk_compiler_flags = []
vtk_include_paths  = [ '/usr/local/include/vtk-5.8' ]
vtk_library_paths  = [ '/usr/local/lib/vtk-5.8' ]
vtk_libraries = [ 'vtkalglib', 'vtkCharts', 'vtkCommon', 'vtkGraphics', 'vtkDICOMParser', 'vtkexoIIc', 'vtkexpat', 'vtkFiltering', 'vtkfreetype', 'vtkftgl', 'vtkGenericFiltering', 'vtkGeovis', 'vtkHybrid', 'vtkImaging', 'vtkInfovis', 'vtkIO', 'vtklibxml2', 'vtkNetCDF', 'vtkproj4', 'vtkRendering', 'vtksys', 'vtkverdict', 'vtkViews', 'vtkVolumeRendering', 'vtkWidgets', 'vtkzlib' ]

# ==========================================================================
# pyCSG Extension module source files ======================================
# ==========================================================================
sources = ['source/mesh_io.cpp', 'source/polyhedron.cpp', 'source/polyhedron_binary_op.cpp', 'source/polyhedron_unary_op.cpp', 'source/triangulate.cpp', 'source/python_wrapper.cpp' ]

# ==========================================================================
# Combine all paths and options into final build options ===================
# ==========================================================================
include_dirs += boost_include_dirs
library_dirs += boost_library_dirs
libraries    += boost_libraries
defines      += boost_defines

# add the carve options to the build, if needed
if use_carve:
    include_dirs += carve_include_paths
    library_dirs += carve_library_paths
    libraries    += carve_libraries
    defines      += carve_defines

# add the cgal options to the build, if needed
if use_cgal:
    include_dirs += cgal_include_paths
    library_dirs += cgal_library_paths
    libraries    += cgal_libraries
    defines      += cgal_defines

# add the vtk options to the build, if needed
if use_vtk:
    include_dirs += vtk_include_paths
    library_dirs += vtk_library_paths
    libraries    += vtk_libraries
    defines      += vtk_defines

# ==========================================================================
# Run the setup script =====================================================
# ==========================================================================
setup( name='pyPolyCSG', version='1.0', ext_modules=[Extension( 'pyPolyCSG', sources, define_macros=defines, extra_compile_args=compiler_flags, include_dirs=include_dirs, library_dirs=library_dirs, libraries=libraries )] )