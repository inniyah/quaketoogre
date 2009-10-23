#include "Common.h"
#include "Quake.h"

#include "quaternion.h"

const float Quake::md2VertexNormals[MD2_NUMVERTEXNORMALS][3] =
{
#include "anorms.h"
};

void Quake::convertVector( float vec[3] )
{
	float tmp;
	tmp = vec[1];
	vec[1] = vec[2];
	vec[2] = -tmp;
}

void Quake::convertQuaternion( quat4_t q )
{
	static const quat4_t trsf = {0.707107f, 0, 0, -0.707107f};
	
	quat4_t tmp;
	Quat_multQuat( trsf, q, tmp );
	Quat_copy( tmp, q );
}
