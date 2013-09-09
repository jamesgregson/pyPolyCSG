#include<vector>
#include<iostream>
#include<sstream>
#include<fstream>
#include<algorithm>
#include<iterator>
#include<string>
#include<cmath>
#include<cstdlib>
#include<map>

#ifdef CSG_USE_VTK
#include<vtkSmartPointer.h>
#include<vtkCellTypes.h>
#include<vtkCell.h>
#include<vtkTriangle.h>
#include<vtkQuad.h>
#include<vtkPolygon.h>
#include<vtkPoints.h>
#include<vtkPolyData.h>
#include<vtkCellArray.h>
#include<vtkXMLPolyDataReader.h>
#include<vtkXMLPolyDataWriter.h>
#include<vtkUnstructuredGrid.h>
#include<vtkXMLUnstructuredGridReader.h>
#include<vtkXMLUnstructuredGridWriter.h>
#endif

#include"mesh_io.h"
#include"polyhedron.h"


// forward declarations of loading functions, these must be added to the load_mesh_file() function
// cases in order to be used
bool load_mesh_file_off( const char *filename, std::vector<double> &coords, std::vector<int> &faces );
bool load_mesh_file_obj( const char *filename, std::vector<double> &coords, std::vector<int> &faces );
bool load_mesh_file_stl( const char *filename, std::vector<double> &coords, std::vector<int> &faces );
bool load_mesh_file_vtp( const char *filename, std::vector<double> &coords, std::vector<int> &faces );
bool load_mesh_file_vtu( const char *filename, std::vector<double> &coords, std::vector<int> &faces );
bool load_mesh_file_ply( const char *filename, std::vector<double> &coords, std::vector<int> &faces );
bool load_mesh_file_wrl( const char *filename, std::vector<double> &coords, std::vector<int> &faces );

// forward declarations of saving functions, these must be added to the save_mesh_file() function
// cases in order to be used
bool save_mesh_file_off( const std::vector<double> &coords, const std::vector<int> &faces, const char *filename );
bool save_mesh_file_obj( const std::vector<double> &coords, const std::vector<int> &faces, const char *filename );
bool save_mesh_file_stl( const std::vector<double> &coords, const std::vector<int> &faces, const char *filename );
bool save_mesh_file_vtp( const std::vector<double> &coords, const std::vector<int> &faces, const char *filename );
bool save_mesh_file_vtu( const std::vector<double> &coords, const std::vector<int> &faces, const char *filename );
bool save_mesh_file_ply( const std::vector<double> &coords, const std::vector<int> &faces, const char *filename );
bool save_mesh_file_wrl( const std::vector<double> &coords, const std::vector<int> &faces, const char *filename );


// determines the extension of the current file, returning it in lower-case
const char *mesh_io_get_file_extension( const char *in ){
	int id;
	std::vector< std::string > tokens;
	std::string input( in );
	std::replace( input.begin(), input.end(), '.', ' ' );
	std::istringstream iss( input );
	std::copy( std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter< std::vector<std::string> >( tokens ) );
	if( tokens.size() > 0 ){
		id = (int)tokens.size()-1;
		std::string tmp = tokens[id];
		std::transform( tmp.begin(), tmp.end(), tmp.begin(), tolower );
		return tmp.c_str();
	}
	return "";
}

bool load_mesh_file( const char *filename, std::vector<double> &coords, std::vector<int> &faces ){
	// get the file extension
	const char *ext = mesh_io_get_file_extension( filename );
	
	// check the possible extensions
	if( strcmp( ext, "off") == 0 ){
		// call *.off loader
		return load_mesh_file_off( filename, coords, faces );
	} else if( strcmp(ext, "obj") == 0 ){
		// call *.obj loader
		return load_mesh_file_obj( filename, coords, faces );
	} else if( strcmp(ext, "stl") == 0 ){
		// call *.std loader
		return load_mesh_file_obj( filename, coords, faces );
	} else if( strcmp(ext, "vtp") == 0 ){
		// call vtkPolyData loader
		return load_mesh_file_vtp( filename, coords, faces );
	} else if( strcmp(ext, "vtu") == 0 ){
		// call vtkUnstructuredGrid loader
		return load_mesh_file_vtu( filename, coords, faces );
	} else if( strcmp(ext, "ply") == 0 ){
		// call the ply loader
		return load_mesh_file_ply( filename, coords, faces );
	} else if( strcmp(ext, "wrl") == 0 ){
		// call the vrml loader
		return load_mesh_file_wrl( filename, coords, faces );
	}

	// if one of the loaders did not succeed, return failure
	return false;
}

