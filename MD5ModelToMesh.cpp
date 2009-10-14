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

	mMeshWriter.openTag( "mesh" );

	mMeshWriter.openTag( "submeshes" );
	for ( SubMeshMap::iterator iter = mSubMeshes.begin(); iter != mSubMeshes.end(); ++iter )
	{
		int index = iter->first;
		if ( index < 0 || index >= mdl.num_meshes )
		{
			cout << "[Warning] Invalid submesh index: " << index << endl;
			continue;
		}

		struct md5_mesh_t *mesh = &mdl.meshes[index];
		PrepareMesh( mesh, mdl.baseSkel );
		buildSubMesh( mesh, iter->second );
	}
	mMeshWriter.closeTag();	// submeshes

	if ( !mSkeletonFile.empty() )
	{
		mMeshWriter.openTag( "skeletonlink" )->SetAttribute( "name", mSkeletonFile );
		mMeshWriter.closeTag();
	}

	mMeshWriter.closeTag();	// mesh
	FreeModel( &mdl );

	if ( !mMeshWriter.saveFile( mOutputFile ) )
	{
		cout << "[Error] Could not save mesh XML file" << endl;
		return false;
	}

	return true;
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

static void crossProduct( const vec3_t a, const vec3_t b, vec3_t out )
{
	out[0] = a[1] * b[2] - b[1] * a[2];
	out[1] = a[2] * b[0] - b[2] * a[0];
	out[2] = a[0] * b[1] - b[0] * a[1];
}

static void normalize( vec3_t v )
{
	float len = (float)sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
	v[0] /= len;
	v[1] /= len;
	v[2] /= len;
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

		// Compute direction vectors
		vec3_t dir[2];
		for ( int j = 0; j < 3; j++ )
			dir[0][j] = v2[j] - v0[j];
		for ( int j = 0; j < 3; j++ )
			dir[1][j] = v1[j] - v0[j];

		vec3_t faceNormal;
		crossProduct( dir[0], dir[1], faceNormal );

		for ( int j = 0; j < 3; j++ )
		{
			vec3_t &normal = normals[tri->index[j]];
			for ( int k = 0; k < 3; k++ )
				normal[k] += faceNormal[k];
		}
	}

	for ( int i = 0; i < mesh->num_verts; i++ )
	{
		normalize( normals[i] );
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
	float v[3], n[3];
	convertVector( position, v );
	convertVector( normal, n );

	mMeshWriter.openTag( "vertex" );

	TiXmlElement *posNode = mMeshWriter.openTag( "position" );
	posNode->SetAttribute( "x", XmlWriter::toStr( v[0] ) );
	posNode->SetAttribute( "y", XmlWriter::toStr( v[1] ) );
	posNode->SetAttribute( "z", XmlWriter::toStr( v[2] ) );
	mMeshWriter.closeTag();
	
	TiXmlElement *normNode = mMeshWriter.openTag( "normal" );
	normNode->SetAttribute( "x", XmlWriter::toStr( n[0] ) );
	normNode->SetAttribute( "y", XmlWriter::toStr( n[1] ) );
	normNode->SetAttribute( "z", XmlWriter::toStr( n[2] ) );
	mMeshWriter.closeTag();

	mMeshWriter.closeTag();	// vertex
}

void MD5ModelToMesh::buildTexCoord( const float texCoord[2] )
{
	mMeshWriter.openTag( "vertex" );

	TiXmlElement *tcNode = mMeshWriter.openTag( "texcoord" );
	tcNode->SetAttribute( "u", XmlWriter::toStr( texCoord[0] ) );
	tcNode->SetAttribute( "v", XmlWriter::toStr( texCoord[1] ) );
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
			vbNode->SetAttribute( "weight", XmlWriter::toStr( w->bias ) );

			mMeshWriter.closeTag();
		}
	}
}

void MD5ModelToMesh::convertVector( const float in[3], float out[3] )
{
	for ( int i = 0; i < 3; i++ )
		out[i] = in[i];

	if ( mConvertCoords )
		Quake::convertCoordinate( out );
}

static string getExtension( const string &filename )
{
	size_t pos = filename.find_last_of( '.' );
	if ( pos == string::npos )
		return "";

	string ext = filename.substr( pos+1 );
	std::transform( ext.begin(), ext.end(), ext.begin(), static_cast<int(*)(int)>(std::tolower) );
	return ext;
}

bool MD5ModelToMesh::isMD5Mesh( const string &filename )
{
	return (getExtension( filename ) == "md5mesh");
}

bool MD5ModelToMesh::isMD5Anim( const string &filename )
{
	return (getExtension( filename ) == "md5anim");
}

void MD5ModelToMesh::printInfo( const string &filename )
{
	if ( isMD5Mesh( filename ) )
	{
		struct md5_model_t mdl;
		if ( !ReadMD5Model( filename.c_str(), &mdl ) )
			return;

		cout << "MD5 Mesh file info" << endl;
		for ( int i = 0; i < mdl.num_meshes; i++ )
		{
			struct md5_mesh_t *mesh = &mdl.meshes[i];
			cout << "Mesh " << i << " shader = " << mesh->shader << endl;
			cout << "Mesh " << i << " has " << mesh->num_verts << " vertices, " 
				<< mesh->num_tris << " triangles, " << mesh->num_weights << " weights" << endl;
		}
		cout << mdl.num_meshes << " meshes total" << endl;

		for ( int i = 0; i < mdl.num_joints; i++ )
		{
			struct md5_joint_t *joint = &mdl.baseSkel[i];
			cout << "Joint " << i << " = " << joint->name << endl;
		}
		cout << mdl.num_joints << " joints total" << endl;

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
