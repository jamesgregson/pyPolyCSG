from distutils.core import setup, Extension


# ==========================================================================
# Run the setup script =====================================================
# ==========================================================================
libraries = [ 'pyPolyCSG' ]
setup( name='pyPolyCSG', version='1.0', ext_modules=[Extension( 'pyPolyCSG', [], library_dirs=['.'], libraries=libraries )] )