bool save_mesh_file( const std::vector<double> &coords, const std::vector<int> &faces, const char *filename ){
	// get the file extension
	const char *ext = mesh_io_get_file_extension( filename );
	    
	// check the possible extensions
	if( strcmp(ext, "off") == 0 ){
		// call *.off saver
		return save_mesh_file_off( coords, faces, filename );
	} else if( strcmp(ext, "obj") == 0 ){
		// call *.obj saver
		return save_mesh_file_obj( coords, faces, filename );
	} else if( strcmp(ext, "stl") == 0 ){
		// call *.std saver
		return save_mesh_file_stl( coords, faces, filename );
	} else if( strcmp(ext, "vtp") == 0 ){
		// call vtkPolyData saver
		return save_mesh_file_vtp( coords, faces, filename );
	} else if( strcmp(ext, "vtu") == 0 ){
		// call vtkUnstructuredGrid saver
		return save_mesh_file_vtu( coords, faces, filename );
	} else if( strcmp(ext, "ply") == 0 ){
		// call the ply saver
		return save_mesh_file_ply( coords, faces, filename );
	} else if( strcmp(ext, "wrl") == 0 ){
		// call the VRML saver
		return save_mesh_file_wrl( coords, faces, filename );
	}
	
	// if one of the savers did not succeed, return failure
	return false;
}
	
bool load_mesh_file_off( const char *filename, std::vector<double> &coords, std::vector<int> &faces ){
	int nverts, nfaces, sid, tmpi;
	double x, y, z;
	std::string token;
	
	// open the input file for reading
	std::ifstream input( filename );
	
	// check that the stream opened successfully
	if( input.fail() )
		return false;
	
	// compute the starting vertex index
	sid = (int)coords.size()/3;
	
	// read the header, number of vertices, faces and edges
	input >> token >> nverts >> nfaces >> tmpi;
	
	// check that the file is an OFF file
	if( token != std::string("OFF") )
		return false;
	
	// now read the vertices
	for( int i=0; i<nverts; i++ ){
		input >> x >> y >> z;
		coords.push_back( x );
		coords.push_back( y );
		coords.push_back( z );
	}
	
	// now read the faces
	for( int i=0; i<nfaces; i++ ){
		// read the number of face vertices
		input >> nverts;
		faces.push_back( nverts );
		
		for( int j=0; j<nverts; j++ ){
			// add the vertex to the face, offseting by
			// the number of vertices initially in the coords array
			input >> tmpi;
			faces.push_back( tmpi+sid );
		}
	}
	
	// close the input file
	input.close();
	
	return true;
}

bool load_mesh_file_obj( const char *filename, std::vector<double> &coords, std::vector<int> &faces ){
	int sid, tmpi;
	double x, y, z;
	std::string line, token;
	
	// open the input file for reading
	std::ifstream input( filename );
	
	// check that the stream opened successfully
	if( input.fail() )
		return false;

	// get the starting vertex id
	sid = (int)coords.size()/3;
	
	// loop over the file line by line
	while( !(getline(input,line)).fail() ){
		if( line[0] == 'v'  && isspace(line[1]) ){
			// load a vertex
			std::istringstream iss( line );
			iss >> token >> x >> y >> z;
			coords.push_back( x );
			coords.push_back( y );
			coords.push_back( z );
		} else if( line[0] == 'f' ){
			// load a face
			std::vector<int> vid;
			std::istringstream iss( line );
			iss >> token; // grab the 'f' 
			while( iss >> token ){
				// have to do this stuff because the vertices may have texture 
				// coordinates and normals included
				std::replace( token.begin(), token.end(), '/', ' ' );
				std::istringstream iss2( token );
				iss2 >> tmpi;
				vid.push_back( tmpi-1 );
			}
			faces.push_back( vid.size() );
			for( int i=0; i<(int)vid.size(); i++ ){
				faces.push_back( vid[i] );
			}
		}
	}
	
	// close the input file
	input.close();
	
	return true;
}

bool load_mesh_file_stl( const char *filename, std::vector<double> &coords, std::vector<int> &faces ){
	// TODO: implement this function, need a way of searching for coincident vertices and merging them
	std::cout << "TODO: STL loading is not yet complete" << std::endl;
	return false;
}

