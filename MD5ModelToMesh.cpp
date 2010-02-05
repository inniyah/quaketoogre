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
#include "MD5ModelToMesh.h"

#include "md5model.h"

MD5ModelToMesh::MD5ModelToMesh( const GlobalOptions &globals ):
	mGlobals( globals ), mMaxWeights( -1 )
{
}

bool MD5ModelToMesh::build()
{
	struct md5_model_t mdl;
	if ( !ReadMD5Model( mInputFile.c_str(), &mdl ) )
	{
		cout << "[Error] Could not load file '" << mInputFile << "'" << endl;
		return false;
	}
	
	if ( mGlobals.convertCoords )
		convertCoordSystem( &mdl );

	buildMesh( &mdl );

	cout << "Saving mesh XML file '" << mOutputFile << "'" << endl;
	if ( !mMeshWriter.saveFile( mOutputFile ) )
	{
		cout << "[Error] Could not save mesh XML file" << endl;
		FreeModel( &mdl );
		return false;
	}

	if ( !mSkeletonName.empty() )
	{
		buildSkeleton( &mdl );

		string skeletonFile = mSkeletonName + ".skeleton.xml";
		cout << "Saving skeleton XML file '" << skeletonFile << "'" << endl;
		if ( !mSkelWriter.saveFile( skeletonFile ) )
			cout << "[Warning] Could not save skeleton XML file" << endl;
	}

	FreeModel( &mdl );
	return true;
}

void MD5ModelToMesh::buildMesh( const struct md5_model_t *mdl )
{
	mMeshWriter.openTag( "mesh" );

	mMeshWriter.openTag( "submeshes" );
	for ( SubMeshMap::iterator iter = mSubMeshes.begin(); iter != mSubMeshes.end(); ++iter )
	{
		int index = iter->first;
		if ( index < 0 || index >= mdl->num_meshes )
		{
			cout << "[Warning] Invalid submesh index: " << index << endl;
			continue;
		}

		cout << "Building submesh " << index << endl;

		struct md5_mesh_t *mesh = &mdl->meshes[index];
		PrepareMesh( mesh, mdl->baseSkel );
		transformMesh( mdl, mesh );
		buildSubMesh( mesh, iter->second );
	}
	mMeshWriter.closeTag();	// submeshes

	if ( !mSkeletonName.empty() )
	{
		mMeshWriter.openTag( "skeletonlink" )->SetAttribute( "name", mSkeletonName + ".skeleton" );
		mMeshWriter.closeTag();
	}

	mMeshWriter.openTag( "submeshnames" );
	for ( SubMeshMap::const_iterator iter = mSubMeshes.begin(); iter != mSubMeshes.end(); ++iter )
	{
		int index = iter->first;
		const SubMeshInfo &info = iter->second;
		if ( info.name.empty() )
			continue;

		TiXmlElement *nameNode = mMeshWriter.openTag( "submeshname" );
		nameNode->SetAttribute( "index", index );
		nameNode->SetAttribute( "name", info.name );
		mMeshWriter.closeTag();	// submeshname
	}
	mMeshWriter.closeTag();	// submeshnames

	mMeshWriter.closeTag();	// mesh
}

void MD5ModelToMesh::buildSubMesh( const struct md5_mesh_t *mesh, const SubMeshInfo &subMeshInfo )
{
	TiXmlElement *submeshNode = mMeshWriter.openTag( "submesh" );

	string matName = (subMeshInfo.material.empty() ? mesh->shader : subMeshInfo.material);
	submeshNode->SetAttribute( "material", matName );
	submeshNode->SetAttribute( "usesharedvertices", "false" );
	submeshNode->SetAttribute( "operationtype", "triangle_list" );

	// Faces
	TiXmlElement *facesNode = mMeshWriter.openTag( "faces" );
	facesNode->SetAttribute( "count", mesh->num_tris );
	for ( int i = 0; i < mesh->num_tris; i++ )
	{
		buildFace( &mesh->triangles[i] );
	}
	mMeshWriter.closeTag();	// faces

	// Geometry
	TiXmlElement *geomNode = mMeshWriter.openTag( "geometry" );
	geomNode->SetAttribute( "vertexcount", mesh->num_verts );
	buildVertexBuffers( mesh );
	mMeshWriter.closeTag();	// geometry

	// Bone assignments
	mMeshWriter.openTag( "boneassignments" );
	buildBoneAssignments( mesh );
	mMeshWriter.closeTag();	// boneassignments

	mMeshWriter.closeTag();	// submesh
}

