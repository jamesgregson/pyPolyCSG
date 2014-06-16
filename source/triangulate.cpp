#include<set>
#include<cmath>
#include<iostream>

#include"triangulate.h"

/**
 @file triangulate.cpp
 @brief Set of functions for triangulation of simple, nearly planar polygons
*/

/** 
 @brief Defines a tolerance for what is considered convex, and what is considered inside a triangle.  This is unnecessary if exact predicates are used.
*/
#define TRIANGULATE_EPSILON 1e-12

/**
 @brief Computes whether the angle formed by points a, b, and c is convex.  Convexity is determined by the dot product of the cross-product of (b-a)x(c-1) with the input normal vector.  If the cross-produce vector points along the normal vector, the angle is deemed convex.  This allows non-planar polygons to be tesselated given an estimated normal vector, rather than requiring all vertices lie on a plane.
 @param[in] normal input assumed normal vector for the polygon (and triangle formed by [a,b,c].  Input parameters should lie approximately on this plane.
 @param[in] a point occuring before point b on the polygon contour
 @param[in] b point to compute convexity of
 @param[in] c point after point b
 @return convexity of point b, dot( normal, cross( b-a, c-a ) ). Positive values indicate angle at b is convex, negative values indicate concavity.
*/
double compute_convexity( const double *normal, const double *a, const double *b, const double *c ){
	double u[] = { b[0]-a[0], b[1]-a[1], b[2]-a[2] };
	double v[] = { c[0]-a[0], c[1]-a[1], c[2]-a[2] };
	return normal[0]*(u[1]*v[2]-u[2]*v[1]) + normal[1]*(u[2]*v[0]-u[0]*v[2]) + normal[2]*(u[0]*v[1]-u[1]*v[0]);
}

/**
 @brief determines if point p lies in triangle [a,b,c]. Points a, b and c are assumed to lie on a triangle with surface-normal similar to normal.  Determines whether point is in triangle by checking whether angles [a,b,p], [a, c, p], [c, a, p] are convex with respect to the normal vector, as in compute_convexity().  TRIANGLE_EPSILON is used to return true if points lie very close to, but strictly outside or on the boundary of the triangle.
 @param[in] normal assumed normal for the triangle
 @param[in] a first triangle point
 @param[in] b second triangle point
 @param[in] c third triangle point
 @param[in] p point to test for inclusion in the triangle
 @return true if the point is inside, or extremely close to the triangle, false otherwise
*/
bool point_in_triangle( const double *normal, const double *a, const double *b, const double *c, const double *p ){
	return compute_convexity( normal, a, b, p ) >= -TRIANGULATE_EPSILON 
	    && compute_convexity( normal, b, c, p ) >= -TRIANGULATE_EPSILON
	    && compute_convexity( normal, c, a, p ) >= -TRIANGULATE_EPSILON;
}

