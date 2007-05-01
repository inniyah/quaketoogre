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
	void convert();

	void buildSubMesh();
	void buildFace( const MD2Triangle &triangle );
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
