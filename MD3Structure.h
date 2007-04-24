#ifndef __MD3STRUCTURE_H__
#define __MD3STRUCTURE_H__

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

class MD3Structure
{
public:
	MD3Structure();
	~MD3Structure();

	bool load( const string &filename );
	void free();

	MD3Header	header;
	MD3Frame	*frames;
	MD3Tag		*tags;
	MD3Mesh		*meshes;
};

typedef vector<const MD3Structure *> MD3StructureList;
typedef map<string, const MD3Structure *> MD3StructureMap;

#endif
