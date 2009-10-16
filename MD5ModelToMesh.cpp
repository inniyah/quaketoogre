#include "Common.h"
#include "MD5ModelToMesh.h"

#include "md5model.h"

MD5ModelToMesh::MD5ModelToMesh():
	mConvertCoords( false )
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
	
	if ( mConvertCoords )
		convertCoordSystem( &mdl );

	buildMesh( &mdl );
	if ( !mMeshWriter.saveFile( mOutputFile ) )
	{
		cout << "[Error] Could not save mesh XML file" << endl;
		FreeModel( &mdl );
		return false;
	}

	if ( !mSkeletonName.empty() )
	{
		buildSkeleton( &mdl );
		if ( !mSkelWriter.saveFile( mSkeletonName + ".skeleton.xml" ) )
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

		struct md5_mesh_t *mesh = &mdl->meshes[index];
		PrepareMesh( mesh, mdl->baseSkel );
		buildSubMesh( mesh, iter->second );
	}
	mMeshWriter.closeTag();	// submeshes

	if ( !mSkeletonName.empty() )
	{
		mMeshWriter.openTag( "skeletonlink" )->SetAttribute( "name", mSkeletonName + ".skeleton" );
		mMeshWriter.closeTag();
	}

	mMeshWriter.closeTag();	// mesh
}

