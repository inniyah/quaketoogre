#include "Common.h"
#include "MD3XmlWriter.h"

MD3XmlWriter::MD3XmlWriter( const MD3Structure &model ):
	mModel( model )
{
	buildXml();
}

void MD3XmlWriter::buildXml()
{
	Attributes modelAtts;
	modelAtts.set( "name", mModel.header.name, 64 );
	modelAtts.set( "version", mModel.header.version );
	modelAtts.set( "flags", mModel.header.flags );
	openTag( "model", modelAtts );
	
	// Frames
	Attributes framesAtts;
	framesAtts.set( "count", mModel.header.numFrames );
	openTag( "frames", framesAtts );
	for ( int i = 0; i < mModel.header.numFrames; i++ )
		writeFrame( mModel.frames[i] );
	closeTag();

	// Tags
	Attributes tagsAtts;
	tagsAtts.set( "count", mModel.header.numTags );
	openTag( "tags", tagsAtts );
	for ( int i = 0; i < mModel.header.numFrames*mModel.header.numTags; i++ )
		writeTag( mModel.tags[i] );
	closeTag();

	// Meshes
	Attributes meshesAtts;
	meshesAtts.set( "count", mModel.header.numMeshes );
	openTag( "meshes", meshesAtts );
	for ( int i = 0; i < mModel.header.numMeshes; i++ )
		writeMesh( mModel.meshes[i] );
	closeTag();

	closeTag();
}

void MD3XmlWriter::writeFrame( const MD3Frame &frame )
{
	Attributes frameAtts;
	frameAtts.set( "name", frame.name, 16 );
	openTag( "frame", frameAtts );

	openTag( "mins" ); writeVec3( frame.mins ); closeTag();
	openTag( "maxs" ); writeVec3( frame.maxs ); closeTag();
	openTag( "origin" ); writeVec3( frame.origin ); closeTag();
	openTag( "radius" ); writeData( frame.radius ); closeTag();

	closeTag();
}

void MD3XmlWriter::writeTag( const MD3Tag &tag )
{
	Attributes tagAtts;
	tagAtts.set( "name", tag.name );
	openTag( "tag", tagAtts );

	openTag( "origin" ); writeVec3( tag.origin ); closeTag();
	openTag( "axes" );
	openTag( "x" ); writeVec3( tag.axis[0] ); closeTag();
	openTag( "y" ); writeVec3( tag.axis[1] ); closeTag();
	openTag( "z" ); writeVec3( tag.axis[2] ); closeTag();
	closeTag();

	closeTag();
}

void MD3XmlWriter::writeShader( const MD3Shader &shader )
{
	Attributes shaderAtts;
	shaderAtts.set( "name", shader.name, 64 );
	shaderAtts.set( "index", shader.index );
	openTag( "shader", shaderAtts );
	closeTag();
}

void MD3XmlWriter::writeTriangle( const MD3Triangle &triangle )
{
	Attributes triangleAtts;
	triangleAtts.set( "v1", triangle.indices[0] );
	triangleAtts.set( "v2", triangle.indices[1] );
	triangleAtts.set( "v3", triangle.indices[2] );
	openTag( "triangle", triangleAtts );
	closeTag();
}

void MD3XmlWriter::writeTexCoord( const MD3TexCoord &texCoord )
{
	Attributes tcAtts;
	tcAtts.set( "u", texCoord.uv[0] );
	tcAtts.set( "v", texCoord.uv[1] );
	openTag( "texcoord", tcAtts );
	closeTag();
}

void MD3XmlWriter::writeVertex( const MD3Vertex &vertex )
{
	openTag( "vertex" );

	Attributes posAtts;
	posAtts.set( "x", vertex.position[0] );
	posAtts.set( "y", vertex.position[1] );
	posAtts.set( "z", vertex.position[2] );	
	openTag( "position", posAtts );
	closeTag();

	Attributes normalAtts;
	normalAtts.set( "lat", (vertex.normal >> 8) & 0xFF );
	normalAtts.set( "lng", vertex.normal & 0xFF );
	openTag( "normal", normalAtts );
	closeTag();

	closeTag();
}

void MD3XmlWriter::writeMesh( const MD3Mesh &mesh )
{
	Attributes meshAtts;
	meshAtts.set( "name", mesh.header.name, 64 );
	meshAtts.set( "flags", mesh.header.flags );
	openTag( "mesh", meshAtts );

	// Shaders
	Attributes shadersAtts;
	shadersAtts.set( "count", mesh.header.numShaders );
	openTag( "shaders", shadersAtts );
	for ( int i = 0; i < mesh.header.numShaders; i++ )
		writeShader( mesh.shaders[i] );
	closeTag();

	// Triangles
	Attributes trisAtts;
	trisAtts.set( "count", mesh.header.numTriangles );
	openTag( "triangles", trisAtts );
	for ( int i = 0; i < mesh.header.numTriangles; i++ )
		writeTriangle( mesh.triangles[i] );
	closeTag();

	// Texture Coordinates
	Attributes tcsAtts;
	tcsAtts.set( "count", mesh.header.numVertices );
	openTag( "texcoords", tcsAtts );
	for ( int i = 0; i < mesh.header.numVertices; i++ )
		writeTexCoord( mesh.texCoords[i] );
	closeTag();

	// Vertices
	Attributes vertsAtts;
	vertsAtts.set( "count", mesh.header.numVertices );
	openTag( "vertices", vertsAtts );
	for ( int i = 0; i < mesh.header.numVertices; i++ )
		writeVertex( mesh.vertices[i] );
	closeTag();

	closeTag();
}

void MD3XmlWriter::writeVec2( const float vec[2] )
{
	stringstream ss;
	ss << "(" << vec[0] << ", " << vec[1] << ")";
	writeData( ss.str() );
}

void MD3XmlWriter::writeVec3( const float vec[3] )
{
	stringstream ss;
	ss << "(" << vec[0] << ", " << vec[1] << ", " << vec[2] << ")";
	writeData( ss.str() );
}
