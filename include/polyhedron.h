#ifndef POLYHEDRON_H
#define POLYHEDRON_H

#include<iostream>
#include<stdexcept>
#include<boost/python.hpp>

/**
 @file   polyhedron.h
 @author James Gregson (james.gregson@gmail.com)
 @brief Definition of the polyhedron class which forms the basis for the CSG library.  Includes methods for constructing polyhedra, including loading from meshes and building mesh-based representations of various primitives.
*/

#include<vector>

/**
 @brief polyhedron class, the workhorse for the library
*/
class polyhedron {
private:
	std::vector<double>		m_coords;
	std::vector<int>		m_faces;
    std::vector<int>        m_faces_start;
public:
	/**
	 @brief default constructor
	*/
	polyhedron();
	
	/** 
	 @brief copy constructor, performs a deep copy of the data
	*/
	polyhedron( const polyhedron &in );
	
    /**
     @brief returns the number of vertices in the mesh
    */
    int num_vertices();
    
    /**
     @brief returns the id'th vertex
    */
    void get_vertex( const int id, double &x, double &y, double &z ) const {
        x = m_coords[id*3+0];
        y = m_coords[id*3+1];
        z = m_coords[id*3+2];
    }
    
    /**
     @brief returns the number of faces in the mesh
    */
    int num_faces();
    
    /**
     @brief returns the number of vertices in the face_id'th face.
     @param[in] face_id id of the face to return the vertex count of
     @return number of vertices in the face_id'th face
    */
    int num_face_vertices( int face_id );
    
    /**
     @brief return the vertex id's corresponding to the face_id'th face
     @param[in] face_id id of the face to get the vertex list of
     @param[out] vertex_id_list array of elements to store the face vertices in, this should be appropriately sized
    */
    void get_face_vertices( int face_id, int *vertex_id_list );
    
    /**
     @brief returns a tuple containing the vertex_id'th vertex's coordinates
     @param[in] vertex_id input vertex id
     @return tuple containing the vertex_id'th vertex coordinates
    */
    boost::python::tuple py_get_vertex_coordinates( int vertex_id );
    
    /**
     @brief returns a list containing the face_id'th vertex indices
     @param[in] face_id the id of the face to get the vertex indices of
     @return list of vertex indices for the face_id'th face
    */
    boost::python::list py_get_face_vertices( int face_id );
    
    /**
     @brief returns a numpy array of mesh vertex coordinates as a 2D numpy array
     @return numpy array of vertex coordinates
    */
    boost::python::numeric::array py_get_vertices();
    
    /**
     @brief temporarily triangulates the current polyhedron and fills a 2D numpy array with the triangle vertex indices
     @return numpy array of triangle vertex indices
    */
    boost::python::numeric::array py_get_triangles();
    
	/**
	 @brief initialize the polyhedron data from an input file, there must be a reader present for the file
	 @param[in] filename input file to load
	 @return true on success, false otherwise
	*/
	bool initialize_load_from_file( const char *filename );
	
	/**
	 @brief initializes the polyhedron data from a mesh, specified as packed point coordinates and face vertex indices.  The other initialize_ methods use this to perform loading and input checking, simply building a properly formatted mesh for this method to operate on.
	 @param[in] coords input array of coordinates, packed as [x,y,z,x,y,z,....,x,y,z]
	 @param[in] faces input array of vertex indices (indexed from zero), packed as [nverts_A,A0,A1,A2,...,nverts_B,B0,B1,B2,B3,...]
	 @return
	*/
	bool initialize_load_from_mesh( const std::vector<double> &coords, const std::vector<int> &faces );
	
	/**
	 @brief generates a sphere with a given radius
	 @param[in] radius the radius of the sphere to create
	 @param[in] is_centered set to true if the sphere should be centered (at [0,0,0]) otherwise centered at [radius, radius, radius]
	 @param[in] hsegments number of segments around the equator
	 @param[in] vsegments number of segments pole-to-pole
	 @return true on success false otherwise
	*/
	bool initialize_create_sphere( const double radius, const bool is_centered=false, const int hsegments=20, const int vsegments=20 );
	
	/**
	 @brief generates an axis-aligned box with given dimensions
	 @param[in] size_x box side-length in x-direction
	 @param[in] size_y box side-length in y-direction
	 @param[in] size_z box side-length in z-direction
	 @param[in] is_centered true if the box should be centered on origin, center at [size_x/2, size_y/2, size_z/2] otherwise
	 @return true on success, false otherwise
	*/
	bool initialize_create_box( const double size_x, const double size_y, const double size_z, const bool is_centered=false ); 
	
	/**
	 @brief generates a cylinder
	 @param[in] radius cylinder radius
	 @param[in] height cylinder height
	 @param[in] is_centered set to true if cylinder is to be centered on origin
	 @param[in] segments number of divisions in cylinder
	*/
	bool initialize_create_cylinder( const double radius, const double height, const bool is_centered=false, const int segments=20 );
	
