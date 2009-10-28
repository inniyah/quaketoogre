#ifndef __Q3MODELTOMESH_H__
#define __Q3MODELTOMESH_H__

#include "XmlWriter.h"
#include "MD3Model.h"
#include "Animation.h"

class Q3ModelToMesh: public XmlWriter
{
public:
	Q3ModelToMesh( 
		const MD3Model &model,
		const AnimationList &animations,
		const StringMap &materials,
		int referenceFrame = 0,
		bool convertCoordinates = false );

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

	const MD3Model &mModel;
	const AnimationList &mAnimations;
	const StringMap &mMaterials;
	bool mConvertCoordinates;
	int mReferenceFrame;
};

#endif