void MD5ModelToMesh::buildSubMesh( const struct md5_mesh_t *mesh, const string &material )
{
	TiXmlElement *submeshNode = mMeshWriter.openTag( "submesh" );

	string matName = (material.empty() ? mesh->shader : material);
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

static void computeNormals( const struct md5_mesh_t *mesh, vec3_t *normals )
{
	memset( normals, 0, sizeof(vec3_t) * mesh->num_verts );

	for ( int i = 0; i < mesh->num_tris; i++ )
	{
		const struct md5_triangle_t *tri = &mesh->triangles[i];

		vec3_t &v0 = mesh->vertexArray[tri->index[0]];
		vec3_t &v1 = mesh->vertexArray[tri->index[1]];
		vec3_t &v2 = mesh->vertexArray[tri->index[2]];

		// Compute face normal
		vec3_t dir[2];
		vec_subtract( v2, v0, dir[0] );
		vec_subtract( v1, v0, dir[1] );

		vec3_t faceNormal;
		vec_crossProduct( dir[0], dir[1], faceNormal );

		// Add face normal to the normal of each face vertex
		for ( int j = 0; j < 3; j++ )
		{
			vec3_t &normal = normals[tri->index[j]];
			vec_add( normal, faceNormal, normal );
		}
	}

	// Normalize each vertex normal
	for ( int i = 0; i < mesh->num_verts; i++ )
	{
		vec_normalize( normals[i] );
	}
}

void MD5ModelToMesh::buildVertexBuffers( const struct md5_mesh_t *mesh )
{
	vec3_t *normals = new vec3_t[mesh->num_verts];
	computeNormals( mesh, normals );

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

void MD5ModelToMesh::buildVertex( const float position[3], const float normal[3] )
{
	mMeshWriter.openTag( "vertex" );

	TiXmlElement *posNode = mMeshWriter.openTag( "position" );
	posNode->SetAttribute( "x", StringUtil::toString( position[0] ) );
	posNode->SetAttribute( "y", StringUtil::toString( position[1] ) );
	posNode->SetAttribute( "z", StringUtil::toString( position[2] ) );
	mMeshWriter.closeTag();
	
	TiXmlElement *normNode = mMeshWriter.openTag( "normal" );
	normNode->SetAttribute( "x", StringUtil::toString( normal[0] ) );
	normNode->SetAttribute( "y", StringUtil::toString( normal[1] ) );
	normNode->SetAttribute( "z", StringUtil::toString( normal[2] ) );
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

void MD5ModelToMesh::buildBoneAssignments( const struct md5_mesh_t *mesh )
{
	for ( int i = 0; i < mesh->num_verts; i++ )
	{
		const struct md5_vertex_t *v = &mesh->vertices[i];
		for ( int j = 0; j < v->count; j++ )
		{
			const struct md5_weight_t *w = &mesh->weights[v->start + j];

			TiXmlElement *vbNode = mMeshWriter.openTag( "vertexboneassignment" );
			vbNode->SetAttribute( "vertexindex", i );
			vbNode->SetAttribute( "boneindex", w->joint );
			vbNode->SetAttribute( "weight", StringUtil::toString( w->bias ) );

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
		const struct md5_joint_t *parent = NULL;
		if ( joint->parent >= 0 )
			parent = &mdl->baseSkel[joint->parent];

		TiXmlElement *boneNode = mSkelWriter.openTag( "bone" );
		boneNode->SetAttribute( "id", i );
		boneNode->SetAttribute( "name", StringUtil::stripQuotes(joint->name) );

		vec3_t pos;
		quat4_t orient;

		if ( parent )
		{
			// Convert the bone's orientation from world space to joint local space
			jointDifference( parent, joint, pos, orient );
		}
		else
		{
			// Root bone, so just copy the bone's world space orientation
			vec_copy( joint->pos, pos );
			Quat_copy( joint->orient, orient );
		}

		// Ogre's mesh format wants its rotations as axis+angle
		vec3_t axis;
		float angle;
		Quat_toAngleAxis( orient, &angle, axis );

		TiXmlElement *posNode = mSkelWriter.openTag( "position" );
		posNode->SetAttribute( "x", StringUtil::toString( pos[0] ) );
		posNode->SetAttribute( "y", StringUtil::toString( pos[1] ) );
		posNode->SetAttribute( "z", StringUtil::toString( pos[2] ) );
		mSkelWriter.closeTag();	// position

		TiXmlElement *rotNode = mSkelWriter.openTag( "rotation" );
		rotNode->SetAttribute( "angle", StringUtil::toString( angle ) );

		TiXmlElement *axisNode = mSkelWriter.openTag( "axis" );
		axisNode->SetAttribute( "x", StringUtil::toString( axis[0] ) );
		axisNode->SetAttribute( "y", StringUtil::toString( axis[1] ) );
		axisNode->SetAttribute( "z", StringUtil::toString( axis[2] ) );
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

	for ( StringMap::iterator iter = mAnimations.begin(); iter != mAnimations.end(); ++iter )
	{
		struct md5_anim_t anim;
		if ( !ReadMD5Anim( iter->second.c_str(), &anim ) )
		{
			cout << "[Warning] Could not load MD5 animation file '" << iter->second << "'" << endl;
			continue;
		}

		if ( !CheckAnimValidity( mdl, &anim ) )
		{
			cout << "[Warning] MD5 animation file '" << iter->second << "' is not compatible with this model" << endl;
			FreeAnim( &anim );
			continue;
		}

		if ( mConvertCoords )
			convertCoordSystem( &anim );

		buildAnimation( iter->first, mdl, &anim );
		FreeAnim( &anim );
	}

	mSkelWriter.closeTag();	// animations
}

void MD5ModelToMesh::buildAnimation( const string &name, const struct md5_model_t *mdl, const struct md5_anim_t *anim )
{
	TiXmlElement *animTag = mSkelWriter.openTag( "animation" );
	animTag->SetAttribute( "name", name );
	animTag->SetAttribute( "length", StringUtil::toString((float)anim->num_frames / (float)anim->frameRate) );

	mSkelWriter.openTag( "tracks" );
	for ( int i = 0; i < anim->num_joints; i++ )
	{
		buildTrack( mdl, anim, i );
	}
	mSkelWriter.closeTag();	// tracks

	mSkelWriter.closeTag();	// animation
}

void MD5ModelToMesh::buildTrack( const struct md5_model_t *mdl, const struct md5_anim_t *anim, int jointIndex )
{
	const struct md5_joint_t *baseJoint = &mdl->baseSkel[jointIndex];

	TiXmlElement *trackTag = mSkelWriter.openTag( "track" );
	trackTag->SetAttribute( "bone", StringUtil::stripQuotes( baseJoint->name ) );

	mSkelWriter.openTag( "keyframes" );

	for ( int i = 0; i < anim->num_frames; i++ )
	{
		const struct md5_joint_t *animJoint = &anim->skelFrames[i][jointIndex];
		
		int parentIndex = baseJoint->parent;
		if ( parentIndex < 0 )
		{
			buildKeyFrame( baseJoint, animJoint, (float)i / (float)anim->frameRate );
		}
		else
		{
			const struct md5_joint_t *baseParent = &mdl->baseSkel[parentIndex];
			const struct md5_joint_t *animParent = &anim->skelFrames[i][parentIndex];
			
			struct md5_joint_t localJoint, tmpJoint;
			jointDifference( baseParent, baseJoint, localJoint.pos, localJoint.orient );
			jointDifference( animParent, animJoint, tmpJoint.pos, tmpJoint.orient );
			buildKeyFrame( &localJoint, &tmpJoint, (float)i / (float)anim->frameRate );
		}
	}

	mSkelWriter.closeTag();	// keyframes

	mSkelWriter.closeTag();	// track
}

void MD5ModelToMesh::buildKeyFrame( const struct md5_joint_t *base, const struct md5_joint_t *frame, float time )
{
	TiXmlElement *frameTag = mSkelWriter.openTag( "keyframe" );
	frameTag->SetAttribute( "time", StringUtil::toString( time ) );

	vec3_t translate;
	quat4_t rotate;
	jointDifference( base, frame, translate, rotate );

	TiXmlElement *translateTag = mSkelWriter.openTag( "translate" );
	translateTag->SetAttribute( "x", StringUtil::toString( translate[0] ) );
	translateTag->SetAttribute( "y", StringUtil::toString( translate[1] ) );
	translateTag->SetAttribute( "z", StringUtil::toString( translate[2] ) );
	mSkelWriter.closeTag();	// translate

	vec3_t axis;
	float angle;
	Quat_toAngleAxis( rotate, &angle, axis );

	TiXmlElement *rotateTag = mSkelWriter.openTag( "rotate" );
	rotateTag->SetAttribute( "angle", StringUtil::toString( angle ) );
	TiXmlElement *axisTag = mSkelWriter.openTag( "axis" );
	axisTag->SetAttribute( "x", StringUtil::toString( axis[0] ) );
	axisTag->SetAttribute( "y", StringUtil::toString( axis[1] ) );
	axisTag->SetAttribute( "z", StringUtil::toString( axis[2] ) );
	mSkelWriter.closeTag();	// axis
	mSkelWriter.closeTag();	// rotate

	mSkelWriter.closeTag();	// keyframe
}

void MD5ModelToMesh::convertQuaternion( quat4_t q )
{
	static const quat4_t trsf = {0.707107f, 0, 0, -0.707107f};
	
	quat4_t tmp;
	Quat_multQuat( trsf, q, tmp );
	Quat_copy( tmp, q );
}

void MD5ModelToMesh::convertCoordSystem( struct md5_model_t *mdl )
{	
	for ( int i = 0; i < mdl->num_joints; i++ )
	{
		struct md5_joint_t *joint = &mdl->baseSkel[i];
		Quake::convertCoordinate( joint->pos );
		convertQuaternion( joint->orient );
	}
}

void MD5ModelToMesh::convertCoordSystem( struct md5_anim_t *anim )
{
	for ( int i = 0; i < anim->num_frames; i++ )
	{
		for ( int j = 0; j < anim->num_joints; j++ )
		{
			struct md5_joint_t *joint = &anim->skelFrames[i][j];
			Quake::convertCoordinate( joint->pos );
			convertQuaternion( joint->orient );
		}
		
		struct md5_bbox_t *bbox = &anim->bboxes[i];
		Quake::convertCoordinate( bbox->min );
		Quake::convertCoordinate( bbox->max );
	}
}

void MD5ModelToMesh::jointDifference( const struct md5_joint_t *from, 
	const struct md5_joint_t *to, vec3_t translate, quat4_t rotate )
{
	// This computes the transformation from one given joint to another
	quat4_t fromOrientInv;
	Quat_inverse( from->orient, fromOrientInv );
	Quat_multQuat( fromOrientInv, to->orient, rotate );

	vec3_t tmp;
	vec_subtract( to->pos, from->pos, tmp );
	Quat_rotatePoint( fromOrientInv, tmp, translate );
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
