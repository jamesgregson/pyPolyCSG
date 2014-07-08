#ifndef POLYHEDRON_UNARY_OP_H
#define POLYHEDRON_UNARY_OP_H

/**
 @file polyhedron_unary_op.h
 @author James Gregson (james.gregson@gmail.com)
 @brief Defines a base class for unary operations on polyhedra that produce polyhedra as output.  A set of affine transformations are then implemented using this base to provide the facility to translate, rotate and scale polyhedra.  
*/

#include"polyhedron.h"

/**
 @brief Base class definining unary operations on polyhedra. Produces
 a polyhedral output for a single polyhedron input.
 */
class polyhedron_unary_op {
public:
	/**
	 @brief The method implementing the operation. Takes an input polyhedron
	 computes an output polyhedron
	 @param[in] in input polyhedron 1
	 @return result polyhedron
	 */
	virtual polyhedron operator()( const polyhedron &in )=0;
};

/**
 @brief Class for translating a polyhedron
*/
class polyhedron_translate : public polyhedron_unary_op {
private:
	/** @brief translation vector */
	double	m_xyz[3];
public:
	/**
	 @brief constructor, takes distances along each axis as arguements
	 @param[in] x distance to move polyhedron along x-axis
	 @param[in] y distance to move polyhedron along y-axis
	 @param[in] z distance to move polyhedron along z-axis
	*/
	polyhedron_translate( const double x, const double y, const double z );
	
	/**
	 @brief applies the translation operation
	 @param[in] in input polhedron
	 @return polyhedron translated by [m_x, m_y, m_z]
	*/
	polyhedron operator()( const polyhedron &in );
};

/**
 @brief Class for rotating a polyhedron
 */
class polyhedron_rotate : public polyhedron_unary_op {
private:
	/** @brief rotation matrix */
	double m_A[3][3];
public:
	/**
	 @brief constructor, takes angles about each axis
	 @param[in] x distance to move polyhedron along x-axis
	 @param[in] y distance to move polyhedron along y-axis
	 @param[in] z distance to move polyhedron along z-axis
	 */
	polyhedron_rotate( const double theta_x, const double theta_y, const double theta_z );
	
	/**
	 @brief applies the translation operation
	 @param[in] in input polhedron
	 @return polyhedron translated by [m_x, m_y, m_z]
	 */
	polyhedron operator()( const polyhedron &in );
};

/**
 @brief Class for translating a polyhedron
 */
class polyhedron_scale : public polyhedron_unary_op {
private:
	/** @brief scaling vector */
	double	m_xyz[3];
public:
	/**
	 @brief constructor, takes distances along each axis as arguements
	 @param[in] x distance to move polyhedron along x-axis
	 @param[in] y distance to move polyhedron along y-axis
	 @param[in] z distance to move polyhedron along z-axis
	 */
	polyhedron_scale( const double x, const double y, const double z );
	
	/**
	 @brief applies the translation operation
	 @param[in] in input polhedron
	 @return polyhedron translated by [m_x, m_y, m_z]
	 */
	polyhedron operator()( const polyhedron &in );
};

class polyhedron_multmatrix3 : public polyhedron_unary_op
{
    private:
        /** @brief matrix to apply */
        double m_matrix[3][3];
    public:
        /**
         @brief constructor, takes a list of matrix elements as arguments
         */
        polyhedron_multmatrix3( const double xx,const double xy,const double xz,
                                const double yx,const double yy,const double yz,
                                const double zx,const double zy,const double zz );

        /**
         @brief applies the matrix multiplication
         @param[in] an input polyhedron
         @return polyhedron transformed by m_matrix
         */
        polyhedron operator()( const polyhedron &in );
};

class polyhedron_multmatrix4 : public polyhedron_unary_op
{
    private:
        /** @brief matrix to apply */
        double m_matrix[4][4];
    public:
        /**
         @brief constructor, takes a list of matrix elements as arguments
         */

        polyhedron_multmatrix4(
            const double xx,const double xy,const double xz, const double xa,
            const double yx,const double yy,const double yz, const double ya,
            const double zx,const double zy,const double zz, const double za,
            const double ax,const double ay,const double az, const double aa );
        /**
         @brief applies the matrix multiplication
         @param[in] an input polyhedron
         @return polyhedron transformed by m_matrix
         */
        polyhedron operator()( const polyhedron &in );
};

#endif
