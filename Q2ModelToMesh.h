#ifndef __Q2MODELTOMESH_H__
#define __Q2MODELTOMESH_H__

#include "XmlWriter.h"
#include "MD2Structure.h"
#include "Animation.h"

class Q2ModelToMesh: public XmlWriter
{
public:
	Q2ModelToMesh();

	bool build();

	void setInputFile( const string &filename ) { mInputFile = filename; }
	void setOutputFile( const string &filename ) { mOutputFile = filename; }
	void setConvertCoordinates( bool value ) { mConvertCoords = value; }
	void setMaterial( const string &material ) { mMaterial = material; }
	void setReferenceFrame( int frame ) { mReferenceFrame = frame; }
	void setAutoDetectAnimations( bool value ) { mAutoAnims = value; }
	void addAnimation( const Animation &anim ) { mAnimations.push_back( anim ); }

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
	void buildKeyframe( const MD2Frame &frame, float time );
	
	void convertPosition( const unsigned char position[3], const MD2FrameHeader &frameHeader, float dest[3] );
	void convertNormal( const unsigned char normalIndex, float dest[3] );

	string mInputFile;
	string mOutputFile;
	bool mConvertCoords;
	string mMaterial;
	int mReferenceFrame;
	bool mAutoAnims;	// TODO implement this
	AnimationList mAnimations;

	MD2Structure mModel;
};

#endif
