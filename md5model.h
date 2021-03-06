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

#ifndef __MD5MODEL_H__
#define __MD5MODEL_H__

#include "vector.h"
#include "quaternion.h"

/* Joint */
struct md5_joint_t
{
  char name[64];
  int parent;

  Vector3 pos;
  Quaternion orient;
};

/* Vertex */
struct md5_vertex_t
{
  float st[2];

  int start; /* start weight */
  int count; /* weight count */
};

/* Triangle */
struct md5_triangle_t
{
  int index[3];
};

/* Weight */
struct md5_weight_t
{
  int joint;
  float bias;

  Vector3 pos;
};

/* Bounding box */
struct md5_bbox_t
{
  Vector3 min;
  Vector3 max;
};

/* MD5 mesh */
struct md5_mesh_t
{
  struct md5_vertex_t *vertices;
  struct md5_triangle_t *triangles;
  struct md5_weight_t *weights;

  int num_verts;
  int num_tris;
  int num_weights;

  char shader[256];

  Vector3 *vertexArray;
};

/* MD5 model structure */
struct md5_model_t
{
  struct md5_joint_t *baseSkel;
  struct md5_mesh_t *meshes;

  int num_joints;
  int num_meshes;
};

/* Animation data */
struct md5_anim_t
{
  int num_frames;
  int num_joints;
  int frameRate;

  struct md5_joint_t **skelFrames;
  struct md5_bbox_t *bboxes;
};

/* Animation info */
struct anim_info_t
{
  int curr_frame;
  int next_frame;

  double last_time;
  double max_time;
};

/**
 * md5mesh prototypes
 */
int ReadMD5Model (const char *filename, struct md5_model_t *mdl);
void FreeModel (struct md5_model_t *mdl);
void PrepareMesh (struct md5_mesh_t *mesh,
		  const struct md5_joint_t *skeleton);
void AllocVertexArrays ();
void FreeVertexArrays ();

/**
 * md5anim prototypes
 */
int CheckAnimValidity (const struct md5_model_t *mdl,
		       const struct md5_anim_t *anim);
int ReadMD5Anim (const char *filename, struct md5_anim_t *anim);
void FreeAnim (struct md5_anim_t *anim);
void InterpolateSkeletons (const struct md5_joint_t *skelA,
			   const struct md5_joint_t *skelB,
			   int num_joints, float interp,
			   struct md5_joint_t *out);
void Animate (const struct md5_anim_t *anim,
	      struct anim_info_t *animInfo, double dt);

// Utility function
void Quat_computeW (Quaternion &q);

#endif /* __MD5MODEL_H__ */
