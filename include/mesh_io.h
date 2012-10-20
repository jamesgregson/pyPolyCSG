#ifndef MESH_IO_H
#define MESH_IO_H

/**
 @file mesh_io.h
 @author James Gregson (james.gregson@gmail.com)
 @brief Defines two public functions for loading/saving meshes to and from files.  Files are loaded by a pair of (non-public) functions defined in mesh_io.cpp, with the choice of which loader to call being determined by the file extension.  Currently there are loaders and savers for *.OBJ, *.OFF, *.STL (loading in progress), *.VTP and *.VTU with support for *.PLY files ongoing.  Not that the loaders and savers only handle the surface geometry of these files, and ignore attributes like texture coordinates and surfaces.
*/

#include<vector>

/**
 @brief Loads a mesh into the packed array data format used by the code
 @param[in] filename name of file to load
 @param[in] coords vector of packed coordinates [x,y,z,x,y,z,...]
 @param[in] faces vector of packed face indices [nverts_A,A0,A1,A2,...,nverts_B,B0,B1,B2,B3,...]
 @return true if load was successful, false otherwise
*/
bool load_mesh_file( const char *filename, std::vector<double> &coords, std::vector<int> &faces );

/**
 @brief Saves a mesh in the packed array data format to a file. The 
 @param[in] filename name of file to load
 @param[in] coords vector of packed coordinates [x,y,z,x,y,z,...]
 @param[in] faces vector of packed face indices [nverts_A,A0,A1,A2,...,nverts_B,B0,B1,B2,B3,...]
 @return true if load was successful, false otherwise
*/
bool save_mesh_file( const std::vector<double> &coords, const std::vector<int> &faces, const char *filename );

#endif