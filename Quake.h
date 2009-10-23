#ifndef __QUAKE_H__
#define __QUAKE_H__

#define MD2_NUMVERTEXNORMALS 162
#define MD3_SCALE (0.015625f)

class Quake
{
public:
	static void convertVector( float vec[3] );
	static void convertQuaternion( float vec[4] );
	
	static const float md2VertexNormals[MD2_NUMVERTEXNORMALS][3];
};

#endif
