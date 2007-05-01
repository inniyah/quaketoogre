#include "Common.h"
#include "Q2ModelToMesh.h"

Q2ModelToMesh::Q2ModelToMesh( 
	const MD2Structure &model,
	const AnimationList &animations,
	const char *material,
	int referenceFrame, 
	bool convertCoordinates ):
	
	mModel( model ),
	mAnimations( animations ),
	mMaterial( material ),
	mReferenceFrame( referenceFrame ), 
	mConvertCoordinates( convertCoordinates )
{
	if ( mReferenceFrame >= mModel.header.numFrames )
		mReferenceFrame = 0;

	convert();
}

void Q2ModelToMesh::convert()
{
	openTag( "mesh" );

	// Build SubMeshes
	openTag( "submeshes" );
	{
		buildSubMesh();
	}
	closeTag();

	// Build Animations
	openTag( "animations" );
	for ( AnimationList::const_iterator i = mAnimations.begin(); i != mAnimations.end(); ++i )
	{
		const Animation &anim = *i;
		buildAnimation( anim.name, anim.startFrame, anim.numFrames, anim.framesPerSecond );
	}
	closeTag();

	closeTag();
}

void Q2ModelToMesh::buildSubMesh()
{
	string materialName = "";
	if ( mMaterial )
		materialName = mMaterial;
//	else if ( mModel.header.numSkins > 0 )
//		materialName = string( mModel.skins[0].name );	// signed/unsigned mismatch, *sigh*
		
	TiXmlElement *submeshNode = openTag( "submesh" );
	submeshNode->SetAttribute( "material", materialName );
	submeshNode->SetAttribute( "usesharedvertices", "false" );
	submeshNode->SetAttribute( "operationtype", "triangle_list" );
	
	// Faces
	TiXmlElement *facesNode = openTag( "faces" );
	facesNode->SetAttribute( "count", mModel.header.numTriangles );
	for ( int i = 0; i < mModel.header.numTriangles; i++ )
	{
		buildFace( mModel.triangles[i] );
	}
	closeTag();

	// Geometry
	TiXmlElement *geomNode = openTag( "geometry" );
	geomNode->SetAttribute( "vertexcount", mModel.header.numVertices );
	buildVertexBuffers( mModel.frames[mReferenceFrame] );
	closeTag();	
	
	closeTag();		
}

void Q2ModelToMesh::buildFace( const MD2Triangle &triangle )
{
	TiXmlElement *faceNode = openTag( "face" );
	// Flip the index order
	faceNode->SetAttribute( "v1", triangle.vertexIndices[0] );
	faceNode->SetAttribute( "v2", triangle.vertexIndices[2] );
	faceNode->SetAttribute( "v3", triangle.vertexIndices[1] );
	closeTag();
}

void Q2ModelToMesh::buildVertexBuffers( const MD2Frame &frame )
{
	// Vertices and normals
	TiXmlElement *vbNode = openTag( "vertexbuffer" );
	vbNode->SetAttribute( "positions", "true" );
	vbNode->SetAttribute( "normals", "true" );
	for ( int i = 0; i < mModel.header.numVertices; i++ )
	{
		buildVertex( frame, i );
	}
	closeTag();

	// Texture coordinates
/*	TiXmlElement *tcNode = openTag( "vertexbuffer" );
	tcNode->SetAttribute( "texture_coords", 1 );
	tcNode->SetAttribute( "texture_coord_dimensions_0", 2 );
	for ( int i = 0; i < mModel.header.numVertices; i++ )
	{
		// TODO this is going to be weird...
		buildTexCoord( mesh.texCoords[i] );
	}
	closeTag();*/
}