bool load_mesh_file_vtp( const char *filename, std::vector<double> &coords, std::vector<int> &faces ){
#if defined(CSG_USE_VTK)
	vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
	reader->SetFileName( filename );
	reader->Update();
	vtkSmartPointer<vtkPolyData> polydata = reader->GetOutput();
	
	// load the datasets points
	int npoints = (int)polydata->GetNumberOfPoints();
	for( int i=0; i<npoints; i++ ){
		double xyz[3];
		polydata->GetPoint( i, xyz );
		coords.push_back( xyz[0] );
		coords.push_back( xyz[1] );
		coords.push_back( xyz[2] );
	}

	// load the polygonal faces
	vtkSmartPointer<vtkCellArray> cells = polydata->GetPolys();
	vtkIdType npts, *pnt_id = new vtkIdType[ cells->GetMaxCellSize() ];
	cells->InitTraversal();
	while( cells->GetNextCell( npts, pnt_id ) ){
		faces.push_back( npts );
		for( int i=0; i<npts; i++ ){
			faces.push_back( pnt_id[i] );
		}
	}
	
	// TODO: add strip loading
	
	return true;
#else
	std::cout << "Error: Loading from vtkPolyData inputs (*.vtp) disabled, enable by recompiling psCSG with CSG_USE_VTK defined" << std::endl;
	return false;
#endif
}

bool load_mesh_file_vtu( const char *filename, std::vector<double> &coords, std::vector<int> &faces ){
#if defined(CSG_USE_VTK)
	// load the file and create the (initial) facet list
	vtkSmartPointer<vtkXMLUnstructuredGridReader> reader = vtkSmartPointer<vtkXMLUnstructuredGridReader>::New();
	reader->SetFileName( filename );
	reader->Update();
	vtkSmartPointer<vtkUnstructuredGrid> grid = reader->GetOutput();
	
	for( int i=0; i<grid->GetNumberOfCells(); i++ ){
		vtkCell *c = grid->GetCell(i);
		switch( c->GetCellType() ){
			case VTK_TRIANGLE:
				faces.push_back( 3 );
				faces.push_back( c->GetPointId(0) );
				faces.push_back( c->GetPointId(1) );
				faces.push_back( c->GetPointId(2) );
				break;
			case VTK_QUAD:
				faces.push_back( 4 );
				faces.push_back( c->GetPointId(0) );
				faces.push_back( c->GetPointId(1) );
				faces.push_back( c->GetPointId(2) );
				faces.push_back( c->GetPointId(3) );
				break;
			case VTK_POLYGON:
				{
					faces.push_back( c->GetNumberOfPoints() );
					for( int j=0; j<c->GetNumberOfPoints(); j++ ){
						faces.push_back( c->GetPointId(j) );
					}
				}
				break;
		}
	}
	
	// renumber all the vertices so that only vertices on surface
	// faces remain
	int next_id = 0;
	std::map<int,int> vid_map;
	std::map<int,int>::iterator iter;
	int tmpi = 0;
	while( tmpi < (int)faces.size() ){
		int nverts = faces[tmpi++];
		for( int i=0; i<nverts; i++ ){
			int vid = faces[tmpi];
			// check if there is already an entry for this
			// vertex id, if not, add the vertex
			iter = vid_map.find( vid );
			if( iter == vid_map.end() ){
				// vertex not found, get its coordinates from
				// the input mesh and add them to the coords array
				double xyz[3];
				grid->GetPoint( vid, xyz );
				coords.push_back( xyz[0] );
				coords.push_back( xyz[1] );
				coords.push_back( xyz[2] );
				
				// then add them to the renumbering list
				vid_map[vid] = next_id++;
			}
			faces[tmpi] = vid_map[vid];
			tmpi++;
		}
	}
	
	return true;
#else
	std::cout << "Error: Loading from vtkUnstructuredGrid inputs (*.vtu) disabled, enable by recompiling psCSG with CSG_USE_VTK defined" << std::endl;
	return false;
#endif
}

bool load_mesh_file_ply( const char *filename, std::vector<double> &coords, std::vector<int> &faces ){
	// TODO: implement this function
	std::cout << "TODO: PLY loading is not yet complete" << std::endl;
	return false;
}

bool load_mesh_file_wrl( const char *filename, std::vector<double> &coords, std::vector<int> &faces ){
	// TODO: implement this function
	std::cout << "TODO: VRML loading is not yet complete" << std::endl;
	return false;
}