	/**
	 @brief generates a cone with given dimensions
	 @param[in] radius radius of the base of the cone
	 @param[in] height height of the cone
	 @param[in] is_centered true if the cone should be centered on origin (TODO: What does this mean for a cone?)
	 @param[in] segments number of segments for the cone
	 @return true on success, false otherwise
	*/
	bool initialize_create_cone( const double radius, const double height, const bool is_centered=false, const int semgnets=20 );
	
	/**
	 @brief generates a torus with given dimensions
	 @param[in] radius_major major radius of the torus
	 @param[in] radius_minor minor radius of the torus
	 @param[in] is_centered true if the torus should be centerd at the origin
	 @param[in] major_segments number of segments around the major circumference
	 @param[in] minor_segments number of segments around the minor circumference
	 @return true if successful, false otherwise
	*/
	bool initialize_create_torus( const double radius_major, const double radius_minor, const bool is_centered=false, const int major_segments=20, const int minor_segments=20 );
	
	// TODO: implement number of segments and twist for extrusion
	/**
	 @brief extrudes a profile, (defined in the x-y plane), in the z direction
	 @param[in] coords coordinates of the profile vertices, packed as [x,y,x,y,...,x,y]
	 @param[in] lines packed array of point indices making up the profile line-segments, [A,B,...]
	 @param[in] distance distance to extrude the profile
	 @return true on success, false otherwise
	*/
	bool initialize_create_extrusion( const std::vector<double> &coords, const std::vector<int> &lines, const double distance );
	
	/**
	 @brief generates a surface of revolution (SOR )from the input 2D profile by revolving a contour around the x-axis
	 @param[in] coords coordinates of the profile vertices, as in initialize_create_extrude()
	 @param[in] lines coordinate of the profile lines, as in initialize_create_extrude()
	 @param[in] angle angle by which to rotate, to generate partial SORs
	 @param[in] segments number of segments to create
	 @return true if successful, false otherwise
	*/
	bool initialize_create_surface_of_revolution( const std::vector<double> &coords, const std::vector<int> &lines, const double angle=360.0, const int segments=20 );	
	
	/**
	 @brief stores the polyhedron in the output mesh, formatted the same as initialize_load_from_mesh()
	 @param[out] coords output coordinate array, see polyhedron::initialize_load_from_mesh()
	 @param[out] faces  output face index array, see polyhedron::initialize_load_from_mesh()
	 @return true on success, false otherwise
	*/
	bool output_store_in_mesh( std::vector<double> &coords, std::vector<int> &faces ) const;
	
	/**
	 @brief writes the polyhedron to a file.  The output type is automaticallydetermined by the file extension, for which there must be a writer
	 @param[in] filename output filename where the mesh should be written
	 @return true on success, false otherwise
	*/
	bool output_store_in_file( const char *filename ) const;
	
	/**
	 @brief triangulates all facets of the polyhedron
	 @return triangulated polyhedron
	*/
	polyhedron triangulate() const;
	
	/**
	 @brief translate the polyhedron
	 @param[in] x translation along x-axis
	 @param[in] y translation along y-axis
	 @param[in] z translation along z-axis
	 @return translated polyhedron
	*/
	polyhedron translate( const double x, const double y, const double z ) const;

	/**
	 @brief rotate the polyhedron
	 @param[in] theta_x rotation-angle about x-axis
	 @param[in] theta_y rotation-angle about y-axis
	 @param[in] theta_z rotation-angle about z-axis
	 @return scaled polyhedron
	 */
	polyhedron rotate( const double theta_x, const double theta_y, const double theta_z ) const;
	
	/**
	 @brief scale the polyhedron
	 @param[in] x scale-factor along x-axis
	 @param[in] y scale-factor along y-axis
	 @param[in] z scale-factor along z-axis
	 @return scaled polyhedron
	 */
	polyhedron scale( const double x, const double y, const double z ) const;
			

    /**
     @brief apply a 3x3 rotation matrix to the polyhedron
     */
    polyhedron mult_matrix_3( double xx,double xy,double xz,
                           double yx,double yy,double yz,
                           double zx,double zy,double zz ) const;
    
    /**
     @brief python version of mult_matrix_3 to get around boost::python argument count limits
    */
    polyhedron py_mult_matrix_3( const boost::python::list &m ) const;
    
    /**
     @brief apply a 4x4 transformation matrix to the polyhedron
     */
    polyhedron mult_matrix_4( double xx,double xy,double xz, double xa,
                           double yx,double yy,double yz, double ya,
                           double zx,double zy,double zz, double za,
                           double ax,double ay,double az, double aa ) const;

    /**
     @brief python version of mult_matrix_4 to get around boost::pythoon argument count limits
    */
    polyhedron py_mult_matrix_4( const boost::python::list &m ) const;
    
