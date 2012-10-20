#include<iostream>
#include<vector>

#include"mesh_io.h"

// Probably the most basic mesh converter, takes two arguments, one for the input
// filename and one for the output.  Determines the file types based on the
// extension, loads the input mesh then writes it out to the output mesh.
int main( int argc, char **argv ){
	
	// if the number of arguments is wrong, print usage information
	if( argc != 3 ){
		std::cout << "Usage:" << std::endl;
		std::cout << "\t" << argv[0] << "input_file output_file" << std::endl;
		return 1;
	}
	
	// some vectors to hold the mesh data
	std::vector<double> coords;
	std::vector<int>    faces;
	
	// load the input file
	if( !load_mesh_file( argv[1], coords, faces ) ){
		std::cout << "failed to load mesh file: " << argv[1] << std::endl;
	}
	   
	// write the output file
	if( !save_mesh_file( coords, faces, argv[2] ) ){
		std::cout << "failed to save mesh file: " << argv[2] << std::endl;
	}
	   
	
	return 0;
}