void Q2ModelToMesh::buildVertex( const MD2Frame &frame, int vertIndex )
{
	const MD2Vertex &vert = frame.vertices[vertIndex];
	float position[3], normal[3];
	convertPosition( vert.vertex, frame.header, position );
	convertNormal( vert.normalIndex, normal );

	openTag( "vertex" );

	// Position
	TiXmlElement *posNode = openTag( "position" );
	posNode->SetAttribute( "x", toStr( position[0] ) );
	posNode->SetAttribute( "y", toStr( position[1] ) );
	posNode->SetAttribute( "z", toStr( position[2] ) );
	closeTag();
	
	// Normal
	TiXmlElement *normNode = openTag( "normal" );
	normNode->SetAttribute( "x", toStr( normal[0] ) );
	normNode->SetAttribute( "y", toStr( normal[1] ) );
	normNode->SetAttribute( "z", toStr( normal[2] ) );
	closeTag();

	closeTag();	
}

void Q2ModelToMesh::buildTexCoord( const MD2TexCoord &texCoord )
{
	openTag( "vertex" );

	TiXmlElement *tcNode = openTag( "texcoord" );
	tcNode->SetAttribute( "u", toStr( (float)texCoord.u / (float)mModel.header.skinWidth ) );
	tcNode->SetAttribute( "v", toStr( (float)texCoord.v / (float)mModel.header.skinHeight ) );
	closeTag();

	closeTag();
}

void Q2ModelToMesh::buildAnimation( const string &name, int startFrame, int numFrames, int fps )
{
	cout << "Building animation '" << name << "'" << endl;

	TiXmlElement *animNode = openTag( "animation" );
	animNode->SetAttribute( "name", name );
	animNode->SetAttribute( "length", toStr( (float)numFrames / (float)fps ) );

	openTag( "tracks" );
	buildTrack( startFrame, numFrames, fps );
	closeTag();

	closeTag();
}

void Q2ModelToMesh::buildTrack( int startFrame, int numFrames, int fps )
{
	TiXmlElement *trackNode = openTag( "track" );
	trackNode->SetAttribute( "target", "submesh" );
	trackNode->SetAttribute( "type", "morph" );
	trackNode->SetAttribute( "index", 0 );

	float time = 0.0f;
	float timePerFrame = 1.0f / (float)fps;

	openTag( "keyframes" );
	for ( int i = 0; i < numFrames; i++ )
	{
		buildKeyframe( startFrame + i, time );
		time += timePerFrame;
	}
	closeTag();

	closeTag();
}

void Q2ModelToMesh::buildKeyframe( int frameIndex, float time )
{
	cout << "Building frame " << frameIndex << endl;

	TiXmlElement *kfNode = openTag( "keyframe" );
	kfNode->SetAttribute( "time", toStr( time ) );

	const MD2Frame &frame = mModel.frames[frameIndex];
	float position[3];

	for ( int i = 0; i < mModel.header.numVertices; i++ )
	{
		const MD2Vertex &vertex = frame.vertices[i];
		convertPosition( vertex.vertex, frame.header, position );

		TiXmlElement *posNode = openTag( "position" );
		posNode->SetAttribute( "x", toStr( position[0] ) );
		posNode->SetAttribute( "y", toStr( position[1] ) );
		posNode->SetAttribute( "z", toStr( position[2] ) );
		closeTag();
	}

	closeTag();
}

void Q2ModelToMesh::convertPosition( const unsigned char position[3], const MD2FrameHeader &frameHeader, float dest[3] )
{
	dest[0] = (position[0] * frameHeader.scale[0]) + frameHeader.translate[0];
	dest[1] = (position[1] * frameHeader.scale[1]) + frameHeader.translate[1];
	dest[2] = (position[2] * frameHeader.scale[2]) + frameHeader.translate[2];

	if ( mConvertCoordinates )
		Quake::convertCoordinate( dest );	
}

void Q2ModelToMesh::convertNormal( const unsigned char normalIndex, float dest[3] )
{
	const float *normal = Quake::md2VertexNormals[normalIndex];
	dest[0] = normal[0];
	dest[1] = normal[1];
	dest[2] = normal[2];

	if ( mConvertCoordinates )
		Quake::convertCoordinate( dest );
}
