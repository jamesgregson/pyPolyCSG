#include<vector>
#include<iostream>

#include"polyhedron.h"
#include"polyhedron_binary_op.h"

#if defined(CSG_USE_CGAL)
#include<CGAL/Simple_cartesian.h>
#include<CGAL/Polyhedron_incremental_builder_3.h>
#include<CGAL/Polyhedron_3.h>
#include<CGAL/Nef_polyhedron_3.h>
#include<CGAL/IO/Polyhedron_iostream.h>
#elif defined(CSG_USE_CARVE)
#include<carve/input.hpp>
#include<carve/csg.hpp>
#endif

#if defined(CSG_USE_CGAL)
typedef CGAL::Simple_cartesian<double>     Kernel;
typedef CGAL::Polyhedron_3<Kernel>         Polyhedron;
typedef Polyhedron::HalfedgeDS             HalfedgeDS;
typedef CGAL::Nef_polyhedron_3<Kernel>     Nef_polyhedron;

// A modifier creating a triangle with the incremental builder.
template<class HDS>
class polyhedron_builder : public CGAL::Modifier_base<HDS> {
public:
    polyhedron t;

    polyhedron_builder( const polyhedron &p ){
        t = p.triangulate();
    }
    void operator()( HDS& hds) {
        typedef typename HDS::Vertex   Vertex;
        typedef typename Vertex::Point Point;
        
        
        // create a cgal incremental builder
        CGAL::Polyhedron_incremental_builder_3<HDS> B( hds, true);
        B.begin_surface( t.num_vertices(), t.num_faces() );
        
        /*
        // add the polyhedron vertices
        for( int i=0; i<t.num_vertices(); i++ ){
            double x, y, z;
            t.get_vertex( i, x, y, z );
            B.add_vertex( Point( x, y, z ) );
        }
        
        // add the polyhedron triangles
        for( int i=0; i<t.num_faces(); i++ ){
            int idx[3];
            if( t.num_face_vertices(i) != 3 ){
                throw "should be triangulated";
            }
            t.get_face_vertices( i, idx );
            B.begin_facet();
            B.add_vertex_to_facet( idx[0] );
            B.add_vertex_to_facet( idx[1] );
            B.add_vertex_to_facet( idx[2] );
            B.end_facet();
        }
        */
        
        // finish up the surface
        B.end_surface();
    }
};

polyhedron polyhedron_union::operator()( const polyhedron &A, const polyhedron &B ){
    Polyhedron pA, pB, pC;
    polyhedron_builder<HalfedgeDS> builderA( A );
    polyhedron_builder<HalfedgeDS> builderB( B );

    pA.delegate( builderA );
    pB.delegate( builderB );
    
    Nef_polyhedron nA(pA), nB(pB);
    Nef_polyhedron nC( (nA+nB).regularization() );
    
    nC.convert_to_polyhedron(pC);
    
    std::vector<double> coords;
    std::vector<int> tris;
    
    for( Polyhedron::Vertex_iterator iter=pC.vertices_begin(); iter!=pC.vertices_end(); iter++ ){
        coords.push_back( (*iter).point().x() );
        coords.push_back( (*iter).point().y() );
        coords.push_back( (*iter).point().z() );
    }
    
    polyhedron ret;
    ret.initialize_load_from_mesh( coords, tris );
    return ret;
}

polyhedron polyhedron_difference::operator()( const polyhedron &A, const polyhedron &B ){
    return A;
}

polyhedron polyhedron_symmetric_difference::operator()( const polyhedron &A, const polyhedron &B ){
    return A;
}

polyhedron polyhedron_intersection::operator()( const polyhedron &A, const polyhedron &B ){
	return A;
}


#endif

#if defined(CSG_USE_CARVE)
carve::poly::Polyhedron *polyhedron_to_carve( const polyhedron &p ){
	std::vector<double> coords;
	std::vector<int> faces;
	p.output_store_in_mesh( coords, faces );
	
	std::vector< carve::poly::Polyhedron::vertex_t > cverts;
	std::vector< carve::poly::Polyhedron::face_t > cfaces;
	
	carve::input::PolyhedronData data;
	for( int i=0; i<(int)coords.size(); i+=3 ){
		cverts.push_back( carve::poly::Polyhedron::vertex_t( carve::geom::VECTOR( coords[i], coords[i+1], coords[i+2] ) ) );
	}
	int nverts, tmpi = 0;
	while( tmpi < (int)faces.size() ){
		nverts = faces[tmpi++];
		std::vector< const carve::poly::Polyhedron::vertex_t* > face_verts;
		for( int i=0; i<nverts; i++ ){
			face_verts.push_back( &cverts[faces[tmpi++]] );
		}
		cfaces.push_back( carve::poly::Polyhedron::face_t( face_verts ) );
	}
	return new carve::poly::Polyhedron( cfaces, cverts );
}

