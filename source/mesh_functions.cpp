#include<set>
#include<cmath>
#include<iostream>
#include"mesh_functions.h"

/*
 This function tests if an input mesh is a closed manifold (approximately), by making sure that each edge has exactly two neighbors. This function depend on the input mesh being oriented correctly (i.e. that faces have consistent windings)
*/
bool mesh_is_closed_manifold( const std::vector<double> &coords, const std::vector<int> &faces ){
	typedef std::pair<int,int> ii_pair;
	std::set<ii_pair> edges;
	std::set<ii_pair>::iterator edge_iter;
	
	int tmpi = 0;
	while( tmpi < (int)faces.size() ){
		// get the number of face vertices
		int nverts = faces[tmpi++];
		
		// loop over the face edges
		for( int i=0; i<nverts; i++ ){
			int v0 = faces[tmpi+i];
			int v1 = faces[tmpi+(i+1)%nverts];
			
			// If there is already an edge for the reverse edge (v1->v0) from
			// a previous facet, this edge balances it and the previous
			// edge can be erased.  Otherwise create an edge record
			// for the forward edge (v0->v1) and add it to the edge set
			edge_iter = edges.find( ii_pair( v1, v0 ) );
			if( edge_iter != edges.end() ){
				edges.erase( edge_iter );
			} else {
				// before adding an edge, we need to make sure it does not
				// already exist.  If it does exist, the mesh is not manifold
				edge_iter = edges.find( ii_pair( v0, v1 ) );
				if( edge_iter != edges.end() ){
					return false;
				}
				// edge does not exist, so add it to the edge list
				edges.insert( ii_pair(v0,v1) );
			}
		}
	}
	
	// when complete, the edge set should be true for a closed manifold mesh
	return edges.size() == 0;
}

bool mesh_estimate_facet_normal( const std::vector<double> &coords, const int *face_vtx, double *normal, double *D ){
	double L = 0.0;
	int nverts = face_vtx[0];
	const int *vtx = &face_vtx[1];
	
	normal[0]=normal[1]=normal[2]=0.0;
	for( int i=0; i<nverts; i++ ){
		int v0 = vtx[i]*3;
		int v1 = vtx[(i+1)%nverts]*3;
		normal[0] += (coords[v0+1]-coords[v1+1])*(coords[v0+2]+coords[v1+2]);
		normal[1] += (coords[v0+2]-coords[v1+2])*(coords[v0+0]+coords[v1+0]);
		normal[2] += (coords[v0+0]-coords[v1+0])*(coords[v0+1]+coords[v1+1]);
	}
	L = sqrt( normal[0]*normal[0]+normal[1]*normal[1]+normal[2]*normal[2]);
	// return false if there is a zero-area polygon
	if( fabs(L) < 1e-10 )
		return false;
	
	// otherwise normalize the normal vector
	normal[0] /= L;
	normal[1] /= L;
	normal[2] /= L;
	
	// if the plane equation D value is to be computed, loop over the 
	// vertices and compute it
	if( D != NULL ){
		*D = 0.0;
		for( int i=0; i<nverts; i++ ){
			*D -= normal[0]*coords[vtx[i]*3+0] + normal[1]*coords[vtx[i]*3+1] + normal[2]*coords[vtx[i]*3+2];
		}
	}
	
	return true;
}

