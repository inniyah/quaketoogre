#ifndef __QUAKE_H__
#define __QUAKE_H__

#include "vector.h"
#include "quaternion.h"

#define MD2_NUMVERTEXNORMALS 162
#define MD3_SCALE (0.015625f)

class Quake
{
public:
	static void convertVector( vec3_t v );
	static void convertQuaternion( quat4_t q );
	
	static const float md2VertexNormals[MD2_NUMVERTEXNORMALS][3];
};

#endif
