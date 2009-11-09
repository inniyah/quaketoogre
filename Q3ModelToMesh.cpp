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
#include "Q3ModelToMesh.h"

Q3ModelToMesh::Q3ModelToMesh( const GlobalOptions &globals ):
	mGlobals( globals ), mReferenceFrame( 0 )
{
}

bool Q3ModelToMesh::build()
{
	if ( !mModel.load( mInputFile ) )
	{
		cout << "[Error] Could not load input file '" << mInputFile << "'" << endl;
		return false;
	}

	if ( mReferenceFrame >= mModel.header.numFrames )
		mReferenceFrame = 0;

	convert();

	cout << "Saving mesh XML file '" << mOutputFile << "'" << endl;
	if ( !mMeshWriter.saveFile( mOutputFile ) )
	{
		cout << "[Error] Could not save mesh XML file" << endl;
		return false;
	}

	return true;
}

void Q3ModelToMesh::convert()
{
	mMeshWriter.openTag( "mesh" );

	// Build SubMeshes
	mMeshWriter.openTag( "submeshes" );
	for ( int i = 0; i < mModel.header.numMeshes; i++ )
	{
		buildSubMesh( mModel.meshes[i] );
	}
	mMeshWriter.closeTag();

	// Collect SubMesh names
	mMeshWriter.openTag( "submeshnames" );
	for ( int i = 0; i < mModel.header.numMeshes; i++ )
	{
		TiXmlElement *smnameNode = mMeshWriter.openTag( "submeshname" );
		smnameNode->SetAttribute( "name", StringUtil::toString( mModel.meshes[i].header.name, 64 ) );
		smnameNode->SetAttribute( "index", i );
		mMeshWriter.closeTag();
	}
	mMeshWriter.closeTag();

	// Build Animations
	mMeshWriter.openTag( "animations" );
	for ( AnimationList::const_iterator i = mAnimations.begin(); i != mAnimations.end(); ++i )
	{
		const Animation &anim = *i;
		buildAnimation( anim.name, anim.startFrame, anim.numFrames, anim.framesPerSecond );
	}
	mMeshWriter.closeTag();

	mMeshWriter.closeTag();
}

void Q3ModelToMesh::buildSubMesh( const MD3Mesh &mesh )
{
	cout << "Building SubMesh '" << mesh.header.name << "'" << endl;

	// Determine what submesh's material name should be
	// Either straight from the MD3 structure, or from the supplied material names
	string materialName;
	StringMap::const_iterator iter = mMaterials.find( mesh.header.name );
	if ( iter != mMaterials.end() )
		materialName = iter->second;	
	else
		materialName = StringUtil::toString( mesh.shaders[0].name, 64 );

	TiXmlElement *submeshNode = mMeshWriter.openTag( "submesh" );
	submeshNode->SetAttribute( "material", materialName );
	submeshNode->SetAttribute( "usesharedvertices", "false" );
	submeshNode->SetAttribute( "operationtype", "triangle_list" );

	// Faces
	TiXmlElement *facesNode = mMeshWriter.openTag( "faces" );
	facesNode->SetAttribute( "count", mesh.header.numTriangles );
	for ( int i = 0; i < mesh.header.numTriangles; i++ )
	{
		buildFace( mesh.triangles[i] );
	}
	mMeshWriter.closeTag();

	// Geometry
	TiXmlElement *geomNode = mMeshWriter.openTag( "geometry" );
	geomNode->SetAttribute( "vertexcount", mesh.header.numVertices );
	buildVertexBuffers( mesh );
	mMeshWriter.closeTag();

	mMeshWriter.closeTag();
}

void Q3ModelToMesh::buildFace( const MD3Triangle &triangle )
{
	TiXmlElement *faceNode = mMeshWriter.openTag( "face" );
	// Quake 3 has its face direction the other way round, so flip the index order
	faceNode->SetAttribute( "v1", triangle.indices[0] );
	faceNode->SetAttribute( "v2", triangle.indices[2] );
	faceNode->SetAttribute( "v3", triangle.indices[1] );
	mMeshWriter.closeTag();
}

void Q3ModelToMesh::buildVertexBuffers( const MD3Mesh &mesh )
{
	const MD3Vertex *verts = &mesh.vertices[mReferenceFrame * mesh.header.numVertices];

	// Vertices and normals
	TiXmlElement *vbNode = mMeshWriter.openTag( "vertexbuffer" );
	vbNode->SetAttribute( "positions", "true" );
	vbNode->SetAttribute( "normals", "true" );
	for ( int i = 0; i < mesh.header.numVertices; i++ )
	{
		buildVertex( verts[i] );
	}
	mMeshWriter.closeTag();

	// Texture coordinates
	TiXmlElement *tcNode = mMeshWriter.openTag( "vertexbuffer" );
	tcNode->SetAttribute( "texture_coords", 1 );
	tcNode->SetAttribute( "texture_coord_dimensions_0", 2 );
	for ( int i = 0; i < mesh.header.numVertices; i++ )
	{
		buildTexCoord( mesh.texCoords[i] );
	}
	mMeshWriter.closeTag();
}

