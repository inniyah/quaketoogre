/*
 * md5mesh.c -- md5mesh model loader + animation
 * last modification: aug. 14, 2007
 *
 * Doom3's md5mesh viewer with animation.  Mesh portion.
 * Dependences: md5model.h, md5anim.c.
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

#include "quaternion.h"
#include <cmath>
#include <cassert>

/**
 * Basic quaternion operations.
 */

float Quaternion::normalise()
{
  /* compute magnitude of the quaternion */
  float mag = static_cast<float>( sqrt ((x*x) + (y*y) + (z*z) + (w*w)) );

  /* check for bogus length, to protect against divide by zero */
  if (mag > 0.0f)
    {
      /* normalize it */
      float oneOverMag = 1.0f / mag;

      x *= oneOverMag;
      y *= oneOverMag;
      z *= oneOverMag;
      w *= oneOverMag;
    }

  return mag;
}

Quaternion Quaternion::operator*(const Quaternion &qb) const
{
	Quaternion out;
	out.w = (w * qb.w) - (x * qb.x) - (y * qb.y) - (z * qb.z);
	out.x = (x * qb.w) + (w * qb.x) + (y * qb.z) - (z * qb.y);
	out.y = (y * qb.w) + (w * qb.y) + (z * qb.x) - (x * qb.z);
	out.z = (z * qb.w) + (w * qb.z) + (x * qb.y) - (y * qb.x);
	return out;
}

static Quaternion Quat_multVec (const Quaternion &q, const Vector3 &v)
{
	Quaternion out;
	out.w = - (q.x * v.x) - (q.y * v.y) - (q.z * v.z);
	out.x =   (q.w * v.x) + (q.y * v.z) - (q.z * v.y);
	out.y =   (q.w * v.y) + (q.z * v.x) - (q.x * v.z);
	out.z =   (q.w * v.z) + (q.x * v.y) - (q.y * v.x);
	return out;
}

Vector3 Quaternion::operator*( const Vector3 &v ) const
{
	Quaternion inv = Inverse();
	Quaternion tmp = Quat_multVec( *this, v );
	Quaternion final = tmp * inv;

	return Vector3( final.x, final.y, final.z );
}

/**
 * More quaternion operations for skeletal animation.
 */

float Quaternion::Dot(const Quaternion &qb) const
{
	return ((x * qb.x) + (y * qb.y) + (z * qb.z) + (w * qb.w));
}

Quaternion Quaternion::Slerp( float t, const Quaternion &qa, const Quaternion &qb )
{
  /* Check for out-of range parameter and return edge points if so */
	if (t <= 0.0)
    {
		return qa;
    }

	if (t >= 1.0)
    {
		return qb;
    }

  /* Compute "cosine of angle between quaternions" using dot product */
	float cosOmega = qa.Dot( qb );

  /* If negative dot, use -q1.  Two quaternions q and -q
     represent the same rotation, but may produce
     different slerp.  We chose q or -q to rotate using
     the acute angle. */
  float q1w = qb.w;
  float q1x = qb.x;
  float q1y = qb.y;
  float q1z = qb.z;

  if (cosOmega < 0.0f)
    {
      q1w = -q1w;
      q1x = -q1x;
      q1y = -q1y;
      q1z = -q1z;
      cosOmega = -cosOmega;
    }

  /* We should have two unit quaternions, so dot should be <= 1.0 */
  assert (cosOmega < 1.1f);

  /* Compute interpolation fraction, checking for quaternions
     almost exactly the same */
  float k0, k1;

  if (cosOmega > 0.9999f)
    {
      /* Very close - just use linear interpolation,
	 which will protect againt a divide by zero */

      k0 = 1.0f - t;
      k1 = t;
    }
  else
    {
      /* Compute the sin of the angle using the
	 trig identity sin^2(omega) + cos^2(omega) = 1 */
      float sinOmega = static_cast<float>( sqrt (1.0f - (cosOmega * cosOmega)) );

      /* Compute the angle from its sin and cosine */
      float omega = atan2 (sinOmega, cosOmega);

      /* Compute inverse of denominator, so we only have
	 to divide once */
      float oneOverSinOmega = 1.0f / sinOmega;

      /* Compute interpolation parameters */
      k0 = static_cast<float>( sin ((1.0f - t) * omega) * oneOverSinOmega );
      k1 = static_cast<float>( sin (t * omega) * oneOverSinOmega );
    }

	/* Interpolate and return new quaternion */
	Quaternion out;
	out.w = (k0 * qa.w) + (k1 * q1w);
	out.x = (k0 * qa.x) + (k1 * q1x);
	out.y = (k0 * qa.y) + (k1 * q1y);
	out.z = (k0 * qa.z) + (k1 * q1z);
	return out;
}

Quaternion Quaternion::UnitInverse() const
{
	return Quaternion( w, -x, -y, -z );
}

Quaternion Quaternion::Inverse() const
{
	Quaternion out = UnitInverse();
	out.normalise();
	return out;
}

void Quaternion::ToAngleAxis( float &angle, Vector3 &axis ) const
{
	Quaternion qp( *this );
	qp.normalise();

	float lenSqr = qp.x*qp.x + qp.y*qp.y + qp.z*qp.z;
	if ( qp.w <= 1.0f && lenSqr > 0.0f )
    {
        angle = 2.0f * (float)acos(qp.w);
        float invLen = 1.0f / (float)sqrt(lenSqr);
        axis.x = qp.x*invLen;
        axis.y = qp.y*invLen;
        axis.z = qp.z*invLen;
    }
    else
    {
        angle = 0.0f;
        axis.x = 1.0f;
        axis.y = 0.0f;
        axis.z = 0.0f;
    }
}