void MD5ModelToMesh::buildFace( const struct md5_triangle_t *triangle )
{
	TiXmlElement *faceNode = mMeshWriter.openTag( "face" );
	faceNode->SetAttribute( "v1", triangle->index[0] );
	faceNode->SetAttribute( "v2", triangle->index[2] );
	faceNode->SetAttribute( "v3", triangle->index[1] );
	mMeshWriter.closeTag();	
}

void MD5ModelToMesh::buildVertexBuffers( const struct md5_mesh_t *mesh )
{
	Vector3 *normals = new Vector3[mesh->num_verts];
	generateNormals( mesh, normals );

	// Vertices and normals
	TiXmlElement *vbNode = mMeshWriter.openTag( "vertexbuffer" );
	vbNode->SetAttribute( "positions", "true" );
	vbNode->SetAttribute( "normals", "true" );
	for ( int i = 0; i < mesh->num_verts; i++ )
	{
		buildVertex( mesh->vertexArray[i], normals[i] );
	}
	mMeshWriter.closeTag();	// vertexbuffer

	delete[] normals;

	// Texture coordinates
	TiXmlElement *tcNode = mMeshWriter.openTag( "vertexbuffer" );
	tcNode->SetAttribute( "texture_coords", 1 );
	tcNode->SetAttribute( "texture_coord_dimensions_0", 2 );
	for ( int i = 0; i < mesh->num_verts; i++ )
	{
		buildTexCoord( mesh->vertices[i].st );
	}
	mMeshWriter.closeTag();
}

void MD5ModelToMesh::buildVertex( const Vector3 &position, const Vector3 &normal )
{
	mMeshWriter.openTag( "vertex" );

	TiXmlElement *posNode = mMeshWriter.openTag( "position" );
	posNode->SetAttribute( "x", StringUtil::toString( position.x ) );
	posNode->SetAttribute( "y", StringUtil::toString( position.y ) );
	posNode->SetAttribute( "z", StringUtil::toString( position.z ) );
	mMeshWriter.closeTag();
	
	TiXmlElement *normNode = mMeshWriter.openTag( "normal" );
	normNode->SetAttribute( "x", StringUtil::toString( normal.x ) );
	normNode->SetAttribute( "y", StringUtil::toString( normal.y ) );
	normNode->SetAttribute( "z", StringUtil::toString( normal.z ) );
	mMeshWriter.closeTag();

	mMeshWriter.closeTag();	// vertex
}

void MD5ModelToMesh::buildTexCoord( const float texCoord[2] )
{
	mMeshWriter.openTag( "vertex" );

	TiXmlElement *tcNode = mMeshWriter.openTag( "texcoord" );
	tcNode->SetAttribute( "u", StringUtil::toString( texCoord[0] ) );
	tcNode->SetAttribute( "v", StringUtil::toString( texCoord[1] ) );
	mMeshWriter.closeTag();

	mMeshWriter.closeTag();
}

static bool weightCompare( const struct md5_weight_t *a, const struct md5_weight_t *b )
{
	return (a->bias < b->bias);
}

void MD5ModelToMesh::buildBoneAssignments( const struct md5_mesh_t *mesh )
{
	typedef vector<const struct md5_weight_t *> WeightVector;
	WeightVector weights;

	for ( int i = 0; i < mesh->num_verts; i++ )
	{
		const struct md5_vertex_t *v = &mesh->vertices[i];
		weights.clear();

		// First, sort all the vertex weights on their bias value in descending order
		for ( int j = 0; j < v->count; j++ )
			weights.push_back( &mesh->weights[v->start + j] );
		std::stable_sort( weights.rbegin(), weights.rend(), &weightCompare );

		// Remove the least significant weights, so only mMaxWeights weights remain
		if ( mMaxWeights > 0 && weights.size() > (size_t)mMaxWeights )
			weights.erase( weights.begin() + mMaxWeights, weights.end() );

		// Count the total bias of all the remaining weights
		float totalWeight = 0;
		for ( WeightVector::iterator iter = weights.begin(); iter != weights.end(); ++iter )
			totalWeight += (*iter)->bias;

		// Finally, write all the remaining weights, with adjusted biases
		for ( WeightVector::iterator iter = weights.begin(); iter != weights.end(); ++iter )
		{
			const struct md5_weight_t *w = *iter;

			TiXmlElement *vbNode = mMeshWriter.openTag( "vertexboneassignment" );
			vbNode->SetAttribute( "vertexindex", i );
			vbNode->SetAttribute( "boneindex", w->joint );
			vbNode->SetAttribute( "weight", StringUtil::toString( w->bias / totalWeight ) );

			mMeshWriter.closeTag();
		}
	}
}