bool save_mesh_file_off( const std::vector<double> &coords, const std::vector<int> &faces, const char *filename ){
	int nverts, nfaces, tmpi;
	
	// open the output file and check for success
	std::ofstream output(filename);
	if( output.fail() )
		return false;
	
	// compute the number of vertices and faces
	nverts = (int)coords.size()/3;
	nfaces = tmpi = 0;
	while( tmpi < (int)faces.size() ){
		tmpi += 1+faces[tmpi];
		nfaces++;
	}
	
	// write out the header
	output << "OFF" << std::endl;
	output << nverts << " " << nfaces << " " << 0 << std::endl;
	
	// write out the vertices
	for( int i=0; i<(int)coords.size(); i+=3 ){
		output << coords[i+0] << " " << coords[i+1] << " " << coords[i+2] << std::endl;
	}
	
	// write out the faces
	tmpi = 0;
	while( tmpi < (int)faces.size() ){
		nverts = faces[tmpi];
		output << faces[tmpi++] << " ";
		for( int i=0; i<nverts; i++ ){
			output << faces[tmpi++] << " ";
		}
		output << std::endl;
	}
	
	// close the output file
	output.close();
	
	return true;
}

bool save_mesh_file_obj( const std::vector<double> &coords, const std::vector<int> &faces, const char *filename ){
	int nverts, tmpi;
	
	// open the output file and check for success
	std::ofstream output(filename);
	if( output.fail() )
		return false;
	
	// compute the number of vertices and faces
	for( int i=0; i<(int)coords.size(); i+=3 ){
		output << "v " << coords[i+0] << " " << coords[i+1] << " " << coords[i+2] << std::endl;
	}
	tmpi = 0;
	while( tmpi < (int)faces.size() ){
		nverts = faces[tmpi++];
		output << "f ";
		for( int i=0; i<nverts; i++ ){
			output << faces[tmpi++]+1 << " ";
		}
		output << std::endl;
	}
	
	output.close();
	
	return true;
}

// computes the normal of a triangle defined by points 'a', 'b', and 'c', storing the result in 'n'
bool mesh_io_compute_normal( const double *a, const double *b, const double *c, double *n ){
	double len;
	
	// compute normal vector
	double u[] = { b[0]-a[0], b[1]-a[1], b[2]-a[2] };
	double v[] = { c[0]-a[0], c[1]-a[1], c[2]-a[2] };
	n[0] = u[1]*v[2] - u[2]*v[1];
	n[1] = u[2]*v[0] - u[0]*v[2];
	n[2] = u[0]*v[1] - u[1]*v[0];
	
	// compute vector length, return false if it is
	// too small, which indicates a degenerate edge
	len = sqrt( n[0]*n[0] + n[1]*n[1] + n[2]*n[2] );
	if( len < 1e-10 )
		return false;
	
	// otherwise return true
	return true;
}

// save a mesh file as STL format
// TODO: handle endianess
bool save_mesh_file_stl( const std::vector<double> &coords, const std::vector<int> &faces, const char *filename ){
	
	// STL only supports triangular faces, but the CSG operations performed
	// using the Carve backend can produce arbitrary simple polygons.  To
	// handle this, create a polyhedron from the input mesh, triangulate it
	// and then export the triangulated mesh
	polyhedron tmp;
	tmp.initialize_load_from_mesh( coords, faces );
	tmp = tmp.triangulate();
	
	std::vector<double> tcoords;
	std::vector<int> tfaces;
	tmp.output_store_in_mesh( tcoords, tfaces );
	
	int tmpi, nfaces;
	
	// check for non-triangular faces and count the total number of faces
	nfaces = tmpi = 0;
	while( tmpi < (int)tfaces.size() ){
		if( tfaces[tmpi] != 3 ){
			std::cout << "Error, file in " << __FILE__ << ", line " << __LINE__ << ": Non-triangular faces are not supported for STL output. Triangulate mesh before saving." << std::endl;
			return false;
		}
		tmpi += 1+tfaces[tmpi];
		nfaces++;
	}
	
	// open the output stream
	std::ofstream output( filename, std::ios_base::binary );
	if( output.fail() ){
		return false;
	}
	
	// write the file header (all zeros)
	char header[80];
	memset( header, 0, 80 );
	output.write( header, 80 );
	output.write( (const char*)&nfaces, sizeof(int) );
	
	// write the triangles (all faces are triangles)
	for( int i=0; i<(int)tfaces.size(); i+=4 ){
		double dnorm[3];
		int v0 = 3*tfaces[i+1], v1 = 3*tfaces[i+2], v2 = 3*tfaces[i+3];
		
		// compute and write the normal to the output
		mesh_io_compute_normal( &tcoords[v0], &tcoords[v1], &tcoords[v2], dnorm );
		float xyz[] = { (float)dnorm[0], (float)dnorm[1], (float)dnorm[2] };
		output.write( (const char*)&xyz[0], 3*sizeof(float) );
		
		// write the first vertex of the triangle to the output
		xyz[0] = (float)tcoords[v0+0]; xyz[1] = (float)tcoords[v0+1]; xyz[2] = (float)tcoords[v0+2];
		output.write( (const char*)&xyz[0], 3*sizeof(float) );

		// write the first vertex of the triangle to the output
		xyz[0] = (float)coords[v1+0]; xyz[1] = (float)coords[v1+1]; xyz[2] = (float)tcoords[v1+2];
		output.write( (const char*)&xyz[0], 3*sizeof(float) );
		
		// write the first vertex of the triangle to the output
		xyz[0] = (float)tcoords[v2+0]; xyz[1] = (float)tcoords[v2+1]; xyz[2] = (float)tcoords[v2+2];
		output.write( (const char*)&xyz[0], 3*sizeof(float) );
		
		// write the 16 bit flag at the end of the triangle
		unsigned short stmp = 0;
		output.write( (const char*)&stmp, sizeof(unsigned short) );
	}
	
	// close the output file
	output.close();
	
	return true;
}

