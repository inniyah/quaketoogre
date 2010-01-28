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
 *
 * gcc -Wall -ansi -lGL -lGLU -lglut md5anim.c md5anim.c -o md5model
 */

#include "quaternion.h"
#include <limits>
#include <math.h>
#include <assert.h>
#include <memory.h>

/**
 * Basic quaternion operations.
 */

void
Quat_computeW (quat4_t q)
{
  float t = 1.0f - (q[X] * q[X]) - (q[Y] * q[Y]) - (q[Z] * q[Z]);

  if (t < 0.0f)
    q[W] = 0.0f;
  else
    q[W] = -sqrt (t);
}

void
Quat_normalize (quat4_t q)
{
  /* compute magnitude of the quaternion */
  float mag = sqrt ((q[X] * q[X]) + (q[Y] * q[Y])
		    + (q[Z] * q[Z]) + (q[W] * q[W]));

  /* check for bogus length, to protect against divide by zero */
  if (mag > 0.0f)
    {
      /* normalize it */
      float oneOverMag = 1.0f / mag;

      q[X] *= oneOverMag;
      q[Y] *= oneOverMag;
      q[Z] *= oneOverMag;
      q[W] *= oneOverMag;
    }
}

void
Quat_multQuat (const quat4_t qa, const quat4_t qb, quat4_t out)
{
  out[W] = (qa[W] * qb[W]) - (qa[X] * qb[X]) - (qa[Y] * qb[Y]) - (qa[Z] * qb[Z]);
  out[X] = (qa[X] * qb[W]) + (qa[W] * qb[X]) + (qa[Y] * qb[Z]) - (qa[Z] * qb[Y]);
  out[Y] = (qa[Y] * qb[W]) + (qa[W] * qb[Y]) + (qa[Z] * qb[X]) - (qa[X] * qb[Z]);
  out[Z] = (qa[Z] * qb[W]) + (qa[W] * qb[Z]) + (qa[X] * qb[Y]) - (qa[Y] * qb[X]);
}

void
Quat_multVec (const quat4_t q, const Vector3 &v, quat4_t out)
{
  out[W] = - (q[X] * v[X]) - (q[Y] * v[Y]) - (q[Z] * v[Z]);
  out[X] =   (q[W] * v[X]) + (q[Y] * v[Z]) - (q[Z] * v[Y]);
  out[Y] =   (q[W] * v[Y]) + (q[Z] * v[X]) - (q[X] * v[Z]);
  out[Z] =   (q[W] * v[Z]) + (q[X] * v[Y]) - (q[Y] * v[X]);
}

void
Quat_rotatePoint (const quat4_t q, const Vector3 &in, Vector3 &out)
{
  quat4_t tmp, inv, final;

  inv[X] = -q[X]; inv[Y] = -q[Y];
  inv[Z] = -q[Z]; inv[W] =  q[W];

  Quat_normalize (inv);

  Quat_multVec (q, in, tmp);
  Quat_multQuat (tmp, inv, final);

  out[X] = final[X];
  out[Y] = final[Y];
  out[Z] = final[Z];
}

/**
 * More quaternion operations for skeletal animation.
 */

float
Quat_dotProduct (const quat4_t qa, const quat4_t qb)
{
  return ((qa[X] * qb[X]) + (qa[Y] * qb[Y]) + (qa[Z] * qb[Z]) + (qa[W] * qb[W]));
}

void
Quat_slerp (const quat4_t qa, const quat4_t qb, float t, quat4_t out)
{
  /* Check for out-of range parameter and return edge points if so */
  if (t <= 0.0)
    {
      memcpy (out, qa, sizeof(quat4_t));
      return;
    }

  if (t >= 1.0)
    {
      memcpy (out, qb, sizeof (quat4_t));
      return;
    }

  /* Compute "cosine of angle between quaternions" using dot product */
  float cosOmega = Quat_dotProduct (qa, qb);

  /* If negative dot, use -q1.  Two quaternions q and -q
     represent the same rotation, but may produce
     different slerp.  We chose q or -q to rotate using
     the acute angle. */
  float q1w = qb[W];
  float q1x = qb[X];
  float q1y = qb[Y];
  float q1z = qb[Z];

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
      float sinOmega = sqrt (1.0f - (cosOmega * cosOmega));

      /* Compute the angle from its sin and cosine */
      float omega = atan2 (sinOmega, cosOmega);

      /* Compute inverse of denominator, so we only have
	 to divide once */
      float oneOverSinOmega = 1.0f / sinOmega;

      /* Compute interpolation parameters */
      k0 = sin ((1.0f - t) * omega) * oneOverSinOmega;
      k1 = sin (t * omega) * oneOverSinOmega;
    }

  /* Interpolate and return new quaternion */
  out[W] = (k0 * qa[3]) + (k1 * q1w);
  out[X] = (k0 * qa[0]) + (k1 * q1x);
  out[Y] = (k0 * qa[1]) + (k1 * q1y);
  out[Z] = (k0 * qa[2]) + (k1 * q1z);
}

void Quat_copy(const quat4_t q, quat4_t out)
{
	out[W] = q[W];
	out[X] = q[X];
	out[Y] = q[Y];
	out[Z] = q[Z];
}

void Quat_conjugate(const quat4_t q, quat4_t out)
{
	out[W] = q[W];
	out[X] = -q[X];
	out[Y] = -q[Y];
	out[Z] = -q[Z];
}

void Quat_inverse(const quat4_t q, quat4_t out)
{
	Quat_conjugate( q, out );
	Quat_normalize( out );
}

void Quat_toAngleAxis( const quat4_t q, float *angle, Vector3 &axis )
{
	quat4_t qp;
	Quat_copy( q, qp );
	Quat_normalize( qp );

	float lenSqr = qp[X]*qp[X] + qp[Y]*qp[Y] + qp[Z]*qp[Z];
	if ( qp[W] <= 1.0f && lenSqr > 0.0f )
    {
        *angle = 2.0f * (float)acos(qp[W]);
        float invLen = 1.0f / (float)sqrt(lenSqr);
        axis[X] = qp[X]*invLen;
        axis[Y] = qp[Y]*invLen;
        axis[Z] = qp[Z]*invLen;
    }
    else
    {
        *angle = 0.0f;
        axis[X] = 1.0f;
        axis[Y] = 0.0f;
        axis[Z] = 0.0f;
    }
}
