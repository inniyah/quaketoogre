#include "Common.h"
#include "Quake.h"

const vec3_t Quake::md2VertexNormals[MD2_NUMVERTEXNORMALS] =
{
#include "anorms.h"
};

void Quake::convertVector( vec3_t v )
{
	float tmp;
	tmp = v[1];
	v[1] = v[2];
	v[2] = -tmp;
}

void Quake::convertQuaternion( quat4_t q )
{
	static const quat4_t trsf = {0.707107f, 0, 0, -0.707107f};
	
	quat4_t tmp;
	Quat_multQuat( trsf, q, tmp );
	Quat_copy( tmp, q );
}
