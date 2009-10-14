#include "vector.h"
#include <math.h>

void vec_copy( const vec3_t in, vec3_t out )
{
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
}

void vec_add( const vec3_t a, const vec3_t b, vec3_t out )
{
	out[0] = a[0] + b[0];
	out[1] = a[1] + b[1];
	out[2] = a[2] + b[2];
}

void vec_subtract( const vec3_t a, const vec3_t b, vec3_t out )
{
	out[0] = a[0] - b[0];
	out[1] = a[1] - b[1];
	out[2] = a[2] - b[2];
}

void vec_scale( const vec3_t v, float f, vec3_t out )
{
	out[0] = v[0] * f;
	out[1] = v[1] * f;
	out[2] = v[2] * f;
}

void vec_crossProduct( const vec3_t a, const vec3_t b, vec3_t out )
{
	out[0] = a[1] * b[2] - b[1] * a[2];
	out[1] = a[2] * b[0] - b[2] * a[0];
	out[2] = a[0] * b[1] - b[0] * a[1];
}

void vec_normalize( vec3_t v )
{
	float len = (float)sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
	v[0] /= len;
	v[1] /= len;
	v[2] /= len;
}