/**
 @brief Estimates a (potentially non-planar) facet's normal by using Newell's method.  Used for triangulation of non-planar polygons.  Triangulation may fail for polygons that are highly non-planar.
 @param[in] coords input array of vertex coordinates, packed [x, y, z, x, y, z, ...]
 @param[in] face_vtx input array of polygon contour indices, packed [nverts, v(0), v(1), ..., v(nverts-1) ]
 @param[out] normal output normal vector, as estimated by Newell's method
 @param[out] D option output plane-equation D value.
 @return true if normal computation succeeded, false otherwise
*/
bool triangulate_estimate_facet_normal( const std::vector<double> &coords, const int *face_vtx, double *normal, double *D=NULL ){
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

bool triangulate_simple_polygon_naive( const std::vector<double> &coords, const int *facet, std::vector<int> &tris ){
	// get the number of facet vertices
	int num_verts = facet[0];
	
	// store some vectors of next and previous vertices, as well
	// as pointers to the xyz coordinates, original vertex id and
	// a convexity flag indicating which vertices can be clipped.
	std::vector<int>			vert( num_verts );		// stores the input vertex id 
	std::vector<int>			next( num_verts );		// stores the index of the next vertex in the contour
	std::vector<int>			prev( num_verts );		// stores the index of the previous vertex in the contour
	std::vector<const double*>	xyz( num_verts );		// stores pointers to the xyz coordinates of each vertex
	std::vector<double>			convexity( num_verts ); // stores the convexity status of each vertex, >0 == convex
	double normal[3];
	
	// compute the facet normal
	triangulate_estimate_facet_normal( coords, facet, normal );
	
	// set up the linked list pointers, vertex indices and coordinates
	for( int i=0; i<num_verts; i++ ){
		vert[i] = facet[i+1];
		xyz[i] = &coords[vert[i]*3];
		prev[i] = (i-1+num_verts)%num_verts;
		next[i] = (i+1)%num_verts;
	}
	
	// compute the convexity of each vertex
	for( int i=0; i<num_verts; i++ ){
		convexity[i] = compute_convexity( normal, xyz[prev[i]], xyz[i], xyz[next[i]] );
	}
	
	// Main loop.  A stopping criteria has been aded so that the 
	// algorithm does not stall on bad inputs where no progress can be made.
	// For simple polygons in the worst case, the algorithm will remove
	// one vertex for every full traversal of the polygon.  This gives an
	// upper bound of (num_verts^2)/2 that the main loop should execute,
	// although in practice it will typically do much better. A bound of
	// num_verts^2 will consequently be more than sufficient for the 
	// algorithm to complete on valid inputs, failure to finish in that
	// many iterations indicates either improper input, or input that
	// cannot be tesselated due to precision of the non-exact tests.
	bool okay;
	int test, tmp_next, tmp_prev, max_tries=num_verts*num_verts, tries=0, curr = 0;
	while( num_verts > 2 && tries++ < max_tries ){
		
		// if the current vertex is (strictly) convex
		if( convexity[curr] > TRIANGULATE_EPSILON ){
			
			// then check the other vertices in the polygon to see if 
			// they fall within the triangle that would be clipped. 
			// All vertices except those that are in the clipped triangle
			// are checked.  This section could (should) be replaced
			// with spatial partitioning searches for very large inputs
			okay = true;
			test = next[curr];
			while( okay && test != curr ){
				// This conditional is necessary, since we may have bridges connecting two contours to form
				// a simple polygon. This test prevents incorrectly failing when a vertex that will be on 
				// the clipped triangle appears later in the polydon due to one of these bridges
				if( vert[test] != vert[prev[curr]] && vert[test] != vert[curr] && vert[test] != vert[next[curr]] ){
					okay &= !point_in_triangle( normal, xyz[prev[curr]], xyz[curr], xyz[next[curr]], xyz[test] );
				}
				test = next[test];
			}
			
			// no vertices fell within the clipped triangle, so 
			// the triangle can be added to the output and the 
			// current vertex removed from the contour
			if( okay ){
				// decrement the number of vertices
				num_verts--;
				
				// store the next and previous vertices for convenience, it
				// avoids unnecessary nesting like next[prev[curr]] that happens
				// otherwise to avoid using vertex curr which gets removed.
				tmp_prev = prev[curr];
				tmp_next = next[curr];
				
				// add the triangle to the output
				tris.push_back( 3 );
				tris.push_back( vert[tmp_prev] );
				tris.push_back( vert[curr] );
				tris.push_back( vert[tmp_next] );
				
				// update the linked list indices
				next[tmp_prev] = tmp_next;
				prev[tmp_next] = tmp_prev;
				
				// update the convexity status of the next
				// and previous vertices, since it may
				// have changed after clipping
				convexity[tmp_prev] = compute_convexity( normal, xyz[prev[tmp_prev]], xyz[tmp_prev], xyz[next[tmp_prev]] );
				convexity[tmp_next] = compute_convexity( normal, xyz[prev[tmp_next]], xyz[tmp_next], xyz[next[tmp_next]] );
			}
		}
		
		// advance one point around the boundary, we do this regardless of whether
		// the vertex was clipped. It is always valid since we never invalidate
		// the current vertex, just clip it out of the loop (i.e. next[curr] will
		// point to a valid vertex in the contour, even though curr will never 
		// be reached again).
		curr = next[curr];
	}
	
	//std::cout << "took " << tries << " out of a maximum of " << max_tries << std::endl;
		
	// should always end up with num_verts-2 triangles when
	// tesselating a simple polygon. Failure to do so indicates
	// non-simple input, or input that cannot be resolved with 
	// the current TRIANGULATE_EPSILON setting
	return num_verts == 2;
}

class triangulate_compare {
private:
	std::vector<double> &m_convexity;
public:
	triangulate_compare( std::vector<double> &convexity ) : m_convexity(convexity) {}
	
	bool operator()( const int &a, const int &b ){
		//int ca = int(m_convexity[a]*12847563.0)%4341;
		//int cb = int(m_convexity[b]*12847563.0)%4341;
		double ca = m_convexity[a];
		double cb = m_convexity[b];
		return ca < cb || (ca == cb && a < b );
	}
};

/**
 @brief Triangulates a simple polygon using an ear-clipping algorithm and heuristic to (hopefully) reduce the time-complexity.  Always tries to clip the ear with minimal area first, under the assumption that this will have the least probability of enclosing an unrelated polygon vertex.  Worst-case complexity of O(N^3 log(N)), with a best-case complexity of O(N^2 log(N)), which the algorithm appears to achieve often in practice. Note that this explicitly checks every vertex in the polygon for inclusion in a clipped ear, spatial partitioning should reduce the the complexity to O(N log(N)).
 @param[in] coords input array of polygon vertices, stored [x, y, z, x, y, z, ...]
 @param[in] facet input pointer to facet contour vertex indices, stored [nverts, v_0, v_1, ... v_(n_verts-1}]
 @param[out] tris output vector of triangles, stored as [ 3, v0, v1, v2, 3, v0, v1, v2, ... ]
 @return true if the triangulation succeeded, false otherwise
*/
bool triangulate_simple_polygon_set( const std::vector<double> &coords, const int *facet, std::vector<int> &tris ){
	// get the number of facet vertices
	int num_verts = facet[0];
	
	// store some vectors of next and previous vertices, as well
	// as pointers to the xyz coordinates, original vertex id and
	// a convexity flag indicating which vertices can be clipped.
	std::vector<int>			vert( num_verts );		// stores the input vertex id 
	std::vector<int>			next( num_verts );		// stores the index of the next vertex in the contour
	std::vector<int>			prev( num_verts );		// stores the index of the previous vertex in the contour
	std::vector<const double*>	xyz( num_verts );		// stores pointers to the xyz coordinates of each vertex
	std::vector<double>			convexity( num_verts ); // stores the convexity status of each vertex, >0 == convex
	double normal[3];
	
	triangulate_compare comp( convexity );
	std::set< int, triangulate_compare > best_vert( comp );
	std::set< int, triangulate_compare >::iterator iter;
	
	// compute the facet normal
	triangulate_estimate_facet_normal( coords, facet, normal );
	
	// set up the linked list pointers, vertex indices and coordinates
	for( int i=0; i<num_verts; i++ ){
		vert[i] = facet[i+1];
		xyz[i] = &coords[vert[i]*3];
		prev[i] = (i-1+num_verts)%num_verts;
		next[i] = (i+1)%num_verts;
	}
	
	// compute the convexity of each vertex
	for( int i=0; i<num_verts; i++ ){
		convexity[i] = compute_convexity( normal, xyz[prev[i]], xyz[i], xyz[next[i]] );
		if( convexity[i] > TRIANGULATE_EPSILON ){
			best_vert.insert( i );
		}
	}
	
	// Main loop.  A stopping criteria has been aded so that the 
	// algorithm does not stall on bad inputs where no progress can be made.
	// For simple polygons in the worst case, the algorithm will remove
	// one vertex for every full traversal of the polygon.  This gives an
	// upper bound of (num_verts^2)/2 that the main loop should execute,
	// although in practice it will typically do much better. A bound of
	// num_verts^2 will consequently be more than sufficient for the 
	// algorithm to complete on valid inputs, failure to finish in that
	// many iterations indicates either improper input, or input that
	// cannot be tesselated due to precision of the non-exact tests.
	bool okay;
	int test, curr, tmp_next, tmp_prev, init_verts=num_verts, tries=0, max_tries=num_verts*num_verts;
	for( int i=0; i<init_verts-2; i++ ){
		
		// Grab the first vertex from the sorted list of vertices. This should
		// be the 'best' vertex to clip according to the criteria in the 
		// triangulate_compare class, which could be the smallest area triangle,
		// or a randomized metric.  We have to iterate over these in order
		// to make sure that we actually find one that can be clipped, since the
		// first might produce a triangle that encloses another vertex
		for( iter=best_vert.begin(); iter!=best_vert.end(); iter++ ){
			curr = *iter;
			tries++;
			
			okay = true;
			test = next[curr];
			while( okay && test != curr ){
				// This conditional is necessary, since we may have bridges connecting two contours to form
				// a simple polygon. This test prevents incorrectly failing when a vertex that will be on 
				// the clipped triangle appears later in the polydon due to one of these bridges
				if( vert[test] != vert[prev[curr]] && vert[test] != vert[curr] && vert[test] != vert[next[curr]] ){
					okay &= !point_in_triangle( normal, xyz[prev[curr]], xyz[curr], xyz[next[curr]], xyz[test] );
				}
				test = next[test];
			}
			if( okay ){
				if( okay ){
					// decrement the number of vertices
					num_verts--;
					
					// remove the clipped vertex from consideration
					best_vert.erase( curr );
					
					// store the next and previous vertices for convenience, it
					// avoids unnecessary nesting like next[prev[curr]] that happens
					// otherwise to avoid using vertex curr which gets removed.
					tmp_prev = prev[curr];
					tmp_next = next[curr];
					
					// add the triangle to the output
					tris.push_back( 3 );
					tris.push_back( vert[tmp_prev] );
					tris.push_back( vert[curr] );
					tris.push_back( vert[tmp_next] );
					
					// update the linked list indices
					next[tmp_prev] = tmp_next;
					prev[tmp_next] = tmp_prev;
					
					// update the convexity status of the next
					// and previous vertices, since it may
					// have changed after clipping
					best_vert.erase( tmp_prev );
					best_vert.erase( tmp_next );
					convexity[tmp_prev] = compute_convexity( normal, xyz[prev[tmp_prev]], xyz[tmp_prev], xyz[next[tmp_prev]] );
					convexity[tmp_next] = compute_convexity( normal, xyz[prev[tmp_next]], xyz[tmp_next], xyz[next[tmp_next]] );
					if( convexity[tmp_prev] > TRIANGULATE_EPSILON ) best_vert.insert( tmp_prev );
					if( convexity[tmp_next] > TRIANGULATE_EPSILON ) best_vert.insert( tmp_next );
					break;
				}				
			}
		}		
	}
	
	//std::cout << "took " << tries << " out of a maximum of " << max_tries << std::endl;
	
	// should always end up with num_verts-2 triangles when
	// tesselating a simple polygon. Failure to do so indicates
	// non-simple input, or input that cannot be resolved with 
	// the current TRIANGULATE_EPSILON setting
	return num_verts == 2;
}

bool triangulate_simple_polygon( const std::vector<double> &coords, const int *facet, std::vector<int> &tris ){
	//return triangulate_simple_polygon_set( coords, facet, tris );
    return triangulate_simple_polygon_naive( coords, facet, tris );
}