void MD5ModelToMesh::buildSkeleton( const struct md5_model_t *mdl )
{
	mSkelWriter.openTag( "skeleton" );
	
	buildBones( mdl );
	buildBoneHierarchy( mdl );

	if ( !mAnimations.empty() )
		buildAnimations( mdl );

	mSkelWriter.closeTag();	// skeleton
}

void MD5ModelToMesh::buildBones( const struct md5_model_t *mdl )
{
	mSkelWriter.openTag( "bones" );
	for ( int i = 0; i < mdl->num_joints; i++ )
	{
		const struct md5_joint_t *joint = &mdl->baseSkel[i];

		TiXmlElement *boneNode = mSkelWriter.openTag( "bone" );
		boneNode->SetAttribute( "id", i );
		boneNode->SetAttribute( "name", StringUtil::stripQuotes(joint->name) );

		Vector3 pos;
		Quaternion orient;
		if ( joint->parent < 0 )
		{
			// Root bone, so just copy the bone's object space orientation
			pos = joint->pos;
			orient = joint->orient;

			// Transform the root bone so that the 'origin bone' gets placed on the origin
			if ( !mOriginBone.empty() )
			{
				const struct md5_joint_t *originJoint = findJoint( mdl, mOriginBone );
				if ( originJoint )
				{
					Quaternion invRotate = originJoint->orient.Inverse();
					orient = invRotate * joint->orient;
					pos = invRotate * (pos - originJoint->pos);
				}
			}
		}
		else
		{
			// Convert the bone's orientation from object space to joint-local space
			const struct md5_joint_t *parent = &mdl->baseSkel[joint->parent];
			jointDifference( parent, joint, pos, orient );
		}

		// Ogre's mesh format wants its rotations as axis+angle
		Vector3 axis;
		float angle;
		orient.ToAngleAxis( angle, axis );

		TiXmlElement *posNode = mSkelWriter.openTag( "position" );
		posNode->SetAttribute( "x", StringUtil::toString( pos.x ) );
		posNode->SetAttribute( "y", StringUtil::toString( pos.y ) );
		posNode->SetAttribute( "z", StringUtil::toString( pos.z ) );
		mSkelWriter.closeTag();	// position

		TiXmlElement *rotNode = mSkelWriter.openTag( "rotation" );
		rotNode->SetAttribute( "angle", StringUtil::toString( angle ) );

		TiXmlElement *axisNode = mSkelWriter.openTag( "axis" );
		axisNode->SetAttribute( "x", StringUtil::toString( axis.x ) );
		axisNode->SetAttribute( "y", StringUtil::toString( axis.y ) );
		axisNode->SetAttribute( "z", StringUtil::toString( axis.z ) );
		mSkelWriter.closeTag();	// axis
		mSkelWriter.closeTag();	// rotation

		mSkelWriter.closeTag();	// bone
	}

	mSkelWriter.closeTag();	// bones
}

void MD5ModelToMesh::buildBoneHierarchy( const struct md5_model_t *mdl )
{
	mSkelWriter.openTag( "bonehierarchy" );

	for ( int i = 0; i < mdl->num_joints; i++ )
	{
		const struct md5_joint_t *joint = &mdl->baseSkel[i];
		if ( joint->parent < 0 )
			continue;

		const struct md5_joint_t *parent = &mdl->baseSkel[joint->parent];

		TiXmlElement *node = mSkelWriter.openTag( "boneparent" );
		node->SetAttribute( "bone", StringUtil::stripQuotes(joint->name) );
		node->SetAttribute( "parent", StringUtil::stripQuotes(parent->name) );
		mSkelWriter.closeTag();
	}

	mSkelWriter.closeTag();	// bonehierarchy
}

