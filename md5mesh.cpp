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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "md5model.h"

#pragma warning(disable:4996)

/**
 * Load an MD5 model from file.
 */
int
ReadMD5Model (const char *filename, struct md5_model_t *mdl)
{
  FILE *fp;
  char buff[512];
  int version;
  int curr_mesh = 0;
  int i;

  fp = fopen (filename, "rb");
  if (!fp)
    {
      fprintf (stderr, "Error: couldn't open \"%s\"!\n", filename);
      return 0;
    }

  while (!feof (fp))
    {
      /* Read whole line */
      fgets (buff, sizeof (buff), fp);

      if (sscanf (buff, " MD5Version %d", &version) == 1)
	{
	  if (version != 10)
	    {
	      /* Bad version */
	      fprintf (stderr, "Error: bad model version\n");
	      fclose (fp);
	      return 0;
	    }
	}
      else if (sscanf (buff, " numJoints %d", &mdl->num_joints) == 1)
	{
	  if (mdl->num_joints > 0)
	    {
	      /* Allocate memory for base skeleton joints */
	      mdl->baseSkel = (struct md5_joint_t *)
		calloc (mdl->num_joints, sizeof (struct md5_joint_t));
	    }
	}
      else if (sscanf (buff, " numMeshes %d", &mdl->num_meshes) == 1)
	{
	  if (mdl->num_meshes > 0)
	    {
	      /* Allocate memory for meshes */
	      mdl->meshes = (struct md5_mesh_t *)
		calloc (mdl->num_meshes, sizeof (struct md5_mesh_t));
	    }
	}
      else if (strncmp (buff, "joints {", 8) == 0)
	{
	  /* Read each joint */
	  for (i = 0; i < mdl->num_joints; ++i)
	    {
	      struct md5_joint_t *joint = &mdl->baseSkel[i];

	      /* Read whole line */
	      fgets (buff, sizeof (buff), fp);

	      if (sscanf (buff, "%s %d ( %f %f %f ) ( %f %f %f )",
			  joint->name, &joint->parent, &joint->pos[0],
			  &joint->pos[1], &joint->pos[2], &joint->orient.x,
			  &joint->orient.y, &joint->orient.z) == 8)
		{
		  /* Compute the w component */
		  Quat_computeW (joint->orient);
		}
	    }
	}
      else if (strncmp (buff, "mesh {", 6) == 0)
	{
	  struct md5_mesh_t *mesh = &mdl->meshes[curr_mesh];
	  int vert_index = 0;
	  int tri_index = 0;
	  int weight_index = 0;
	  float fdata[4];
	  int idata[3];

	  while ((buff[0] != '}') && !feof (fp))
	    {
	      /* Read whole line */
	      fgets (buff, sizeof (buff), fp);

	      if (strstr (buff, "shader "))
		{
		  int quote = 0, j = 0;

		  /* Copy the shader name whithout the quote marks */
		  for (unsigned int i = 0; i < sizeof (buff) && (quote < 2); ++i)
		    {
		      if (buff[i] == '\"')
			quote++;

		      if ((quote == 1) && (buff[i] != '\"'))
			{
			  mesh->shader[j] = buff[i];
			  j++;
			}
		    }
		}
	      else if (sscanf (buff, " numverts %d", &mesh->num_verts) == 1)
		{
		  if (mesh->num_verts > 0)
		    {
		      /* Allocate memory for vertices */
		      mesh->vertices = (struct md5_vertex_t *)
			malloc (sizeof (struct md5_vertex_t) * mesh->num_verts);

			  mesh->vertexArray = (Vector3 *)malloc (sizeof (Vector3) * mesh->num_verts);
		    }
		}
	      else if (sscanf (buff, " numtris %d", &mesh->num_tris) == 1)
		{
		  if (mesh->num_tris > 0)
		    {
		      /* Allocate memory for triangles */
		      mesh->triangles = (struct md5_triangle_t *)
			malloc (sizeof (struct md5_triangle_t) * mesh->num_tris);
		    }
		}
	      else if (sscanf (buff, " numweights %d", &mesh->num_weights) == 1)
		{
		  if (mesh->num_weights > 0)
		    {
		      /* Allocate memory for vertex weights */
		      mesh->weights = (struct md5_weight_t *)
			malloc (sizeof (struct md5_weight_t) * mesh->num_weights);
		    }
		}
	      else if (sscanf (buff, " vert %d ( %f %f ) %d %d", &vert_index,
			       &fdata[0], &fdata[1], &idata[0], &idata[1]) == 5)
		{
		  /* Copy vertex data */
		  mesh->vertices[vert_index].st[0] = fdata[0];
		  mesh->vertices[vert_index].st[1] = fdata[1];
		  mesh->vertices[vert_index].start = idata[0];
		  mesh->vertices[vert_index].count = idata[1];
		}
	      else if (sscanf (buff, " tri %d %d %d %d", &tri_index,
			       &idata[0], &idata[1], &idata[2]) == 4)
		{
		  /* Copy triangle data */
		  mesh->triangles[tri_index ].index[0] = idata[0];
		  mesh->triangles[tri_index ].index[1] = idata[1];
		  mesh->triangles[tri_index ].index[2] = idata[2];
		}
	      else if (sscanf (buff, " weight %d %d %f ( %f %f %f )",
			       &weight_index, &idata[0], &fdata[3],
			       &fdata[0], &fdata[1], &fdata[2]) == 6)
		{
		  /* Copy vertex data */
		  mesh->weights[weight_index].joint  = idata[0];
		  mesh->weights[weight_index].bias   = fdata[3];
		  mesh->weights[weight_index].pos[0] = fdata[0];
		  mesh->weights[weight_index].pos[1] = fdata[1];
		  mesh->weights[weight_index].pos[2] = fdata[2];
		}
	    }

	  curr_mesh++;
	}
    }

  fclose (fp);

  return 1;
}