void Q3ModelToMesh::buildVertex( const MD3Vertex &vert )
{
	float position[3], normal[3];
	convertPosition( vert.position, position );
	convertNormal( vert.normal, normal );

	mMeshWriter.openTag( "vertex" );

	// Position
	TiXmlElement *posNode = mMeshWriter.openTag( "position" );
	posNode->SetAttribute( "x", StringUtil::toString( position[0] ) );
	posNode->SetAttribute( "y", StringUtil::toString( position[1] ) );
	posNode->SetAttribute( "z", StringUtil::toString( position[2] ) );
	mMeshWriter.closeTag();
	
	// Normal
	TiXmlElement *normNode = mMeshWriter.openTag( "normal" );
	normNode->SetAttribute( "x", StringUtil::toString( normal[0] ) );
	normNode->SetAttribute( "y", StringUtil::toString( normal[1] ) );
	normNode->SetAttribute( "z", StringUtil::toString( normal[2] ) );
	mMeshWriter.closeTag();

	mMeshWriter.closeTag();
}

void Q3ModelToMesh::buildTexCoord( const MD3TexCoord &texCoord )
{
	mMeshWriter.openTag( "vertex" );

	TiXmlElement *tcNode = mMeshWriter.openTag( "texcoord" );
	tcNode->SetAttribute( "u", StringUtil::toString( texCoord.uv[0] ) );
	tcNode->SetAttribute( "v", StringUtil::toString( texCoord.uv[1] ) );
	mMeshWriter.closeTag();

	mMeshWriter.closeTag();
}

void Q3ModelToMesh::buildAnimation( const string &name, int startFrame, int numFrames, int fps )
{
	cout << "Building animation '" << name << "'" << endl;

	TiXmlElement *animNode = mMeshWriter.openTag( "animation" );
	animNode->SetAttribute( "name", name );
	animNode->SetAttribute( "length", StringUtil::toString( (float)numFrames / (float)fps ) );

	mMeshWriter.openTag( "tracks" );
	for ( int i = 0; i < mModel.header.numMeshes; i++ )
	{
		buildTrack( i, startFrame, numFrames, fps );
	}
	mMeshWriter.closeTag();

	mMeshWriter.closeTag();
}

void Q3ModelToMesh::buildTrack( int meshIndex, int startFrame, int numFrames, int fps )
{
	const MD3Mesh &mesh = mModel.meshes[meshIndex];

	TiXmlElement *trackNode = mMeshWriter.openTag( "track" );
	trackNode->SetAttribute( "target", "submesh" );
	trackNode->SetAttribute( "type", "morph" );
	trackNode->SetAttribute( "index", meshIndex );

	float time = 0.0f;
	float timePerFrame = 1.0f / (float)fps;

	mMeshWriter.openTag( "keyframes" );
	for ( int i = 0; i < numFrames; i++ )
	{
		buildKeyframe( mesh, startFrame + i, time );
		time += timePerFrame;
	}
	mMeshWriter.closeTag();

	mMeshWriter.closeTag();
}

void Q3ModelToMesh::buildKeyframe( const MD3Mesh &mesh, int frame, float time )
{
	cout << "Building frame " << frame << " for SubMesh '" << mesh.header.name << "'" << endl;

	TiXmlElement *kfNode = mMeshWriter.openTag( "keyframe" );
	kfNode->SetAttribute( "time", StringUtil::toString( time ) );

	const MD3Vertex *verts = &mesh.vertices[frame * mesh.header.numVertices];
	float position[3], normal[3];

	for ( int i = 0; i < mesh.header.numVertices; i++ )
	{
		const MD3Vertex &vertex = verts[i];
		convertPosition( vertex.position, position );
		convertNormal( vertex.normal, normal );

		TiXmlElement *posNode = mMeshWriter.openTag( "position" );
		posNode->SetAttribute( "x", StringUtil::toString( position[0] ) );
		posNode->SetAttribute( "y", StringUtil::toString( position[1] ) );
		posNode->SetAttribute( "z", StringUtil::toString( position[2] ) );
		mMeshWriter.closeTag();
		
		TiXmlElement *normNode = mMeshWriter.openTag( "normal" );
		normNode->SetAttribute( "x", StringUtil::toString( normal[0] ) );
		normNode->SetAttribute( "y", StringUtil::toString( normal[1] ) );
		normNode->SetAttribute( "z", StringUtil::toString( normal[2] ) );
		mMeshWriter.closeTag();		
	}

	mMeshWriter.closeTag();
}

void Q3ModelToMesh::convertPosition( const short position[3], float dest[3] )
{
	dest[0] = (float)position[0] * MD3_SCALE;
	dest[1] = (float)position[1] * MD3_SCALE;
	dest[2] = (float)position[2] * MD3_SCALE;
	
	if ( mGlobals.convertCoords )
		Quake::convertVector( dest );
}

void Q3ModelToMesh::convertNormal( const short &normal, float dest[3] )
{
	double lat = (double)( ( normal >> 8 ) & 0xFF ) / 255.0;
	double lng = (double)( normal & 0xFF ) / 255.0;
	lat *= 6.2831853;
	lng *= 6.2831853;

	dest[0] = (float)( cos(lat) * sin(lng) );
	dest[1] = (float)( sin(lat) * sin(lng) );
	dest[2] = (float)( cos(lng) );
	
	if ( mGlobals.convertCoords )
		Quake::convertVector( dest );	
}