void MD5ModelToMesh::buildAnimations( const struct md5_model_t *mdl )
{
	mSkelWriter.openTag( "animations" );

	for ( AnimationMap::iterator iter = mAnimations.begin(); iter != mAnimations.end(); ++iter )
	{
		buildAnimation( mdl, iter->first, iter->second );
	}

	mSkelWriter.closeTag();	// animations
}

void MD5ModelToMesh::buildAnimation( const struct md5_model_t *mdl, const string &name, const AnimationInfo &animInfo )
{
	struct md5_anim_t anim;
	if ( !ReadMD5Anim( animInfo.inputFile.c_str(), &anim ) )
	{
		cout << "[Warning] Could not load MD5 animation file '" << animInfo.inputFile << "'" << endl;
		return;
	}

	if ( !CheckAnimValidity( mdl, &anim ) )
	{
		cout << "[Warning] MD5 animation file '" << animInfo.inputFile << "' is not compatible with this model" << endl;
		FreeAnim( &anim );
		return;
	}

	if ( mGlobals.convertCoords )
		convertCoordSystem( &anim );

	cout << "Building animation '" << name << "'" << endl;

	TiXmlElement *animTag = mSkelWriter.openTag( "animation" );
	animTag->SetAttribute( "name", name );
	animTag->SetAttribute( "length", StringUtil::toString((float)anim.num_frames / (float)anim.frameRate) );

	// Resample the animation to change the animation's framerate
	struct md5_anim_t newAnim, *finalAnim = &anim;
	if ( animInfo.fps > 0 && animInfo.fps != anim.frameRate )
	{
		cout << "Resampling animation to " << animInfo.fps << " fps" << endl;
		resampleAnimation( &anim, &newAnim, animInfo.fps );
		finalAnim = &newAnim;
		FreeAnim( &anim );
	}

	mSkelWriter.openTag( "tracks" );
	for ( int i = 0; i < anim.num_joints; i++ )
	{
		buildTrack( mdl, finalAnim, i, animInfo );
		cout << ((i+1) * 100 / anim.num_joints) << "%\r";
	}
	mSkelWriter.closeTag();	// tracks

	mSkelWriter.closeTag();	// animation

	FreeAnim( finalAnim );
}

void MD5ModelToMesh::buildTrack( const struct md5_model_t *mdl, const struct md5_anim_t *anim, int jointIndex, const AnimationInfo &animInfo )
{
	const struct md5_joint_t *baseJoint = &mdl->baseSkel[jointIndex];
	static Vector3 translate;
	static Quaternion rotate;

	TiXmlElement *trackTag = mSkelWriter.openTag( "track" );
	trackTag->SetAttribute( "bone", StringUtil::stripQuotes( baseJoint->name ) );

	mSkelWriter.openTag( "keyframes" );

	for ( int i = 0; i < anim->num_frames; i++ )
	{
		float time = (float)i / (float)anim->frameRate;
	
		const struct md5_joint_t *animJoint = &anim->skelFrames[i][jointIndex];
		const struct md5_joint_t *baseParent = NULL;
		const struct md5_joint_t *animParent = NULL;		
		
		int parentIndex = baseJoint->parent;
		if ( parentIndex >= 0 )
		{
			baseParent = &mdl->baseSkel[parentIndex];
			animParent = &anim->skelFrames[i][parentIndex];
		}
			
		animationDelta( baseParent, animParent, baseJoint, animJoint, rotate, translate );

		if ( parentIndex < 0 && animInfo.lockRoot )
		{
			// Lock the root bone in place
			translate = Vector3( 0, 0, 0 );
		}

		buildKeyFrame( time, translate, rotate );
	}

	mSkelWriter.closeTag();	// keyframes

	mSkelWriter.closeTag();	// track
}

