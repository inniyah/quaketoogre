#include "Common.h"
#include "Quake.h"

const float Quake::md2VertexNormals[MD2_NUMVERTEXNORMALS][3] =
{
#include "anorms.h"
};

void Quake::convertCoordinate( float vec[3] )
{
	float tmp;
	tmp = vec[1];
	vec[1] = vec[2];
	vec[2] = -tmp;
}
