#ifndef __MD5MODELTOMESH_H__
#define __MD5MODELTOMESH_H__

#include "XmlWriter.h"
#include "MD3Structure.h"
#include "Animation.h"

class MD5ModelToMesh: public XmlWriter
{
public:
	MD5ModelToMesh();

	bool build();

	void setConvertCoordinates( bool value ) { mConvertCoords = value; }
	void setInputFile( const string &filename ) { mInputFile = filename; }
	void setOutputFile( const string &filename ) { mOutputFile = filename; }
	void addAnimation( const string &name, const string &filename ) { mAnimations[name] = filename; }

	static bool isMD5Mesh( const string &filename );
	static bool isMD5Anim( const string &filename );
	static void printInfo( const string &filename );

private:
	bool mConvertCoords;
	string mInputFile;
	string mOutputFile;
	StringMap mAnimations;
};

#endif	// __MD5MODELTOMESH_H__