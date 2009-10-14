#ifndef __MD5MODELTOMESH_H__
#define __MD5MODELTOMESH_H__

#include "XmlWriter.h"
#include "Quake.h"

struct md5_model_t;
struct md5_mesh_t;
struct md5_triangle_t;
struct md5_joint_t;

class MD5ModelToMesh
{
public:
	MD5ModelToMesh();

	bool build();

	void setConvertCoordinates( bool value ) { mConvertCoords = value; }
	void setInputFile( const string &filename ) { mInputFile = filename; }
	void setOutputFile( const string &filename ) { mOutputFile = filename; }
	void setSkeletonName( const string &name ) { mSkeletonName = name; }
	void addSubMesh( int index, const string &material ) { mSubMeshes[index] = material; }
	void addAnimation( const string &name, const string &filename ) { mAnimations[name] = filename; }

	static bool isMD5Mesh( const string &filename );
	static bool isMD5Anim( const string &filename );
	static void printInfo( const string &filename );

private:
	void buildMesh( const struct md5_model_t *mdl );
	void buildSubMesh( const struct md5_mesh_t *mesh, const string &material );
	void buildFace( const struct md5_triangle_t *triangle );
	void buildVertexBuffers( const struct md5_mesh_t *mesh );
	void buildVertex( const float position[3], const float normal[3] );
	void buildTexCoord( const float texCoord[2] );
	void buildBoneAssignments( const struct md5_mesh_t *mesh );

	void buildSkeleton( const struct md5_model_t *mdl );
	void buildBones( const struct md5_model_t *mdl );
	void buildBoneHierarchy( const struct md5_model_t *mdl );

	void convertVector( const float in[3], float out[3] );

	XmlWriter mMeshWriter;
	XmlWriter mSkelWriter;

	bool mConvertCoords;
	string mInputFile;
	string mOutputFile;
	string mSkeletonName;
	StringMap mAnimations;

	typedef map<int, string> SubMeshMap;
	SubMeshMap mSubMeshes;
};

#endif	// __MD5MODELTOMESH_H__