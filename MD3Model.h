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
#ifndef __MD3MODEL_H__
#define __MD3MODEL_H__

#include "Quake.h"

struct MD3Header
{
	char	magic[4];
	int		version;
	char	name[64];
	int		flags;
	int		numFrames;
	int		numTags;
	int		numMeshes;
	int		numSkins;
	int		offsetFrames;
	int		offsetTags;
	int		offsetMeshes;
	int		filesize;
};

struct MD3Frame
{
	float	mins[3];
	float	maxs[3];
	float	origin[3];
	float	radius;
	char	name[16];
};

struct MD3Tag
{
	char	name[64];
	float	origin[3];
	float	axis[3][3];
};

struct MD3Shader
{
	char	name[64];
	int		index;
};

struct MD3Triangle
{
	int		indices[3];
};

struct MD3TexCoord
{
	float	uv[2];
};

struct MD3Vertex
{
	short	position[3];
	short	normal;
};

struct MD3MeshHeader
{
	char	magic[4];
	char	name[64];
	int		flags;
	int		numFrames;
	int		numShaders;
	int		numVertices;
	int		numTriangles;
	int		offsetTriangles;
	int		offsetShaders;
	int		offsetTexCoords;
	int		offsetVertices;
	int		length;
};

struct MD3Mesh
{
	MD3MeshHeader	header;
	MD3Shader		*shaders;
	MD3Triangle		*triangles;
	MD3TexCoord		*texCoords;
	MD3Vertex		*vertices;
};

class MD3Model
{
public:
	MD3Model();
	~MD3Model();

	bool load( const string &filename );
	void free();

	void printInfo() const;

	MD3Header	header;
	MD3Frame	*frames;
	MD3Tag		*tags;
	MD3Mesh		*meshes;
};

#endif	// __MD3MODEL_H__