void MD5ModelToMesh::buildKeyFrame( float time, const Vector3 &translate, const Quaternion &rotate )
{
	TiXmlElement *frameTag = mSkelWriter.openTag( "keyframe" );
	frameTag->SetAttribute( "time", StringUtil::toString( time ) );

	TiXmlElement *translateTag = mSkelWriter.openTag( "translate" );
	translateTag->SetAttribute( "x", StringUtil::toString( translate.x ) );
	translateTag->SetAttribute( "y", StringUtil::toString( translate.y ) );
	translateTag->SetAttribute( "z", StringUtil::toString( translate.z ) );
	mSkelWriter.closeTag();	// translate

	Vector3 axis;
	float angle;
	rotate.ToAngleAxis( angle, axis );

	TiXmlElement *rotateTag = mSkelWriter.openTag( "rotate" );
	rotateTag->SetAttribute( "angle", StringUtil::toString( angle ) );
	TiXmlElement *axisTag = mSkelWriter.openTag( "axis" );
	axisTag->SetAttribute( "x", StringUtil::toString( axis.x ) );
	axisTag->SetAttribute( "y", StringUtil::toString( axis.y ) );
	axisTag->SetAttribute( "z", StringUtil::toString( axis.z ) );
	mSkelWriter.closeTag();	// axis
	mSkelWriter.closeTag();	// rotate

	mSkelWriter.closeTag();	// keyframe
}

void MD5ModelToMesh::transformMesh( const struct md5_model_t *mdl, struct md5_mesh_t *mesh )
{
	if ( mOriginBone.empty() )
		return;

	const struct md5_joint_t *originJoint = findJoint( mdl, mOriginBone );
	if ( !originJoint )
		return;

	Quaternion invRotate = originJoint->orient.Inverse();

	for ( int i = 0; i < mesh->num_verts; ++i )
	{
		Vector3 &v = mesh->vertexArray[i];
		v = invRotate * (v - originJoint->pos);
	}
}

void MD5ModelToMesh::generateNormals( const struct md5_mesh_t *mesh, Vector3 *normals )
{
	memset( normals, 0, sizeof(Vector3) * mesh->num_verts );

	for ( int i = 0; i < mesh->num_tris; i++ )
	{
		const struct md5_triangle_t *tri = &mesh->triangles[i];

		Vector3 &v0 = mesh->vertexArray[tri->index[0]];
		Vector3 &v1 = mesh->vertexArray[tri->index[1]];
		Vector3 &v2 = mesh->vertexArray[tri->index[2]];

		// Compute face normal
		Vector3 faceNormal = (v2 - v0).crossProduct( v1 - v0 );

		// Add face normal to the normal of each face vertex
		for ( int j = 0; j < 3; j++ )
		{
			normals[tri->index[j]] += faceNormal;
		}
	}

	// Normalize each vertex normal
	for ( int i = 0; i < mesh->num_verts; i++ )
	{
		normals[i].normalise();
	}
}

void MD5ModelToMesh::resampleAnimation( const struct md5_anim_t *in, struct md5_anim_t *out, int fps )
{
	memset( out, 0, sizeof(struct md5_anim_t) );
	out->frameRate = fps;
	out->num_joints = in->num_joints;	
	out->num_frames = (in->num_frames * out->frameRate) / in->frameRate;
	out->skelFrames = (struct md5_joint_t **)malloc( sizeof(struct md5_joint_t *) * out->num_frames );

	float interval = (float)in->frameRate / (float)out->frameRate;
	for ( int i = 0; i < out->num_frames; i++ )
	{
		out->skelFrames[i] = (struct md5_joint_t *)malloc( sizeof(struct md5_joint_t) * out->num_joints );

		float position = i * interval;
		int frame = (int)floor( position );
		float interp = position - frame;
		InterpolateSkeletons( in->skelFrames[frame], in->skelFrames[frame+1], in->num_joints, interp, out->skelFrames[i] );
	}
}

const struct md5_joint_t *MD5ModelToMesh::findJoint( const struct md5_model_t *mdl, const string &name )
{
	for ( int i = 0; i < mdl->num_joints; i++ )
	{
		const struct md5_joint_t *joint = &mdl->baseSkel[i];
		if ( StringUtil::stripQuotes(joint->name) == name )
			return joint;
	}
	return NULL;
}

