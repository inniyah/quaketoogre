#ifndef __MD5MODELTOMESH_H__
#define __MD5MODELTOMESH_H__

#include "XmlWriter.h"
#include "MD3Structure.h"
#include "Animation.h"

struct md5_mesh_t;
struct md5_triangle_t;

class MD5ModelToMesh: public XmlWriter
{
public:
	MD5ModelToMesh();

	bool build();

	void setConvertCoordinates( bool value ) { mConvertCoords = value; }
	void setInputFile( const string &filename ) { mInputFile = filename; }
	void setOutputFile( const string &filename ) { mOutputFile = filename; }
	void addSubMesh( int index, const string &material ) { mSubMeshes[index] = material; }
	void addAnimation( const string &name, const string &filename ) { mAnimations[name] = filename; }

	static bool isMD5Mesh( const string &filename );
	static bool isMD5Anim( const string &filename );
	static void printInfo( const string &filename );

private:
	void buildSubMesh( const struct md5_mesh_t *mesh, const string &material );
	void buildFace( const struct md5_triangle_t *triangle );
	void buildVertexBuffers( const struct md5_mesh_t *mesh );
	void buildVertex( const float position[3], const float normal[3] );
	void buildTexCoord( const float texCoord[2] );

	void convertVector( const float in[3], float out[3] );

	bool mConvertCoords;
	string mInputFile;
	string mOutputFile;
	StringMap mAnimations;

	typedef map<int, string> SubMeshMap;
	SubMeshMap mSubMeshes;
};

#endif	// __MD5MODELTOMESH_H__