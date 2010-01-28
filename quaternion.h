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
 */

#ifndef __QUATERNION_H__
#define __QUATERNION_H__

#include "vector.h"

/**
This class has the same interface as Ogre's Quaternion class, so they should be interchangeable.
*/
class Quaternion
{
public:
	float w, x, y, z;

	Quaternion(): w(1), x(0), y(0), z(0) {}
	Quaternion( float w, float x, float y, float z ): w(w), x(x), y(y), z(z) {}
	Quaternion( const Quaternion &other ): w(other.w), x(other.x), y(other.y), z(other.z) {}

	Quaternion &operator=( const Quaternion &other )
	{
		w = other.w;
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}

	float normalise();
	float Dot( const Quaternion &other ) const;
	void ToAngleAxis( float &angle, Vector3 &axis ) const;

	Quaternion Inverse() const;
	Quaternion UnitInverse() const;

	Quaternion operator*( const Quaternion &other ) const;
	Vector3 operator*( const Vector3 &v ) const;

	static Quaternion Slerp( float t, const Quaternion &qa, const Quaternion &qb );
};

#endif	// __QUATERNION_H__
