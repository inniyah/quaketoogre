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
#include "Common.h"
#include "MD2Model.h"

MD2Model::MD2Model():
	skins(NULL), frames(NULL), texCoords(NULL), triangles(NULL)
{
}

MD2Model::~MD2Model()
{
	free();	
}

bool MD2Model::load( const string &filename )
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

	printf( "MD2Model::load() - Loaded %i skins, %i vertices, %i texture coordinates, %i triangles, %i frames\n", 
		header.numSkins, header.numVertices, header.numTexCoords, header.numTriangles, header.numFrames );

	return true;
}

void MD2Model::free()
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

void MD2Model::printInfo() const
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
