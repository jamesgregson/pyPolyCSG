#ifndef TRIANGULATE_H
#define TRIANGULATE_H

/**
 @file triangulate.h
 @author James Gregson (james.gregson@gmail.com)
 @brief API for triangulation of polygons, used by the polyhedron library to triangulate non-triangular faces for output to STL format, for rendering and to generate polyhedral inputs for the CGAL CSG back-end when completed.  Currently in development, and not used by the code
*/

#include<vector>

/**
 @brief triangulates a simple polygon with no holes or self-intersections by ear-clipping
 @param[in]  coords  input array of coordinates, packed [x,y,x,y,x,y,...]
 @param[in]  contour ordered list of vertices making up the contour
 @param[out] tris    output list of triangle vertex indices, packed [a0, a1, a2, b0, b1, b2, ... ]
 @return true on success, false on failure
*/
bool triangulate_simple_polygon( const std::vector<double> &coords, const int *contour, std::vector<int> &tris );

#endif