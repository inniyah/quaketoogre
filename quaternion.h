/*
 * md5model.h -- md5mesh model loader + animation
 * last modification: aug. 14, 2007
 *
 * Copyright (c) 2005-2007 David HENRY
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * gcc -Wall -ansi -lGL -lGLU -lglut md5anim.c md5anim.c -o md5model
 */

#ifndef __QUATERNION_H__
#define __QUATERNION_H__

#include "vector.h"

/* Quaternion (x, y, z, w) */
typedef float quat4_t[4];

enum {
  X, Y, Z, W
//	W, X, Y, Z
};

/**
 * Quaternion prototypes
 */
void Quat_computeW (quat4_t q);	// TODO implement outside Quaternion class
void Quat_normalize (quat4_t q);
void Quat_multQuat (const quat4_t qa, const quat4_t qb, quat4_t out);
void Quat_multVec (const quat4_t q, const Vector3 &v, quat4_t out);
void Quat_rotatePoint (const quat4_t q, const Vector3 &in, Vector3 &out);
float Quat_dotProduct (const quat4_t qa, const quat4_t qb);
void Quat_slerp (const quat4_t qa, const quat4_t qb, float t, quat4_t out);
void Quat_copy(const quat4_t q, quat4_t out);
void Quat_conjugate(const quat4_t q, quat4_t out);
void Quat_inverse(const quat4_t q, quat4_t out);
void Quat_toAngleAxis(const quat4_t q, float *angle, Vector3 &axis);

class Quaternion
{
public:
	float w, x, y, z;

	Quaternion(): w(1), x(0), y(0), z(0) {}
	Quaternion( float w, float x, float y, float z ): w(w), x(x), y(y), z(z) {}
	Quaternion( const Quaternion &other ): w(other.w), x(other.x), y(other.y), z(other.z) {}

	
};

#endif	// __QUATERNION_H__
