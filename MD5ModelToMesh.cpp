#include "Common.h"
#include "MD5ModelToMesh.h"

#include "md5model.h"

MD5ModelToMesh::MD5ModelToMesh():
	mConvertCoords( false )
{
}

bool MD5ModelToMesh::build()
{
	return false;
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
