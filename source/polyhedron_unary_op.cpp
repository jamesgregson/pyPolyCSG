#include<cmath>
#include<vector>
#include"polyhedron_unary_op.h"

polyhedron_translate::polyhedron_translate( const double x, const double y, const double z ){
	m_xyz[0]=x; m_xyz[1]=y; m_xyz[2]=z;
}

polyhedron polyhedron_translate::operator()( const polyhedron &in ){
	std::vector<double> coords;
	std::vector<int>    faces;
	in.output_store_in_mesh( coords, faces );
	for( int i=0; i<(int)coords.size(); i+=3 ){
		coords[i+0] += m_xyz[0];
		coords[i+1] += m_xyz[1];
		coords[i+2] += m_xyz[2];
	}
	polyhedron ret;
	ret.initialize_load_from_mesh( coords, faces );
	return ret;
}

void unary_op_matrix_mult( const double A[3][3], const double B[3][3], double C[3][3] ){
	for( int i=0; i<3; i++ ){
		for( int j=0; j<3; j++ ){
			C[i][j] = A[i][0]*B[0][j] + A[i][1]*B[1][j] + A[i][2]*B[2][j];
		}
	}
}

polyhedron_rotate::polyhedron_rotate( double theta_x, double theta_y, double theta_z ){
	double Rx[3][3], Ry[3][3], Rz[3][3], c, s;
	c = cos( theta_x*M_PI/180.0 );
	s = sin( theta_x*M_PI/180.0 );
	Rx[0][0]=1.0; Rx[0][1]=0.0; Rx[0][2]=0.0;
	Rx[1][0]=0.0; Rx[1][1]=c;   Rx[1][2]=-s;
	Rx[2][0]=0.0; Rx[2][1]=s;   Rx[2][2]=c;
	
	c = cos( theta_y*M_PI/180.0 );
	s = sin( theta_y*M_PI/180.0 );
	Ry[0][0]=c;   Ry[0][1]=0.0; Ry[0][2]=s;
	Ry[1][0]=0.0; Ry[1][1]=1.0; Ry[1][2]=0.0;
	Ry[2][0]=-s;  Ry[2][1]=0.0; Ry[2][2]=c;
	
	c = cos( theta_z*M_PI/180.0 );
	s = sin( theta_z*M_PI/180.0 );
	Rz[0][0]=c;   Rz[0][1]=-s;  Rz[0][2]=0.0;
	Rz[1][0]=s;   Rz[1][1]=c;   Rz[1][2]=0.0;
	Rz[2][0]=0.0; Rz[2][1]=0.0; Rz[2][2]=1.0;
	
	unary_op_matrix_mult( Rx, Ry, m_A );
	unary_op_matrix_mult( m_A, Rz, Rx );
	for( int i=0; i<3; i++ ){
		for( int j=0; j<3; j++ ){
			m_A[i][j]=Rx[i][j];
		}
	}
}

polyhedron polyhedron_rotate::operator()( const polyhedron &in ){
	std::vector<double> coords;
	std::vector<int>    faces;
	in.output_store_in_mesh( coords, faces );
	for( int i=0; i<(int)coords.size(); i+=3 ){
		double x=coords[i+0], y=coords[i+1], z=coords[i+2];
		coords[i+0] = m_A[0][0]*x + m_A[0][1]*y + m_A[0][2]*z;
		coords[i+1] = m_A[1][0]*x + m_A[1][1]*y + m_A[1][2]*z;
		coords[i+2] = m_A[2][0]*x + m_A[2][1]*y + m_A[2][2]*z;
	}
	polyhedron ret;
	ret.initialize_load_from_mesh( coords, faces );
	return ret;
}

polyhedron_scale::polyhedron_scale( const double x, const double y, const double z ){
	m_xyz[0]=x; m_xyz[1]=y; m_xyz[2]=z;
}

polyhedron polyhedron_scale::operator()( const polyhedron &in ){
	std::vector<double> coords;
	std::vector<int>    faces;
	in.output_store_in_mesh( coords, faces );
	for( int i=0; i<(int)coords.size(); i+=3 ){
		coords[i+0] *= m_xyz[0];
		coords[i+1] *= m_xyz[1];
		coords[i+2] *= m_xyz[2];
	}
	polyhedron ret;
	ret.initialize_load_from_mesh( coords, faces );
	return ret;
}