	/**
	 @brief polyhedron union operator, takes an input polyhedron and returns the union of this polyhedron with it
	 @param[in] in input polyhedron to compute union with
	 @return union of current polyhedron with in
	*/
	polyhedron operator+( const polyhedron &in ) const;
	
	/**
	 @brief polyhedron difference operator, takes an input polyhedron and subtracts it from the current polyhedron
	 @param[in] in input polyhedron to subtract from current polyhedron
	 @return polyhedron in subtracted from current polyhedron
	*/
	polyhedron operator-( const polyhedron &in ) const; 
	
	/**
	 @brief polyhedron symmetric different operator, returns polhedron of space that is in current or input polyhedron but not both
	 @parm[in] in input polyhedron to compute symmetric difference with
	 @return symmetric difference of current and input polyhedron
	*/
	polyhedron operator^( const polyhedron &in ) const;
	
	/**
	 @brief polyhedron intersection operator, returns polyhedron of space that is in both current and input polyhedon
	 @param[in] in input polyhedron to compute intersection with
	 @return intersection of current and input polyhedron
	*/
	polyhedron operator*( const polyhedron &in ) const;
};

/**
 @brief convenience function for loading a polyhedron from a mesh without first constructing an object
 @param[in] filename input mesh filename
 @return loaded polyhedron (if successful) or an empty polyhedron
*/
polyhedron load_mesh_file( const char *filename );

/**
 @brief convenience function for creating a sphere, basically wraps polyhedron::initialize_create_sphere()
 @param[in] radius sphere radius
 @param[in] is_centered set to true if the sphere should be centered at the origin, false otherwise
 @param[in] hsegments number of segments around the equator
 @param[in] vsegments number of segments pole-to-pole
 @return generated polyhedron if successful or an empty polyhedron otherwise
*/
polyhedron sphere( const double radius, const bool is_centered=false, const int hsegments=20, const int vsegments=20 );

/**
 @brief convenience function for creating a box, wraps polyhedron::initialize_create_box()
 @param[in] size_x size along the x-axis
 @param[in] size_y size along the y-axis
 @param[in] size_z size along the z-axis
 @param[in] is_centered set this to true if the box should be centered
 @return generated polyhedron if successful, or an empty polyhedron otherwise
*/
polyhedron box( const double size_x, const double size_y, const double size_z, const bool is_centered=false );

/**
 @brief convenience function for creating a cylinder, wraps polyhedron::initialize_create_cylinder()
 @param[in] radius cylinder radius
 @param[in] height cylinder height
 @param[in] is_centered set this to true if the cylinder should be centered on the origin
 @param[in] segments number of segments
 @return generated polyhedron if successful, or an empty polyhedron otherwise
*/
polyhedron cylinder( const double radius, const double height, const bool is_centered=false, const int segments=20 );

/**
 @brief convenience function for creating a cone, wraps polyhedron::initialize_create_cone()
 @param[in] radius radius of the base of the cone
 @param[in] height height of the cone
 @param[in] is_centered set this to true if the cone should be centered on the origin
 @param[in] segments number of segments to discretize the cone
 @return generated polyhedron if successful, or an empty polyhedron otherwise
*/
polyhedron cone( const double radius, const double height, const bool is_centered=false, const int segments=20 );

/**
 @brief convenience function for creating a torus, wraps polyhedron::initialize_create_torus()
 @param[in] radius_major the major (larger) radius of the torus
 @param[in] radius_minor the minor (smaller) radius of the torus
 @param[in] is_centered set this to true if the torus should be centered on the origin
 @param[in] major_segments number of segments to discretize the major radius
 @param[in] minor_segments number of segments to discretize the minor radius
 @return generated polyhedron or an empty polyhedron otherwise
*/
polyhedron torus( const double radius_major, const double radius_minor, const bool is_centered=false, const int major_segments=20, const int minor_segments=20 );

/**
 @brief generates an extrusion from the input 2D profile
 @param[in] coords coordinates of the profile vertices, as in initialize_create_extrude()
 @param[in] lines coordinate of the profile lines, as in initialize_create_extrude()
 @param[in] distance the distance to extrude the contour
 @return true if successful, false otherwise
 */
polyhedron extrusion( const std::vector<double> &coords, const std::vector<int> &lines, const double distance );

/**
 @brief generates a surface of revolution (SOR )from the input 2D profile by revolving a contour around the x-axis
 @param[in] coords coordinates of the profile vertices, as in initialize_create_extrude()
 @param[in] lines coordinate of the profile lines, as in initialize_create_extrude()
 @param[in] angle angle by which to rotate, to generate partial SORs
 @param[in] segments number of segments to create
 @return true if successful, false otherwise
 */
polyhedron surface_of_revolution( const std::vector<double> &coords, const std::vector<int> &lines, const double angle=360.0, const int segments=20 );

#endif
