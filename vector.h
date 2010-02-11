/*
-------------------------------------------------------------------------------
Copyright (c) 2009-2010 Nico de Poel

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

#include <cstddef>

/**
This class has the same interface as Ogre's Vector3 class, so they should be interchangeable.
*/
class Vector3
{
public:
	float x, y, z;

	Vector3(): x(0), y(0), z(0) {}
	Vector3( const Vector3 &other ): x(other.x), y(other.y), z(other.z) {}
	Vector3( float x, float y, float z ): x(x), y(y), z(z) {}

	Vector3 crossProduct( const Vector3 &other ) const
	{
		Vector3 out;
		out.x = y * other.z - other.y * z;
		out.y = z * other.x - other.z * x;
		out.z = x * other.y - other.x * y;
		return out;
	}

	float normalise();

	float operator[]( const size_t i ) const
	{
		return *(&x + i);
	}

	float &operator[]( const size_t i )
	{
		return *(&x + i);
	}

	Vector3 &operator=( const Vector3 &other )
	{
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}

	Vector3 operator+( const Vector3 &other ) const
	{
		return Vector3( x + other.x, y + other.y, z + other.z );
	}

	Vector3 operator-( const Vector3 &other ) const
	{
		return Vector3( x - other.x, y - other.y, z - other.z );
	}

	Vector3 operator-() const
	{
		return Vector3( -x, -y, -z );
	}

	Vector3 operator*( float scale ) const
	{
		return Vector3( x * scale, y * scale, z * scale );
	}

	Vector3 &operator+=( const Vector3 &other )
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	Vector3 &operator-=( const Vector3 &other )
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	Vector3 &operator*=( float scale )
	{
		x *= scale;
		y *= scale;
		z *= scale;
		return *this;
	}
};

#endif