/**
 * Free resources allocated for the model.
 */
void
FreeModel (struct md5_model_t *mdl)
{
  int i;

  if (mdl->baseSkel)
    {
      free (mdl->baseSkel);
      mdl->baseSkel = NULL;
    }

  if (mdl->meshes)
    {
      /* Free mesh data */
      for (i = 0; i < mdl->num_meshes; ++i)
	{
	  if (mdl->meshes[i].vertices)
	    {
	      free (mdl->meshes[i].vertices);
	      mdl->meshes[i].vertices = NULL;
	    }

	  if (mdl->meshes[i].triangles)
	    {
	      free (mdl->meshes[i].triangles);
	      mdl->meshes[i].triangles = NULL;
	    }

	  if (mdl->meshes[i].weights)
	    {
	      free (mdl->meshes[i].weights);
	      mdl->meshes[i].weights = NULL;
	    }
	  if (mdl->meshes[i].vertexArray)
	  {
		  free(mdl->meshes[i].vertexArray);
		  mdl->meshes[i].vertexArray = NULL;
	  }
	}

      free (mdl->meshes);
      mdl->meshes = NULL;
    }
}

/**
 * Prepare a mesh for drawing.  Compute mesh's final vertex positions
 * given a skeleton.  Put the vertices in vertex arrays.
 */
void
PrepareMesh (struct md5_mesh_t *mesh,
	     const struct md5_joint_t *skeleton)

{
  int i, j;

  Vector3 *vertexArray = mesh->vertexArray;

  /* Setup vertices */
  for (i = 0; i < mesh->num_verts; ++i)
    {
      Vector3 finalVertex;

      /* Calculate final vertex to draw with weights */
      for (j = 0; j < mesh->vertices[i].count; ++j)
	{
	  const struct md5_weight_t *weight
	    = &mesh->weights[mesh->vertices[i].start + j];
	  const struct md5_joint_t *joint
	    = &skeleton[weight->joint];

	  /* Calculate transformed vertex for this weight */
	  Vector3 wv = joint->orient * weight->pos;

	  /* The sum of all weight->bias should be 1.0 */
	  finalVertex += (joint->pos + wv) * weight->bias;
	}

	  vertexArray[i] = finalVertex;
    }
}

void
Quat_computeW (Quaternion &q)
{
  float t = 1.0f - (q.x * q.x) - (q.y * q.y) - (q.z * q.z);

  if (t < 0.0f)
    q.w = 0.0f;
  else
    q.w = -sqrt (t);
}