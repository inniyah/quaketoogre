#include "Common.h"
#include "Q3ModelToMesh.h"

Q3ModelToMesh::Q3ModelToMesh( 
	const MD3Structure &model,
	const AnimationList &animations,
	int referenceFrame, 
	bool convertCoordinates ):
	
	mModel( model ),
	mAnimations( animations ),
	mReferenceFrame( referenceFrame ), 
	mConvertCoordinates( convertCoordinates )
{
	convert();
}

void Q3ModelToMesh::convert()
{
	openTag( "mesh" );

	// Build SubMeshes
	openTag( "submeshes" );
	for ( int i = 0; i < mModel.header.numMeshes; i++ )
	{
		buildSubMesh( mModel.meshes[i] );
	}
	closeTag();

	// Collect SubMesh names
	openTag( "submeshnames" );
	for ( int i = 0; i < mModel.header.numMeshes; i++ )
	{
		TiXmlElement *smnameNode = openTag( "submeshname" );
		smnameNode->SetAttribute( "name", toStr( mModel.meshes[i].header.name, 64 ) );
		smnameNode->SetAttribute( "index", i );
		closeTag();
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

void Q3ModelToMesh::buildSubMesh( const MD3Mesh &mesh )
{
	cout << "Building SubMesh '" << mesh.header.name << "'" << endl;

	TiXmlElement *submeshNode = openTag( "submesh" );
	submeshNode->SetAttribute( "material", toStr( mesh.shaders[0].name, 64 ) );
	submeshNode->SetAttribute( "usesharedvertices", "false" );
	submeshNode->SetAttribute( "operationtype", "triangle_list" );

	// Faces
	TiXmlElement *facesNode = openTag( "faces" );
	facesNode->SetAttribute( "count", mesh.header.numTriangles );
	for ( int i = 0; i < mesh.header.numTriangles; i++ )
	{
		buildFace( mesh.triangles[i] );
	}
	closeTag();

	// Geometry
	TiXmlElement *geomNode = openTag( "geometry" );
	geomNode->SetAttribute( "vertexcount", mesh.header.numVertices );
	buildVertexBuffers( mesh );
	closeTag();

	closeTag();
}

void Q3ModelToMesh::buildFace( const MD3Triangle &triangle )
{
	TiXmlElement *faceNode = openTag( "face" );
	// Quake 3 has its face direction the other way round, so flip the index order
	faceNode->SetAttribute( "v1", triangle.indices[0] );
	faceNode->SetAttribute( "v2", triangle.indices[2] );
	faceNode->SetAttribute( "v3", triangle.indices[1] );
	closeTag();
}

void Q3ModelToMesh::buildVertexBuffers( const MD3Mesh &mesh )
{
	const MD3Vertex *verts = &mesh.vertices[mReferenceFrame * mesh.header.numVertices];

	// Vertices and normals
	TiXmlElement *vbNode = openTag( "vertexbuffer" );
	vbNode->SetAttribute( "positions", "true" );
	vbNode->SetAttribute( "normals", "true" );
	for ( int i = 0; i < mesh.header.numVertices; i++ )
	{
		buildVertex( verts[i] );
	}
	closeTag();

	// Texture coordinates
	TiXmlElement *tcNode = openTag( "vertexbuffer" );
	tcNode->SetAttribute( "texture_coords", 1 );
	tcNode->SetAttribute( "texture_coord_dimensions_0", 2 );
	for ( int i = 0; i < mesh.header.numVertices; i++ )
	{
		buildTexCoord( mesh.texCoords[i] );
	}
	closeTag();
}

void Q3ModelToMesh::buildVertex( const MD3Vertex &vert )
{
	float position[3], normal[3];
	convertPosition( vert.position, position );
	convertNormal( vert.normal, normal );

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

void Q3ModelToMesh::buildTexCoord( const MD3TexCoord &texCoord )
{
	openTag( "vertex" );

	TiXmlElement *tcNode = openTag( "texcoord" );
	tcNode->SetAttribute( "u", toStr( texCoord.uv[0] ) );
	tcNode->SetAttribute( "v", toStr( texCoord.uv[1] ) );
	closeTag();

	closeTag();
}

void Q3ModelToMesh::buildAnimation( const string &name, int startFrame, int numFrames, int fps )
{
	cout << "Building animation '" << name << "'" << endl;

	TiXmlElement *animNode = openTag( "animation" );
	animNode->SetAttribute( "name", name );
	animNode->SetAttribute( "length", toStr( (float)numFrames / (float)fps ) );

	openTag( "tracks" );
	for ( int i = 0; i < mModel.header.numMeshes; i++ )
	{
		buildTrack( i, startFrame, numFrames, fps );
	}
	closeTag();

	closeTag();
}

void Q3ModelToMesh::buildTrack( int meshIndex, int startFrame, int numFrames, int fps )
{
	const MD3Mesh &mesh = mModel.meshes[meshIndex];

	TiXmlElement *trackNode = openTag( "track" );
	trackNode->SetAttribute( "target", "submesh" );
	trackNode->SetAttribute( "type", "morph" );
	trackNode->SetAttribute( "index", meshIndex );

	float time = 0.0f;
	float timePerFrame = 1.0f / (float)fps;

	openTag( "keyframes" );
	for ( int i = 0; i < numFrames; i++ )
	{
		buildKeyframe( mesh, startFrame + i, time );
		time += timePerFrame;
	}
	closeTag();

	closeTag();
}

void Q3ModelToMesh::buildKeyframe( const MD3Mesh &mesh, int frame, float time )
{
	cout << "Building frame " << frame << " for SubMesh '" << mesh.header.name << "'" << endl;

	TiXmlElement *kfNode = openTag( "keyframe" );
	kfNode->SetAttribute( "time", toStr( time ) );

	const MD3Vertex *verts = &mesh.vertices[frame * mesh.header.numVertices];
	float position[3];

	for ( int i = 0; i < mesh.header.numVertices; i++ )
	{
		const MD3Vertex &vertex = verts[i];
		convertPosition( vertex.position, position );

		TiXmlElement *posNode = openTag( "position" );
		posNode->SetAttribute( "x", toStr( position[0] ) );
		posNode->SetAttribute( "y", toStr( position[1] ) );
		posNode->SetAttribute( "z", toStr( position[2] ) );
		closeTag();
	}

	closeTag();
}

void Q3ModelToMesh::convertPosition( const short position[3], float dest[3] )
{
	dest[0] = (float)position[0] * MD3_SCALE;
	dest[1] = (float)position[1] * MD3_SCALE;
	dest[2] = (float)position[2] * MD3_SCALE;
	
	if ( mConvertCoordinates )
		Quake::convertCoordinate( dest );
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
	
	if ( mConvertCoordinates )
		Quake::convertCoordinate( dest );	
}
