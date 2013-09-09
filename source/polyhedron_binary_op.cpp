#include<vector>
#include<iterator>
#include<iostream>

#include"polyhedron.h"
#include"polyhedron_binary_op.h"

#if defined(CSG_USE_CGAL) && !defined(CSG_USE_CARVE)
#include <CGAL/Polyhedron_items_with_id_3.h> 
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include<CGAL/Polyhedron_incremental_builder_3.h>
#include<CGAL/Polyhedron_3.h>
#include<CGAL/Nef_polyhedron_3.h>
#include<CGAL/IO/Polyhedron_iostream.h>
#include <CGAL/Iterator_project.h>
#include <CGAL/function_objects.h>
#elif defined(CSG_USE_CARVE)
#if defined(HAVE_CONFIG_H)
#include <carve/config.h>
#endif
#include <carve/interpolator.hpp>
#include <carve/csg_triangulator.hpp>
#include <carve/csg.hpp>
#endif

#if defined(CSG_USE_CGAL) && !defined(CSG_USE_CARVE)
typedef CGAL::Exact_predicates_exact_constructions_kernel     Kernel;
typedef CGAL::Polyhedron_3<Kernel>         Polyhedron;
typedef Polyhedron::HalfedgeDS             HalfedgeDS;
typedef CGAL::Nef_polyhedron_3<Kernel>     Nef_polyhedron;

// A modifier creating a triangle with the incremental builder.
template<class HDS>
class polyhedron_builder : public CGAL::Modifier_base<HDS> {
public:
    polyhedron t;

    polyhedron_builder( const polyhedron &p ){
        t = p;
    }
    void operator()( HDS& hds) {
        typedef typename HDS::Vertex   Vertex;
        typedef typename Vertex::Point Point;
        
        
        // create a cgal incremental builder
        CGAL::Polyhedron_incremental_builder_3<HDS> B( hds, true);
        B.begin_surface( t.num_vertices(), t.num_faces() );
        
        // add the polyhedron vertices
        for( int i=0; i<t.num_vertices(); i++ ){
            double x, y, z;
            t.get_vertex( i, x, y, z );
            B.add_vertex( Point( x, y, z ) );
        }
        
        std::vector<double> coords;
        std::vector<int> faces;
        t.output_store_in_mesh( coords, faces );
        
        int nverts, tmpi = 0;
        while( tmpi < (int)faces.size() ){
            nverts = faces[tmpi++];
            if( nverts != 3 )
                std::cout << "face has " << nverts << " vertices" << std::endl;
            B.begin_facet();
            for( int i=0; i<nverts; i++ ){
                B.add_vertex_to_facet( faces[tmpi++] );
            }
            B.end_facet();
        }
        
        // finish up the surface
        B.end_surface();
    }
};

Nef_polyhedron polyhedron_to_cgal( const polyhedron &p ){
    polyhedron tmp = p.triangulate();
    Polyhedron P;
    polyhedron_builder<HalfedgeDS> builder( tmp );
    P.delegate( builder );
    if( P.is_closed() )
        return Nef_polyhedron( P );
    else
        std::cout << "input polyhedron is not closed!" << std::endl;
    
    return Nef_polyhedron();
}

polyhedron cgal_to_polyhedron( const Nef_polyhedron &NP ){
    Polyhedron P;
    polyhedron ret;
    
    if( NP.is_simple() ){
        NP.convert_to_polyhedron(P);
        std::vector<double> coords;
        std::vector<int> tris;
        int next_id = 0;
        std::map< Polyhedron::Vertex*, int > vid;
        for( Polyhedron::Vertex_iterator iter=P.vertices_begin(); iter!=P.vertices_end(); iter++ ){
            coords.push_back( CGAL::to_double( (*iter).point().x() ) );
            coords.push_back( CGAL::to_double( (*iter).point().y() ) );
            coords.push_back( CGAL::to_double( (*iter).point().z() ) );
            vid[ &(*iter) ] = next_id++;
        }
        
        for( Polyhedron::Facet_iterator iter=P.facets_begin(); iter!=P.facets_end(); iter++ ){
            Polyhedron::Halfedge_around_facet_circulator j = iter->facet_begin();
            tris.push_back( CGAL::circulator_size(j) );
            do {
                tris.push_back( std::distance(P.vertices_begin(), j->vertex()) );
            } while ( ++j != iter->facet_begin());
        }
        
        ret.initialize_load_from_mesh( coords, tris );
    } else {
        std::cout << "resulting polyhedron is not simple!" << std::endl;
    }
    return ret;
}

polyhedron polyhedron_union::operator()( const polyhedron &A, const polyhedron &B ){
    Nef_polyhedron a, b, c;
    try {
        a = polyhedron_to_cgal( A );
        b = polyhedron_to_cgal( B );
        c = (a + b).interior().closure();
        return cgal_to_polyhedron( c );
    } catch( std::exception &e ){
        return A;
    }
}

polyhedron polyhedron_difference::operator()( const polyhedron &A, const polyhedron &B ){
    Nef_polyhedron a, b, c;
    try {
        a = polyhedron_to_cgal( A );
        b = polyhedron_to_cgal( B );
        c = (a - b).interior().closure();
        return cgal_to_polyhedron( c );
    } catch( std::exception &e ){
        return A;
    }
}

