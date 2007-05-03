#ifndef __Q2MODELTOMESH_H__
#define __Q2MODELTOMESH_H__

#include "XmlWriter.h"
#include "MD2Structure.h"
#include "Animation.h"

class Q2ModelToMesh: public XmlWriter
{
public:
	Q2ModelToMesh( 
		const MD2Structure &model,
		const AnimationList &animations,
		const char *material,	// Can be NULL
		int referenceFrame = 0,
		bool convertCoordinates = false );

private:
	struct NewTriangle
	{
		int indices[3];
	};
	typedef vector<NewTriangle> NewTriangleList;
	typedef pair<int, int> NewVertex;
	typedef vector<NewVertex> NewVertexList;
	
	NewTriangleList mNewTriangles;
	NewVertexList mNewVertices;
	
	/**
	In MD2 meshes, the vertices aren't directly linked to the texture coordinates,
	so it's possible to have several texture coordinates per vertex (or vice versa).
	This type of structure is not possible in Ogre meshes, so we need to restructure
	the mesh's triangles, vertices and texture coordinates to get a 1-on-1 relationship
	between vertices and texture coordinates.
	*/
	void restructureVertices();

	void convert();

	void buildSubMesh();
	void buildFace( const NewTriangle &triangle );
	void buildVertexBuffers( const MD2Frame &frame );
	void buildVertex( const MD2Frame &frame, int vertIndex );
	void buildTexCoord( const MD2TexCoord &texCoord );

	void buildAnimation( const string &name, int startFrame, int numFrames, int fps );
	void buildTrack( int startFrame, int numFrames, int fps );
	void buildKeyframe( int frameIndex, float time );
	
	void convertPosition( const unsigned char position[3], const MD2FrameHeader &frameHeader, float dest[3] );
	void convertNormal( const unsigned char normalIndex, float dest[3] );

	const MD2Structure &mModel;
	const AnimationList &mAnimations;
	const char *mMaterial;
	bool mConvertCoordinates;
	int mReferenceFrame;
};

#endif