bool save_mesh_file_vtp( const std::vector<double> &coords, const std::vector<int> &faces, const char *filename ){
#if defined(CSG_USE_VTK)
	vtkSmartPointer<vtkPoints>     points = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray>  cells  = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkPolyData>   polydata = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();

	
	// create the coordinate array
	for( int i=0; i<(int)coords.size(); i+=3 ){
		points->InsertNextPoint( coords[i], coords[i+1], coords[i+2] );
	}
	
	// create the cell array
	int tmpi = 0;
	while( tmpi < (int)faces.size() ){
		std::vector<vtkIdType> pnt_ids;
		int npts = faces[tmpi++];
		for( int i=0; i<npts; i++ ){
			pnt_ids.push_back( faces[tmpi++] );
		}
		cells->InsertNextCell( npts, &pnt_ids[0] );
	}
	polydata->SetPoints( points );
	polydata->SetPolys( cells );

	writer->SetInput( polydata );
	writer->SetFileName( filename );
	writer->Write();
	
	return true;
#else
	std::cout << "Error: Saving to vtkPolyData outputs (*.vtp) disabled, enable by recompiling psCSG with CSG_USE_VTK defined" << std::endl;
	return false;
#endif	
}

bool save_mesh_file_vtu( const std::vector<double> &coords, const std::vector<int> &faces, const char *filename ){
#if defined(CSG_USE_VTK)
	vtkSmartPointer<vtkXMLUnstructuredGridWriter> writer = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkUnstructuredGrid> grid = vtkSmartPointer<vtkUnstructuredGrid>::New();
	
	// add the points to the grid
	for( int i=0; i<(int)coords.size(); i+=3 ){
		points->InsertNextPoint( &coords[i] );
	}
	
	int tmpi = 0;
	while( tmpi < (int)faces.size() ){
		int nverts = faces[tmpi++];
		std::vector<vtkIdType> vlist;
		for( int i=0; i<nverts; i++ ){
			vlist.push_back( faces[tmpi++] );
		}
		switch( nverts ){
			case 3:
				grid->InsertNextCell( VTK_TRIANGLE, 3, &vlist[0] );
				break;
			case 4:
				grid->InsertNextCell( VTK_QUAD, 4, &vlist[0] );
				break;
			default:
				grid->InsertNextCell( VTK_POLYGON, nverts, &vlist[0] );
				break;
		}
	}
	
	grid->SetPoints( points );
	writer->SetFileName( filename );
	writer->SetInput( grid );
	writer->Write();
	
	return true;
#else
	std::cout << "Error: Saving to vtkUnstructuredGrid outputs (*.vtu) disabled, enable by recompiling psCSG with CSG_USE_VTK defined" << std::endl;
	return false;
#endif	
}

bool save_mesh_file_ply( const std::vector<double> &coords, const std::vector<int> &faces, const char *filename ){
	// TODO: implement this function
	std::cout << "TODO: ply saving is not yet complete" << std::endl;
	return false;
}

bool save_mesh_file_wrl( const std::vector<double> &coords, const std::vector<int> &faces, const char *filename ){
	// TODO: implement this function
	std::cout << "TODO: wrl saving is not yet complete" << std::endl;
	return false;
}


