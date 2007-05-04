#include "Common.h"
#include "MD2Structure.h"

MD2Structure::MD2Structure()
{
}

MD2Structure::~MD2Structure()
{
	free();	
}

bool MD2Structure::load( const string &filename )
{
	FILE *f;

	if ( !( f = fopen( filename.c_str(), "rb" ) ) )
	{
		return false;
	}

	fread( &header, sizeof( MD2Header ), 1, f );
	if (	header.magic[0] != 'I' || header.magic[1] != 'D' ||
			header.magic[2] != 'P' || header.magic[3] != '2' ||
			header.version != 8 )
	{
		fclose( f );
		return false;
	}

	skins = new MD2Skin[ header.numSkins ];
	fseek( f, header.offsetSkins, SEEK_SET );
	fread( skins, sizeof( MD2Skin ), header.numSkins, f );

	texCoords = new MD2TexCoord[ header.numTexCoords ];
	fseek( f, header.offsetTexCoords, SEEK_SET );
	fread( texCoords, sizeof( MD2TexCoord ), header.numTexCoords, f );

	triangles = new MD2Triangle[ header.numTriangles ];
	fseek( f, header.offsetTriangles, SEEK_SET );
	fread( triangles, sizeof( MD2Triangle ), header.numTriangles, f );

	frames = new MD2Frame[ header.numFrames ];
	fseek( f, header.offsetFrames, SEEK_SET );
	for ( int i = 0; i < header.numFrames; i++ )
	{
		MD2Frame &frame = frames[i];

		fread( &frame.header, sizeof( MD2FrameHeader ), 1, f );

		frame.vertices = new MD2Vertex[ header.numVertices ];
		fread( frame.vertices, sizeof( MD2Vertex ), header.numVertices, f );
	}

	fclose( f );

	printf( "MD2Structure::load() - Loaded %i skins, %i vertices, %i texture coordinates, %i triangles, %i frames\n", 
		header.numSkins, header.numVertices, header.numTexCoords, header.numTriangles, header.numFrames );

	return true;
}

void MD2Structure::free()
{
	if ( skins )
	{
		delete[] skins;
		skins = NULL;
	}

	if ( texCoords )
	{
		delete[] texCoords;
		texCoords = NULL;
	}

	if ( triangles )
	{
		delete[] triangles;
		triangles = NULL;
	}

	if ( frames )
	{
		for ( int i = 0; i < header.numFrames; i++ )
		{
			delete[] frames[i].vertices;
		}
		delete[] frames;
		frames = NULL;
	}
}

void MD2Structure::printInfo()
{
	cout << "MD2 file info" << endl;
	for ( int i = 0; i < header.numSkins; i++ )
	{
		cout << "Skin " << i << " = " << skins[i].name << endl;
	}
	cout << header.numSkins << " skins total" << endl;
	for ( int i = 0; i < header.numFrames; i++ )
	{
		cout << "Frame " << i << " = " << frames[i].header.name << endl;
	}
	cout << header.numFrames << " frames total" << endl;
}