polyhedron polyhedron_symmetric_difference::operator()( const polyhedron &A, const polyhedron &B ){
    Nef_polyhedron a, b, c;
    try {
        a = polyhedron_to_cgal( A );
        b = polyhedron_to_cgal( B );
        c = (a ^ b).interior().closure();
        return cgal_to_polyhedron( c );
    } catch( std::exception &e ){
        return A;
    }
}

polyhedron polyhedron_intersection::operator()( const polyhedron &A, const polyhedron &B ){
    Nef_polyhedron a, b, c;
    try {
        a = polyhedron_to_cgal( A );
        b = polyhedron_to_cgal( B );
        c = (a * b).interior().closure();
        return cgal_to_polyhedron( c );
    } catch( std::exception &e ){
        return A;
    }
}


#elif defined(CSG_USE_CARVE)

carve::mesh::MeshSet<3> *polyhedron_to_carve( const polyhedron &p ){
	std::vector<double> coords;
	std::vector<int> faces;
    
	p.output_store_in_mesh( coords, faces );
	
    std::vector<carve::mesh::MeshSet<3>::vertex_t*> v;
    std::vector<carve::mesh::MeshSet<3>::face_t *> f;
    for( int i=0; i<(int)coords.size(); i+=3 ){
        v.push_back( new carve::mesh::MeshSet<3>::vertex_t( carve::geom::VECTOR( coords[i+0], coords[i+1], coords[i+2]) ) );
    }
    int nverts, tmpi=0;
	while( tmpi < (int)faces.size() ){
		nverts = faces[tmpi++];
		std::vector<carve::mesh::MeshSet<3>::vertex_t*> face_verts;
		for( int i=0; i<nverts; i++ ){
			face_verts.push_back( v[faces[tmpi++]] );
		}
        carve::mesh::MeshSet<3>::face_t *tf = new carve::mesh::MeshSet<3>::face_t( face_verts.begin(), face_verts.end() );
		f.push_back( tf );
	}

	return new carve::mesh::MeshSet<3>( f );
}

polyhedron carve_to_polyhedron( carve::mesh::MeshSet<3> *p ){
	std::map< const carve::mesh::MeshSet<3>::vertex_t*, int > vid;
	std::vector<double> coords;
	std::vector<int> faces;
    
    int nextvid = 0;
    for( carve::mesh::MeshSet<3>::face_iter i=p->faceBegin(); i!=p->faceEnd(); ++i ){
        carve::mesh::MeshSet<3>::face_t *f = *i;
        
        std::vector<int> fvid;
        for (carve::mesh::MeshSet<3>::face_t::edge_iter_t e = f->begin(); e != f->end(); ++e) {
            carve::mesh::MeshSet<3>::vertex_t *tv = e->vert;
            if( vid.find(tv) == vid.end() ){
                vid[tv] = nextvid++;
                coords.push_back( tv->v.x );
                coords.push_back( tv->v.y );
                coords.push_back( tv->v.z );
            }
            fvid.push_back( vid[tv] );
        }
        faces.push_back( fvid.size() );
        for( int j=0; j<(int)fvid.size(); j++ ){
            faces.push_back( fvid[j] );
        }
    }
    
	polyhedron poly;
	poly.initialize_load_from_mesh( coords, faces );
	return poly;
}

polyhedron polyhedron_union::operator()( const polyhedron &A, const polyhedron &B ){	
	carve::mesh::MeshSet<3> *pA = polyhedron_to_carve( A );
	carve::mesh::MeshSet<3> *pB = polyhedron_to_carve( B );
	carve::csg::CSG csg;
	carve::mesh::MeshSet<3> *pR = csg.compute( pA, pB, carve::csg::CSG::UNION );
	polyhedron R = carve_to_polyhedron( pR );
	delete pA;
	delete pB;
	delete pR;
	return R;
}

polyhedron polyhedron_difference::operator()( const polyhedron &A, const polyhedron &B ){
	carve::mesh::MeshSet<3> *pA = polyhedron_to_carve( A );
	carve::mesh::MeshSet<3> *pB = polyhedron_to_carve( B );
	carve::csg::CSG csg;
	carve::mesh::MeshSet<3> *pR = csg.compute( pA, pB, carve::csg::CSG::A_MINUS_B );
	polyhedron R = carve_to_polyhedron( pR );
	delete pA;
	delete pB;
	delete pR;
	return R;
}

polyhedron polyhedron_symmetric_difference::operator()( const polyhedron &A, const polyhedron &B ){
	carve::mesh::MeshSet<3> *pA = polyhedron_to_carve( A );
	carve::mesh::MeshSet<3> *pB = polyhedron_to_carve( B );
	carve::csg::CSG csg;
	carve::mesh::MeshSet<3> *pR = csg.compute( pA, pB, carve::csg::CSG::SYMMETRIC_DIFFERENCE );
	polyhedron R = carve_to_polyhedron( pR );
	delete pA;
	delete pB;
	delete pR;
	return R;
}

polyhedron polyhedron_intersection::operator()( const polyhedron &A, const polyhedron &B ){
	carve::mesh::MeshSet<3> *pA = polyhedron_to_carve( A );
	carve::mesh::MeshSet<3> *pB = polyhedron_to_carve( B );
	carve::csg::CSG csg;
	carve::mesh::MeshSet<3> *pR = csg.compute( pA, pB, carve::csg::CSG::INTERSECTION );
	polyhedron R = carve_to_polyhedron( pR );
	delete pA;
	delete pB;
	delete pR;
	return R;	
}
#endif


