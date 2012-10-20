#ifndef MESH_FUNCTIONS_H
#define MESH_FUNCTIONS_H

#include<vector>

/**
 @brief determines if the input mesh is a closed manifold
 @param[in] coords input coordinate array, packed [x,y,z,x,y,z,...]
 @param[in] faces  input face vertex indices, packed [ nverts, v0, v1, v2, ..., nverts, v0, v1, ... ]
 @return true if the mesh is a closed manifold, false otherwise
*/
bool mesh_is_closed_manifold( const std::vector<double> &coords, const std::vector<int> &faces );

/**
 @brief estimates the normal of a facet using Newell's method, effectively an average of normals defined by the two adjacent edges at each vertex.
 @param[in] coords input array of vertex coordinates
 @param[in] vertex_ids array of vertex indices, packed [ nverts, v0, v1, ..., vn ]
 @param[out] normal estimated facet normal
 @param[out] optional output plane equation D value, for P = dot( N, p ) + D = 0
 @return true if the normal was successfully estimated, false otherwise
*/
bool mesh_estimate_facet_normal( const std::vector<double> &coords, const int *face_vtx, double *normal, double *D=NULL );

#endif