#include "Common.h"
#include "MD3Structure.h"

MD3Model::MD3Model():
	frames(NULL), tags(NULL), meshes(NULL)
{
}

MD3Model::~MD3Model()
{
	free();
}

bool MD3Model::load( const string &filename )
{
	FILE *f;

	if ( !( f = fopen( filename.c_str(), "rb" ) ) )
	{
		return false;
	}

	fread( &header, sizeof( MD3Header ), 1, f );
	if (	header.magic[0] != 'I' || header.magic[1] != 'D' ||
			header.magic[2] != 'P' || header.magic[3] != '3' ||
			header.version != 15 )
	{
		fclose( f );
		return false;
	}

	frames = new MD3Frame[ header.numFrames ];
	tags = new MD3Tag[ header.numTags * header.numFrames ];
	meshes = new MD3Mesh[ header.numMeshes ];

	fseek( f, header.offsetFrames, SEEK_SET );
	fread( frames, sizeof( MD3Frame ), header.numFrames, f );

	fseek( f, header.offsetTags, SEEK_SET );
	fread( tags, sizeof( MD3Tag ), header.numTags * header.numFrames, f );

	int offset = header.offsetMeshes;
	for ( int i = 0; i < header.numMeshes; i++ )
	{
		MD3Mesh &mesh = meshes[i];

		fseek( f, offset, SEEK_SET );
		fread( &mesh.header, sizeof( MD3MeshHeader ), 1, f );

		mesh.shaders = new MD3Shader[ mesh.header.numShaders ];
		mesh.triangles = new MD3Triangle[ mesh.header.numTriangles ];
		mesh.texCoords = new MD3TexCoord[ mesh.header.numVertices ];
		mesh.vertices = new MD3Vertex[ mesh.header.numFrames * mesh.header.numVertices ];

		fseek( f, offset + mesh.header.offsetShaders, SEEK_SET );
		fread( mesh.shaders, sizeof( MD3Shader ), mesh.header.numShaders, f );

		fseek( f, offset + mesh.header.offsetTriangles, SEEK_SET );
		fread( mesh.triangles, sizeof( MD3Triangle ), mesh.header.numTriangles, f );

		fseek( f, offset + mesh.header.offsetTexCoords, SEEK_SET );
		fread( mesh.texCoords, sizeof( MD3TexCoord ), mesh.header.numVertices, f );

		fseek( f, offset + mesh.header.offsetVertices, SEEK_SET );
		fread( mesh.vertices, sizeof( MD3Vertex ), mesh.header.numFrames * mesh.header.numVertices, f );

		offset += mesh.header.length;
	}

	printf( "MD3Model::load() - Loaded %i frames, %i meshes\n", header.numFrames, header.numMeshes );

	fclose( f );
	return true;
}

void MD3Model::free()
{
	if ( frames )
	{
		delete[] frames;
		frames = NULL;
	}

	if ( tags )
	{
		delete[] tags;
		tags = NULL;
	}

	if ( meshes )
	{
		for ( int i = 0; i < header.numMeshes; i++ )
		{
			delete[] meshes[i].shaders;
			delete[] meshes[i].triangles;
			delete[] meshes[i].texCoords;
			delete[] meshes[i].vertices;
		}
		delete[] meshes;
		meshes = NULL;
	}
}

void MD3Model::printInfo() const
{
	cout << "MD3 file info" << endl;
	for ( int i = 0; i < header.numFrames; i++ )
	{
		cout << "Frame " << i << " = " << frames[i].name << endl;
	}
	cout << header.numFrames << " frames total" << endl;
	for ( int i = 0; i < header.numTags; i++ )
	{
		cout << "Tag " << i << " = " << tags[i].name << endl;
	}
	cout << header.numTags << " tags total" << endl;
	for ( int i = 0; i < header.numMeshes; i++ )
	{
		cout << "Mesh " << i << " = " << meshes[i].header.name << endl;
		for ( int j = 0; j < meshes[i].header.numShaders; j++ )
		{
			cout << "Mesh " << i << " shader " << j << " = " << meshes[i].shaders[j].name << endl;
		}
		cout << "Mesh " << i << " has " << meshes[i].header.numShaders << " shaders total" << endl;
	}
	cout << header.numMeshes << " meshes total" << endl;
}
