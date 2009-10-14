#ifndef __VECTOR_H__
#define __VECTOR_H__

typedef float vec2_t[2];
typedef float vec3_t[3];

void vec_copy( const vec3_t in, vec3_t out );
void vec_add( const vec3_t a, const vec3_t b, vec3_t out );
void vec_subtract( const vec3_t a, const vec3_t b, vec3_t out );
void vec_crossProduct( const vec3_t a, const vec3_t b, vec3_t out );
void vec_normalize( vec3_t v );

#endif