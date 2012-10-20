#include<vector>
#include<iostream>

#include<carve/input.hpp> 
#include<carve/csg.hpp>

#include"polyhedron_binary_op.h"

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
