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
		Attributes smnameAtts;
		smnameAtts.set( "name", mModel.meshes[i].header.name, 64 );
		smnameAtts.set( "index", i );
		openTag( "submeshname", smnameAtts );
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

	Attributes submeshAtts;
	submeshAtts.set( "material", mesh.shaders[0].name, 64 );
	submeshAtts.set( "usesharedvertices", "false" );
	submeshAtts.set( "operationtype", "triangle_list" );
	openTag( "submesh", submeshAtts );

	// Faces
	Attributes facesAtts;
	facesAtts.set( "count", mesh.header.numTriangles );
	openTag( "faces", facesAtts );
	for ( int i = 0; i < mesh.header.numTriangles; i++ )
	{
		buildFace( mesh.triangles[i] );
	}
	closeTag();

	// Geometry
	Attributes geomAtts;
	geomAtts.set( "vertexcount", mesh.header.numVertices );
	openTag( "geometry", geomAtts );
	buildVertexBuffers( mesh );
	closeTag();

	closeTag();
}

void Q3ModelToMesh::buildFace( const MD3Triangle &triangle )
{
	// For some reason we need to flip the index order
	Attributes faceAtts;
	faceAtts.set( "v1", triangle.indices[0] );
	faceAtts.set( "v2", triangle.indices[2] );
	faceAtts.set( "v3", triangle.indices[1] );
	openTag( "face", faceAtts );
	closeTag();
}

void Q3ModelToMesh::buildVertexBuffers( const MD3Mesh &mesh )
{
	const MD3Vertex *verts = &mesh.vertices[mReferenceFrame * mesh.header.numVertices];

	// Vertices and normals
	Attributes vbAtts;
	vbAtts.set( "positions", "true" );
	vbAtts.set( "normals", "true" );
	openTag( "vertexbuffer", vbAtts );
	for ( int i = 0; i < mesh.header.numVertices; i++ )
	{
		buildVertex( verts[i] );
	}
	closeTag();

	// Texture coordinates
	Attributes tcAtts;
	tcAtts.set( "texture_coords", "1" );
	tcAtts.set( "texture_coord_dimensions_0", "2" );
	openTag( "vertexbuffer", tcAtts );
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
	Attributes posAtts;
	posAtts.set( "x", position[0] );
	posAtts.set( "y", position[1] );
	posAtts.set( "z", position[2] );
	openTag( "position", posAtts );
	closeTag();
	
	// Normal
	Attributes normAtts;
	normAtts.set( "x", normal[0] );
	normAtts.set( "y", normal[1] );
	normAtts.set( "z", normal[2] );
	openTag( "normal", normAtts );
	closeTag();

	closeTag();
}

void Q3ModelToMesh::buildTexCoord( const MD3TexCoord &texCoord )
{
	openTag( "vertex" );

	Attributes tcAtts;
	tcAtts.set( "u", texCoord.uv[0] );
	tcAtts.set( "v", texCoord.uv[1] );
	openTag( "texcoord", tcAtts );
	closeTag();

	closeTag();
}

void Q3ModelToMesh::buildAnimation( const string &name, int startFrame, int numFrames, int fps )
{
	cout << "Building animation '" << name << "'" << endl;

	Attributes animAtts;
	animAtts.set( "name", name );
	animAtts.set( "length", (float)numFrames / (float)fps );
	openTag( "animation", animAtts );

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

	Attributes trackAtts;
	trackAtts.set( "target", "submesh" );
	trackAtts.set( "type", "morph" );
	trackAtts.set( "index", meshIndex );
	openTag( "track", trackAtts );

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

	Attributes kfAtts;
	kfAtts.set( "time", time );
	openTag( "keyframe", kfAtts );

	const MD3Vertex *verts = &mesh.vertices[frame * mesh.header.numVertices];
	float position[3];

	for ( int i = 0; i < mesh.header.numVertices; i++ )
	{
		const MD3Vertex &vertex = verts[i];
		convertPosition( vertex.position, position );

		Attributes posAtts;
		posAtts.set( "x", position[0] );
		posAtts.set( "y", position[1] );
		posAtts.set( "z", position[2] );
		openTag( "position", posAtts );
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
