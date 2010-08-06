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
#ifndef __Q2MODELTOMESH_H__
#define __Q2MODELTOMESH_H__

#include "XmlWriter.h"
#include "MD2Model.h"
#include "Animation.h"
#include "vector.h"

class Q2ModelToMesh
{
public:
	Q2ModelToMesh( const GlobalOptions &globals );

	bool build();

	void setInputFile( const string &filename ) { mInputFile = filename; }
	void setOutputFile( const string &filename ) { mOutputFile = filename; }
	void setMaterial( const string &material ) { mMaterial = material; }
	void setReferenceFrame( int frame ) { mReferenceFrame = frame; }
	AnimationInfo &getAnimation( const string &name ) { return mAnimations[name]; }
	void setIncludeNormals( bool enable ) { mIncludeNormals = enable; }

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

	void buildAnimation( const string &name, const AnimationInfo &animInfo );
	void buildTrack( const AnimationInfo &animInfo );
	void buildKeyframe( const MD2Frame &frame, float time );
	
	void convertPosition( const unsigned char position[3], const MD2FrameHeader &frameHeader, Vector3 &dest );
	void convertNormal( const unsigned char normalIndex, Vector3 &dest );

	const GlobalOptions &mGlobals;

	XmlWriter mMeshWriter;

	string mInputFile;
	string mOutputFile;
	string mMaterial;
	int mReferenceFrame;
	AnimationMap mAnimations;
	bool mIncludeNormals;

	MD2Model mModel;
};

#endif
