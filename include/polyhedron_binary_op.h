#ifndef POLYHEDRON_BINARY_OP_H
#define POLYHEDRON_BINARY_OP_H

/**
 @file polyhedron_binary_op.h
 @author James Gregson (james.gregson@gmail.com)
 @brief Defines a base class for binary operations on polyhedra that produce polyhedral output.  This is then used as a base class for polyhedron set operations (union, difference, intersection, symmetric difference) which are implemented using different back-ends in polyhedron_binary_opp.cpp.
*/

#include"polyhedron.h"

/**
 @brief Base class definining binary operations on polyhedra. These take a pair
 of polyhedral inputs and use them to compute a single polyhedral output
*/
class polyhedron_binary_op {
public:
	/**
	 @brief The method implementing the operation. Takes two input polyhedra
	 and computes an output polyhedron
	 @param[in] A input polyhedron 1
	 @param[in] B input polyhedron 2
	 @return result polyhedron
	*/
	virtual polyhedron operator()( const polyhedron &A, const polyhedron &B )=0;
};

class polyhedron_union : public polyhedron_binary_op {
public:
	/**
	 @brief Union of two polyhedra, subclassed from polyhedron_binary_op
	 @param[in] A input polyhedron 1
	 @param[in] B input polyhedron 2
	 @return union of A and B
	*/
	polyhedron operator()( const polyhedron &A, const polyhedron &B );
};

class polyhedron_difference : public polyhedron_binary_op {
public:
	/**
	 @brief Difference (A-B) of two polyhedra, subclassed from polyhedron_binary_op
	 @param[in] A input polyhedron 1
	 @param[in] B input polyhedron 2
	 @return Difference of A and B
	 */
	polyhedron operator()( const polyhedron &A, const polyhedron &B );
};

class polyhedron_symmetric_difference : public polyhedron_binary_op {
public:
	/**
	 @brief Symmetric difference ((A-B)U(B-A)) of two polyhedra, subclassed from polyhedron_binary_op
	 @param[in] A input polyhedron 1
	 @param[in] B input polyhedron 2
	 @return symmetric difference of A and B
	 */
	polyhedron operator()( const polyhedron &A, const polyhedron &B );
};

class polyhedron_intersection : public polyhedron_binary_op {
public:
	/**
	 @brief Intersection of two polyhedra, subclassed from polyhedron_binary_op
	 @param[in] A input polyhedron 1
	 @param[in] B input polyhedron 2
	 @return intersection of A and B
	 */
	polyhedron operator()( const polyhedron &A, const polyhedron &B );
};


#endif