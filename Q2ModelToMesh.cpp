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
#include "Common.h"
#include "Q2ModelToMesh.h"

Q2ModelToMesh::Q2ModelToMesh( const GlobalOptions &globals ):
	mGlobals( globals ), mReferenceFrame( 0 )
{
}

bool Q2ModelToMesh::build()
{
	if ( !mModel.load( mInputFile ) )
	{
		cout << "[Error] Could not load input file '" << mInputFile << "'" << endl;
		return false;
	}

	if ( mReferenceFrame >= mModel.header.numFrames )
		mReferenceFrame = 0;

	restructureVertices();
	convert();

	cout << "Saving mesh XML file '" << mOutputFile << "'" << endl;
	if ( !mMeshWriter.saveFile( mOutputFile ) )
	{
		cout << "[Error] Could not save mesh XML file" << endl;
		return false;
	}

	return true;
}

void Q2ModelToMesh::restructureVertices()
{
	typedef map<NewVertex, int> NewIndexMap;
	NewIndexMap newIndices;

	int curIndex = 0;

	for ( int i = 0; i < mModel.header.numTriangles; i++ )
	{
		const MD2Triangle &triangle = mModel.triangles[i];
		NewTriangle newTriangle;
		
		for ( int j = 0; j < 3; j++ )
		{
			const int &vertIndex = triangle.vertexIndices[j];
			const int &tcIndex = triangle.textureIndices[j];
			int newIndex;
			
			NewVertex newVert = make_pair<int, int>( vertIndex, tcIndex );
			NewIndexMap::iterator iter = newIndices.find( newVert );
			if ( iter != newIndices.end() )
			{
				newIndex = iter->second;
			}
			else
			{
				newIndex = curIndex++;
				mNewVertices.push_back( newVert );
				newIndices[newVert] = newIndex;
			}			
			newTriangle.indices[j] = newIndex;
		}		
		mNewTriangles.push_back( newTriangle );
	}	
}

void Q2ModelToMesh::convert()
{
	mMeshWriter.openTag( "mesh" );

	// Build SubMeshes
	mMeshWriter.openTag( "submeshes" );
	buildSubMesh();
	mMeshWriter.closeTag();

	// Build Animations
	mMeshWriter.openTag( "animations" );
	for ( AnimationMap::const_iterator i = mAnimations.begin(); i != mAnimations.end(); ++i )
	{
		buildAnimation( i->first, i->second );
	}
	mMeshWriter.closeTag();

	mMeshWriter.closeTag();
}

void Q2ModelToMesh::buildSubMesh()
{
	string materialName;
	if ( !mMaterial.empty() )
		materialName = mMaterial;
	else if ( mModel.header.numSkins > 0 )
		materialName = string( (const char*)mModel.skins[0].name );
		
	TiXmlElement *submeshNode = mMeshWriter.openTag( "submesh" );
	submeshNode->SetAttribute( "material", materialName );
	submeshNode->SetAttribute( "usesharedvertices", "false" );
	submeshNode->SetAttribute( "operationtype", "triangle_list" );
	
	// Faces
	TiXmlElement *facesNode = mMeshWriter.openTag( "faces" );
	facesNode->SetAttribute( "count", (int)mNewTriangles.size() );
	for ( NewTriangleList::const_iterator i = mNewTriangles.begin(); i != mNewTriangles.end(); ++i )
	{
		buildFace( *i );
	}
	mMeshWriter.closeTag();

	// Geometry
	TiXmlElement *geomNode = mMeshWriter.openTag( "geometry" );
	geomNode->SetAttribute( "vertexcount", (int)mNewVertices.size() );
	buildVertexBuffers( mModel.frames[mReferenceFrame] );
	mMeshWriter.closeTag();	
	
	mMeshWriter.closeTag();		
}

void Q2ModelToMesh::buildFace( const Q2ModelToMesh::NewTriangle &triangle )
{
	TiXmlElement *faceNode = mMeshWriter.openTag( "face" );
	// Flip the index order
	faceNode->SetAttribute( "v1", triangle.indices[0] );
	faceNode->SetAttribute( "v2", triangle.indices[2] );
	faceNode->SetAttribute( "v3", triangle.indices[1] );
	mMeshWriter.closeTag();
}

void Q2ModelToMesh::buildVertexBuffers( const MD2Frame &frame )
{
	// Vertices and normals
	TiXmlElement *vbNode = mMeshWriter.openTag( "vertexbuffer" );
	vbNode->SetAttribute( "positions", "true" );
	vbNode->SetAttribute( "normals", "true" );
	for ( int i = 0; i < (int)mNewVertices.size(); i++ )
	{
		buildVertex( frame, i );
	}
	mMeshWriter.closeTag();

	// Texture coordinates
	TiXmlElement *tcNode = mMeshWriter.openTag( "vertexbuffer" );
	tcNode->SetAttribute( "texture_coords", 1 );
	tcNode->SetAttribute( "texture_coord_dimensions_0", 2 );
	for ( int i = 0; i < (int)mNewVertices.size(); i++ )
	{
		const NewVertex &newVert = mNewVertices[i];
		buildTexCoord( mModel.texCoords[newVert.second] );
	}
	mMeshWriter.closeTag();
}