// This computes the transformation from one given joint to another
void MD5ModelToMesh::jointDifference( const struct md5_joint_t *from, const struct md5_joint_t *to, 
									 Vector3 &translate, Quaternion &rotate )
{
	Quaternion fromOrientInv = from->orient.Inverse();

	rotate = fromOrientInv * to->orient;
	translate = fromOrientInv * (to->pos - from->pos);
}

void MD5ModelToMesh::animationDelta( const struct md5_joint_t *baseParent, const struct md5_joint_t *animParent, 
									const struct md5_joint_t *baseJoint, const struct md5_joint_t *animJoint, 
									Quaternion &rotate, Vector3 &translate )
{
	if ( baseParent && animParent )
	{
		struct md5_joint_t relJoint;
		jointDifference( baseParent, baseJoint, relJoint.pos, relJoint.orient );
		
		Quaternion animParentInv = animParent->orient.Inverse();
		
		rotate = relJoint.orient.Inverse() * (animParentInv * animJoint->orient);		
		translate = (animParentInv * (animJoint->pos - animParent->pos)) - relJoint.pos;
	}
	else
	{
		rotate = baseJoint->orient.Inverse() * animJoint->orient;
		translate = animJoint->pos - baseJoint->pos;
	}
}

void MD5ModelToMesh::convertCoordSystem( struct md5_model_t *mdl )
{	
	for ( int i = 0; i < mdl->num_joints; i++ )
	{
		struct md5_joint_t *joint = &mdl->baseSkel[i];
		Quake::convertVector( joint->pos );
		Quake::convertQuaternion( joint->orient );
	}
}

void MD5ModelToMesh::convertCoordSystem( struct md5_anim_t *anim )
{
	for ( int i = 0; i < anim->num_frames; i++ )
	{
		for ( int j = 0; j < anim->num_joints; j++ )
		{
			struct md5_joint_t *joint = &anim->skelFrames[i][j];
			Quake::convertVector( joint->pos );
			Quake::convertQuaternion( joint->orient );
		}
		
		struct md5_bbox_t *bbox = &anim->bboxes[i];
		Quake::convertVector( bbox->min );
		Quake::convertVector( bbox->max );
	}
}

bool MD5ModelToMesh::isMD5Mesh( const string &filename )
{
	return (StringUtil::getExtension( filename ) == "md5mesh");
}

bool MD5ModelToMesh::isMD5Anim( const string &filename )
{
	return (StringUtil::getExtension( filename ) == "md5anim");
}

void MD5ModelToMesh::printInfo( const string &filename )
{
	if ( isMD5Mesh( filename ) )
	{
		struct md5_model_t mdl;
		if ( !ReadMD5Model( filename.c_str(), &mdl ) )
			return;

		cout << "MD5 Mesh file info" << endl;
		for ( int i = 0; i < mdl.num_joints; i++ )
		{
			struct md5_joint_t *joint = &mdl.baseSkel[i];
			cout << "Joint " << i << " = " << joint->name << endl;
		}
		cout << mdl.num_joints << " joints total" << endl;

		for ( int i = 0; i < mdl.num_meshes; i++ )
		{
			struct md5_mesh_t *mesh = &mdl.meshes[i];
			cout << "Mesh " << i << " shader = " << mesh->shader << endl;
			cout << "Mesh " << i << " has " << mesh->num_verts << " vertices, " 
				<< mesh->num_tris << " triangles, " << mesh->num_weights << " weights" << endl;
		}
		cout << mdl.num_meshes << " meshes total" << endl;

		FreeModel( &mdl );
		return;
	}

	if ( isMD5Anim( filename ) )
	{
		struct md5_anim_t anim;
		if ( !ReadMD5Anim( filename.c_str(), &anim ) )
			return;

		cout << "MD5 Animation file info" << endl;

		struct md5_joint_t *frame = anim.skelFrames[0];
		for ( int i = 0; i < anim.num_joints; i++ )
		{
			struct md5_joint_t *joint = &frame[i];
			cout << "Joint " << i << " = " << joint->name << endl;
		}
		cout << anim.num_joints << " joints total" << endl;

		cout << anim.num_frames << " frames at " << anim.frameRate << " fps";

		FreeAnim( &anim );
		return;
	}
}
