#ifndef __MD2MODEL_H__
#define __MD2MODEL_H__

#include "Quake.h"

struct MD2Header
{
	char	magic[4];
	int		version;
	int		skinWidth;
	int		skinHeight;
	int		frameSize;
	int		numSkins;
	int		numVertices;
	int		numTexCoords;
	int		numTriangles;
	int		numGLCommands;
	int		numFrames;
	int		offsetSkins;
	int		offsetTexCoords;
	int		offsetTriangles;
	int		offsetFrames;
	int		offsetGlCommands;
	int		offsetEnd;
};

struct MD2Skin
{
	unsigned char	name[64];
};

struct MD2Vertex
{
	unsigned char	vertex[3];
	unsigned char	normalIndex;
};

struct MD2FrameHeader
{
	vec3_t	scale;
	vec3_t	translate;
	char	name[16];
};

struct MD2Frame
{
	MD2FrameHeader	header;
	MD2Vertex		*vertices;
};

struct MD2Triangle
{
	short	vertexIndices[3];
	short	textureIndices[3];
};

struct MD2TexCoord
{
	short	u, v;
};

class MD2Model
{
public:
	MD2Model();
	~MD2Model();

	bool load( const string &filename );
	void free();
	
	void printInfo() const;
	
	MD2Header	header;
	MD2Skin		*skins;
	MD2Frame	*frames;
	MD2TexCoord	*texCoords;
	MD2Triangle	*triangles;
};

#endif	// __MD2MODEL_H__