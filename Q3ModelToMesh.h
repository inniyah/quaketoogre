#ifndef __Q3MODELTOMESH_H__
#define __Q3MODELTOMESH_H__

#include "XmlWriter.h"
#include "MD3Structure.h"
#include "Animation.h"

class Q3ModelToMesh: public XmlWriter
{
public:
	Q3ModelToMesh( 
		const MD3Structure &model,
		const AnimationList &animations,
		int referenceFrame = 0,
		bool convertCoordinates = false );
		
	// only works for lower+upper+head combination
	// otherwise we don't know how the tag dependencies work out
/*	Q3ModelToMesh(
		const MD3StructureList &models,
		const AnimationList &animations,
		int referenceFrame = 0,
		bool convertCoordinates = false );*/

private:
	void convert();

	void buildSubMesh( const MD3Mesh &mesh );
	void buildFace( const MD3Triangle &triangle );
	void buildVertexBuffers( const MD3Mesh &mesh );
	void buildVertex( const MD3Vertex &vert );
	void buildTexCoord( const MD3TexCoord &texCoord );

	void buildAnimation( const string &name, int startFrame, int numFrames, int fps );
	void buildTrack( int meshIndex, int startFrame, int numFrames, int fps );
	void buildKeyframe( const MD3Mesh &mesh, int frame, float time );
	
	void convertPosition( const short position[3], float dest[3] );
	void convertNormal( const short &normal, float dest[3] );

	const MD3Structure &mModel;
	const AnimationList &mAnimations;
	bool mConvertCoordinates;
	int mReferenceFrame;
};
/*
class Q3MeshToSubMesh: public XmlWriter
{
public:
	Q3MeshToSubMesh( const MD3Mesh &mesh, int referenceFrame = 0 );

private:
	void buildSubMesh( const MD3Mesh &mesh );
	void buildFace( const MD3Triangle &triangle );
	void buildVertexBuffers( const MD3Mesh &mesh );
	void buildVertex( const MD3Vertex &vert );
	void buildTexCoord( const MD3TexCoord &texCoord );

	int mReferenceFrame;
};*/

#endif