/*
-------------------------------------------------------------------------------
Copyright (c) 2009 Nico de Poel

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
#ifndef __MD5MODELTOMESH_H__
#define __MD5MODELTOMESH_H__

#include "XmlWriter.h"
#include "Quake.h"
#include "vector.h"
#include "quaternion.h"

struct md5_model_t;
struct md5_mesh_t;
struct md5_triangle_t;
struct md5_joint_t;
struct md5_anim_t;

class MD5ModelToMesh
{
public:
	MD5ModelToMesh( const GlobalOptions &globals );

	bool build();

	struct SubMeshInfo
	{
		string name;
		string material;
	};

	struct AnimationInfo
	{
		AnimationInfo(): fps(0), lockRoot(false) {}

		string inputFile;
		int fps;
		bool lockRoot;
	};

	void setInputFile( const string &filename ) { mInputFile = filename; }
	void setOutputFile( const string &filename ) { mOutputFile = filename; }
	void setSkeletonName( const string &name ) { mSkeletonName = name; }
	void setOriginBone( const string &bone ) { mOriginBone = bone; }
	void setMaxWeights( int value ) { mMaxWeights = value; }
	SubMeshInfo &getSubMesh( int index ) { return mSubMeshes[index]; }
	AnimationInfo &getAnimation( const string &name ) { return mAnimations[name]; }

	static bool isMD5Mesh( const string &filename );
	static bool isMD5Anim( const string &filename );
	static void printInfo( const string &filename );

private:
	void buildMesh( const struct md5_model_t *mdl );
	void buildSubMesh( const struct md5_mesh_t *mesh, const SubMeshInfo &subMeshInfo );
	void buildFace( const struct md5_triangle_t *triangle );
	void buildVertexBuffers( const struct md5_mesh_t *mesh );
	void buildVertex( const Vector3 &position, const Vector3 &normal );
	void buildTexCoord( const float texCoord[2] );
	void buildBoneAssignments( const struct md5_mesh_t *mesh );

	void buildSkeleton( const struct md5_model_t *mdl );
	void buildBones( const struct md5_model_t *mdl );
	void buildBoneHierarchy( const struct md5_model_t *mdl );
	void buildAnimations( const struct md5_model_t *mdl );
	void buildAnimation( const struct md5_model_t *mdl, const string &name, const AnimationInfo &animInfo );
	void buildTrack( const struct md5_model_t *mdl, const struct md5_anim_t *anim, int jointIndex, const AnimationInfo &animInfo );
	void buildKeyFrame( float time, const Vector3 &translate, const Quaternion &rotate );

	static void generateNormals( const struct md5_mesh_t *mesh, Vector3 *normals );
	static void resampleAnimation( const struct md5_anim_t *in, struct md5_anim_t *out, int fps );
	static const struct md5_joint_t *findJoint( const struct md5_model_t *mdl, const string &name );
	static void jointDifference( const struct md5_joint_t *from, const struct md5_joint_t *to, 
								Vector3 &translate, Quaternion &rotate );
	static void animationDelta( const struct md5_joint_t *baseParent, const struct md5_joint_t *animParent, 
								const struct md5_joint_t *baseJoint, const struct md5_joint_t *animJoint, 
								Quaternion &rotate, Vector3 &translate );

	static void convertCoordSystem( struct md5_model_t *mdl );
	static void convertCoordSystem( struct md5_anim_t *anim );

	const GlobalOptions &mGlobals;

	XmlWriter mMeshWriter;
	XmlWriter mSkelWriter;

	string mInputFile;
	string mOutputFile;
	string mSkeletonName;
	string mOriginBone;

	typedef map<int, SubMeshInfo> SubMeshMap;
	SubMeshMap mSubMeshes;
	int mMaxWeights;

	typedef map<string, AnimationInfo> AnimationMap;
	AnimationMap mAnimations;
};

#endif	// __MD5MODELTOMESH_H__
