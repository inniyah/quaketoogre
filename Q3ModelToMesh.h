/*
-------------------------------------------------------------------------------
Copyright (c) 2009 Nico de Poel

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
#ifndef __Q3MODELTOMESH_H__
#define __Q3MODELTOMESH_H__

#include "XmlWriter.h"
#include "MD3Model.h"
#include "Animation.h"

class Q3ModelToMesh
{
public:
	Q3ModelToMesh( const GlobalOptions &globals );

	bool build();

	void setInputFile( const string &filename ) { mInputFile = filename; }
	void setOutputFile( const string &filename ) { mOutputFile = filename; }
	void setSubMeshMaterial( const string &subMesh, const string &material ) { mMaterials[subMesh] = material; }
	void setReferenceFrame( int frame ) { mReferenceFrame = frame; }
	AnimationInfo &getAnimation( const string &name ) { return mAnimations[name]; }

private:
	void convert();

	void buildSubMesh( const MD3Mesh &mesh );
	void buildFace( const MD3Triangle &triangle );
	void buildVertexBuffers( const MD3Mesh &mesh );
	void buildVertex( const MD3Vertex &vert );
	void buildTexCoord( const MD3TexCoord &texCoord );

	void buildAnimation( const string &name, const AnimationInfo &animInfo );
	void buildTrack( int meshIndex, const AnimationInfo &animInfo );
	void buildKeyframe( const MD3Mesh &mesh, int frame, float time );
	
	void convertPosition( const short position[3], float dest[3] );
	void convertNormal( const short &normal, float dest[3] );

	const GlobalOptions &mGlobals;

	XmlWriter mMeshWriter;

	string mInputFile;
	string mOutputFile;
	StringMap mMaterials;
	int mReferenceFrame;
	AnimationMap mAnimations;

	MD3Model mModel;
};

#endif
