#ifndef __QUAKE_H__
#define __QUAKE_H__

#define MD2_NUMVERTEXNORMALS 162
#define MD3_SCALE (0.015625f)

class Quake
{
public:
	static void convertCoordinate( float vec[3] );	
	
	static const float md2VertexNormals[MD2_NUMVERTEXNORMALS][3];
};

#endif
