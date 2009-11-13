/*
-------------------------------------------------------------------------------
Copyright (c) 2009 Nico de Poel

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-------------------------------------------------------------------------------
*/
#ifndef __VECTOR_H__
#define __VECTOR_H__

typedef float vec2_t[2];
typedef float vec3_t[3];

void vec_assign( vec3_t v, float x, float y, float z );
void vec_copy( const vec3_t in, vec3_t out );
void vec_add( const vec3_t a, const vec3_t b, vec3_t out );
void vec_subtract( const vec3_t a, const vec3_t b, vec3_t out );
void vec_scale( const vec3_t v, float f, vec3_t out );
void vec_crossProduct( const vec3_t a, const vec3_t b, vec3_t out );
void vec_normalize( vec3_t v );

#endif