/*
 // old implementation, only worked for tris and quads
carve::poly::Polyhedron *polyhedron_to_carve( const polyhedron &p ){
	std::vector<double> coords;
	std::vector<int> faces;
	p.output_store_in_mesh( coords, faces );
	
	carve::input::PolyhedronData data;
	for( int i=0; i<(int)coords.size(); i+=3 ){
		data.addVertex( carve::geom::VECTOR( coords[i], coords[i+1], coords[i+2] ) );
	}
	int nverts, tmpi = 0;
	while( tmpi < (int)faces.size() ){
		nverts = faces[tmpi++];
		if( nverts == 3 ){
			data.addFace( faces[tmpi], faces[tmpi+1], faces[tmpi+2] );
			tmpi += 3;
		} else if( nverts == 4 ){
			data.addFace( faces[tmpi], faces[tmpi+1], faces[tmpi+2], faces[tmpi+3] );
			tmpi += 4;			
		} else {
			std::cout << "error, face is neither quad nor triangle" << std::endl;
			tmpi += nverts;
		}
	}
	return data.create();
}
*/

polyhedron carve_to_polyhedron( carve::poly::Polyhedron *p ){
	std::map< const carve::poly::Polyhedron::vertex_t*, int > vid;
	std::vector<double> coords;
	std::vector<int> faces;
	for( int i=0; i<(int)p->vertices.size(); i++ ){
		vid[&p->vertices[i]] = i;
		coords.push_back( p->vertices[i].v[0] );
		coords.push_back( p->vertices[i].v[1] );
		coords.push_back( p->vertices[i].v[2] );
	}
	for( int i=0; i<(int)p->faces.size(); i++ ){
		faces.push_back( p->faces[i].nVertices() );
		for( int j=0; j<(int)p->faces[i].nVertices(); j++ ){
			const carve::poly::Vertex<3u>* tv = p->faces[i].vertex(j);
			faces.push_back( vid[ tv ] );
		}
	}
	polyhedron poly;
	poly.initialize_load_from_mesh( coords, faces );
	return poly;
}

polyhedron polyhedron_union::operator()( const polyhedron &A, const polyhedron &B ){	
	carve::poly::Polyhedron *pA = polyhedron_to_carve( A );
	carve::poly::Polyhedron *pB = polyhedron_to_carve( B );
	carve::csg::CSG csg;
	carve::poly::Polyhedron *pR = csg.compute( pA, pB, carve::csg::CSG::UNION );
	polyhedron R = carve_to_polyhedron( pR );
	delete pA;
	delete pB;
	delete pR;
	return R;
}

polyhedron polyhedron_difference::operator()( const polyhedron &A, const polyhedron &B ){
	carve::poly::Polyhedron *pA = polyhedron_to_carve( A );
	carve::poly::Polyhedron *pB = polyhedron_to_carve( B );
	carve::csg::CSG csg;
	carve::poly::Polyhedron *pR = csg.compute( pA, pB, carve::csg::CSG::A_MINUS_B );
	polyhedron R = carve_to_polyhedron( pR );
	delete pA;
	delete pB;
	delete pR;
	return R;
}

polyhedron polyhedron_symmetric_difference::operator()( const polyhedron &A, const polyhedron &B ){
	carve::poly::Polyhedron *pA = polyhedron_to_carve( A );
	carve::poly::Polyhedron *pB = polyhedron_to_carve( B );
	carve::csg::CSG csg;
	carve::poly::Polyhedron *pR = csg.compute( pA, pB, carve::csg::CSG::SYMMETRIC_DIFFERENCE );
	polyhedron R = carve_to_polyhedron( pR );
	delete pA;
	delete pB;
	delete pR;
	return R;
}

polyhedron polyhedron_intersection::operator()( const polyhedron &A, const polyhedron &B ){
	carve::poly::Polyhedron *pA = polyhedron_to_carve( A );
	carve::poly::Polyhedron *pB = polyhedron_to_carve( B );
	carve::csg::CSG csg;
	carve::poly::Polyhedron *pR = csg.compute( pA, pB, carve::csg::CSG::INTERSECTION );
	polyhedron R = carve_to_polyhedron( pR );
	delete pA;
	delete pB;
	delete pR;
	return R;	
}
#endif