void Q2ModelToMesh::buildVertex( const MD2Frame &frame, int vertIndex )
{
	const NewVertex &newVert = mNewVertices[vertIndex];
	const MD2Vertex &vert = frame.vertices[newVert.first];
	
	Vector3 position, normal;
	convertPosition( vert.vertex, frame.header, position );
	convertNormal( vert.normalIndex, normal );

	mMeshWriter.openTag( "vertex" );

	// Position
	TiXmlElement *posNode = mMeshWriter.openTag( "position" );
	posNode->SetAttribute( "x", StringUtil::toString( position.x ) );
	posNode->SetAttribute( "y", StringUtil::toString( position.y ) );
	posNode->SetAttribute( "z", StringUtil::toString( position.z ) );
	mMeshWriter.closeTag();
	
	// Normal
	TiXmlElement *normNode = mMeshWriter.openTag( "normal" );
	normNode->SetAttribute( "x", StringUtil::toString( normal.x ) );
	normNode->SetAttribute( "y", StringUtil::toString( normal.y ) );
	normNode->SetAttribute( "z", StringUtil::toString( normal.z ) );
	mMeshWriter.closeTag();

	mMeshWriter.closeTag();	
}

void Q2ModelToMesh::buildTexCoord( const MD2TexCoord &texCoord )
{
	mMeshWriter.openTag( "vertex" );

	TiXmlElement *tcNode = mMeshWriter.openTag( "texcoord" );
	tcNode->SetAttribute( "u", StringUtil::toString( (float)texCoord.u / (float)mModel.header.skinWidth ) );
	tcNode->SetAttribute( "v", StringUtil::toString( (float)texCoord.v / (float)mModel.header.skinHeight ) );
	mMeshWriter.closeTag();

	mMeshWriter.closeTag();
}

void Q2ModelToMesh::buildAnimation( const string &name, const AnimationInfo &animInfo )
{
	cout << "Building animation '" << name << "'" << endl;

	TiXmlElement *animNode = mMeshWriter.openTag( "animation" );
	animNode->SetAttribute( "name", name );
	animNode->SetAttribute( "length", StringUtil::toString( (float)animInfo.numFrames / (float)animInfo.framesPerSecond ) );

	mMeshWriter.openTag( "tracks" );
	buildTrack( animInfo );
	mMeshWriter.closeTag();

	mMeshWriter.closeTag();
}

void Q2ModelToMesh::buildTrack( const AnimationInfo &animInfo )
{
	TiXmlElement *trackNode = mMeshWriter.openTag( "track" );
	trackNode->SetAttribute( "target", "submesh" );
	trackNode->SetAttribute( "type", "morph" );
	trackNode->SetAttribute( "index", 0 );

	float time = 0.0f;
	float timePerFrame = 1.0f / (float)animInfo.framesPerSecond;

	mMeshWriter.openTag( "keyframes" );
	for ( int i = 0; i < animInfo.numFrames; i++ )
	{
		const MD2Frame &frame = mModel.frames[animInfo.startFrame + i];
		buildKeyframe( frame, time );
		time += timePerFrame;
	}
	mMeshWriter.closeTag();

	mMeshWriter.closeTag();
}

void Q2ModelToMesh::buildKeyframe( const MD2Frame &frame, float time )
{
	TiXmlElement *kfNode = mMeshWriter.openTag( "keyframe" );
	kfNode->SetAttribute( "time", StringUtil::toString( time ) );

	Vector3 position, normal;

	for ( int i = 0; i < (int)mNewVertices.size(); i++ )
	{
		const NewVertex &newVert = mNewVertices[i];
		const MD2Vertex &vert = frame.vertices[newVert.first];
		convertPosition( vert.vertex, frame.header, position );
		convertNormal( vert.normalIndex, normal );

		TiXmlElement *posNode = mMeshWriter.openTag( "position" );
		posNode->SetAttribute( "x", StringUtil::toString( position.x ) );
		posNode->SetAttribute( "y", StringUtil::toString( position.y ) );
		posNode->SetAttribute( "z", StringUtil::toString( position.z ) );
		mMeshWriter.closeTag();
		
		TiXmlElement *normNode = mMeshWriter.openTag( "normal" );
		normNode->SetAttribute( "x", StringUtil::toString( normal.x ) );
		normNode->SetAttribute( "y", StringUtil::toString( normal.y ) );
		normNode->SetAttribute( "z", StringUtil::toString( normal.z ) );
		mMeshWriter.closeTag();
	}

	mMeshWriter.closeTag();
}

void Q2ModelToMesh::convertPosition( const unsigned char position[3], const MD2FrameHeader &frameHeader, Vector3 &dest )
{
	dest[0] = (position[0] * frameHeader.scale[0]) + frameHeader.translate[0];
	dest[1] = (position[1] * frameHeader.scale[1]) + frameHeader.translate[1];
	dest[2] = (position[2] * frameHeader.scale[2]) + frameHeader.translate[2];

	if ( mGlobals.convertCoords )
		Quake::convertVector( dest );	
}

void Q2ModelToMesh::convertNormal( const unsigned char normalIndex, Vector3 &dest )
{
	const float *normal = Quake::md2VertexNormals[normalIndex];
	dest.x = normal[0];
	dest.y = normal[1];
	dest.z = normal[2];

	if ( mGlobals.convertCoords )
		Quake::convertVector( dest );
}

