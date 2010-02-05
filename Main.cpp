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
#include "MD3Model.h"
#include "Q2ModelToMesh.h"
#include "Q3ModelToMesh.h"
#include "MD5ModelToMesh.h"
#include "Animation.h"

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#define _chdir chdir
void _chdrive( int ) {}
#endif

GlobalOptions::GlobalOptions():
	convertCoords( true ), writeMaterials( false )
{
}

static GlobalOptions gGlobals;

bool processAnimationFile( TiXmlElement *animFileNode, Q3ModelToMesh &builder )
{
	cout << "Processing animation file" << endl;

	TiXmlElement *filenameNode = animFileNode->FirstChildElement( "inputfile" );
	if ( !filenameNode )
	{
		cout << "[Warning] Animation file declaration misses input filename" << endl;
		return false;
	}

	AnimationFile animFile;
	string animFilename = filenameNode->GetText();
	if ( !animFile.load( animFilename ) )
	{
		cout << "[Warning] Could not load animation file '" << animFilename << "'" << endl;
		return false;
	}

	for ( TiXmlElement *node = animFileNode->FirstChildElement(); node; node = node->NextSiblingElement() )
	{
		const string &nodeName = node->ValueStr();
		if ( nodeName == "convertlegs" )
		{
			cout << "Converting all legs animations from animation file" << endl;
			const AnimationMap &animMap = animFile.getLowerAnimations();
			for ( AnimationMap::const_iterator i = animMap.begin(); i != animMap.end(); ++i )
			{
				builder.getAnimation( i->first ) = i->second;
			}
			break;
		}
		else if ( nodeName == "converttorso" )
		{
			cout << "Converting all torso animations from animation file" << endl;
			const AnimationMap &animMap = animFile.getUpperAnimations();
			for ( AnimationMap::const_iterator i = animMap.begin(); i != animMap.end(); ++i )
			{
				builder.getAnimation( i->first ) = i->second;
			}
			break;
		}
		else if ( nodeName == "convertselection" )
		{
			cout << "Converting a selection of animations from animation file" << endl;
			const AnimationMap &animMap = animFile.getAnimations();
			for ( TiXmlElement *child = node->FirstChildElement(); child; child = child->NextSiblingElement() )
			{
				const string &childName = child->ValueStr();
				if ( childName == "animationname" )
				{
					string animName = child->GetText();
					AnimationMap::const_iterator i = animMap.find( animName );
					if ( i != animMap.end() )
					{
						cout << "Adding animation '" << animName << "'" << endl;
						builder.getAnimation( i->first ) = i->second;
					}
					else
					{
						cout << "[Warning] Cannot find animation '" << animName << "'" << endl;
					}
				}
			}
			break;
		}
	}

	return true;
}

bool processAnimations( TiXmlElement *animsNode, AnimationMap &dest )
{
	cout << "Processing manual animation definitions" << endl;

	for ( TiXmlElement *node = animsNode->FirstChildElement(); node; node = node->NextSiblingElement() )
	{
		const string &nodeName = node->ValueStr();
		if ( nodeName == "animationsequence" )
		{
			TiXmlElement *animNameNode = node->FirstChildElement( "animationname" );
			TiXmlElement *startFrameNode = node->FirstChildElement( "startframe" );
			TiXmlElement *numFramesNode = node->FirstChildElement( "numframes" );
			TiXmlElement *fpsNode = node->FirstChildElement( "fps" );
			if ( !animNameNode || !startFrameNode || !numFramesNode || !fpsNode )
			{
				cout << "[Warning] Invalid animation sequence" << endl;
				continue;
			}
			
			AnimationInfo anim;
			string animName = animNameNode->GetText();
			anim.startFrame = atoi( startFrameNode->GetText() );
			anim.numFrames = atoi( numFramesNode->GetText() );
			anim.framesPerSecond = atoi( fpsNode->GetText() );
			
			cout << "Adding animation '" << animName << "'" << endl;
			dest[animName] = anim;
		}
	}

	return true;
}

bool processMaterials( TiXmlElement *matsNode, Q3ModelToMesh &builder )
{
	cout << "Processing materials" << endl;
	
	for ( TiXmlElement *node = matsNode->FirstChildElement(); node; node = node->NextSiblingElement() )
	{
		const string &nodeName = node->ValueStr();
		if ( nodeName == "material" )
		{
			string key, value;
		
			for ( TiXmlElement *child = node->FirstChildElement(); 
				child; child = child->NextSiblingElement() )
			{
				const string &childName = child->ValueStr();
				if ( childName == "submeshname" )
				{
					key = child->GetText();
				}
				else if ( childName == "materialname" )
				{
					value = child->GetText();
				}
			}
			
			if ( key.empty() )
			{
				cout << "[Warning] Material found without submesh name" << endl;
				continue;
			}
			
			cout << "Using material '" << value << "' for submesh '" << key << "'" << endl;
			builder.setSubMeshMaterial( key, value );
		}
	}
	
	return true;
}

bool convertMD2Mesh( TiXmlElement *configNode )
{
	cout << "Doing MD2 Mesh conversion" << endl;

	Q2ModelToMesh builder( gGlobals );

	// Process the configuration XML tree
	for ( TiXmlElement *node = configNode->FirstChildElement(); node; node = node->NextSiblingElement() )
	{
		const string &nodeName = node->ValueStr();
		if ( nodeName == "inputfile" )
		{
			builder.setInputFile( node->GetText() );
		}
		else if ( nodeName == "outputfile" )
		{
			builder.setOutputFile( node->GetText() );
		}
		else if ( nodeName == "referenceframe" )
		{
			builder.setReferenceFrame( atoi(node->GetText()) );
		}
		else if ( nodeName == "animations" )
		{
			AnimationMap anims;
			processAnimations( node, anims );
			for ( AnimationMap::const_iterator iter = anims.begin(); iter != anims.end(); ++iter )
			{
				builder.getAnimation( iter->first ) = iter->second;
			}
		}
		else if ( nodeName == "materialname" )
		{
			builder.setMaterial( node->GetText() );
		}
	}
	
	if ( !builder.build() )
	{
		cout << "[Error] Failed to convert MD2 file" << endl;
		return false;
	}
	
	return true;
}

bool convertMD3Mesh( TiXmlElement *configNode )
{
	cout << "Doing MD3 Mesh conversion" << endl;
	
	Q3ModelToMesh builder( gGlobals );
	
	// Process the configuration XML tree
	for ( TiXmlElement *node = configNode->FirstChildElement(); node; node = node->NextSiblingElement() )
	{
		const string &nodeName = node->ValueStr();
		if ( nodeName == "inputfile" )
		{
			builder.setInputFile( node->GetText() );
		}
		else if ( nodeName == "outputfile" )
		{
			builder.setOutputFile( node->GetText() );
		}
		else if ( nodeName == "referenceframe" )
		{
			builder.setReferenceFrame( atoi( node->GetText() ) );
		}
		else if ( nodeName == "animationfile" )
		{
			if ( !processAnimationFile( node, builder ) )
				cout << "[Warning] Failed to process animation file" << endl;
		}
		else if ( nodeName == "animations" )
		{
			AnimationMap anims;
			processAnimations( node, anims );
			for ( AnimationMap::const_iterator iter = anims.begin(); iter != anims.end(); ++iter )
			{
				builder.getAnimation( iter->first ) = iter->second;
			}
		}
		else if ( nodeName == "materials" )
		{
			processMaterials( node, builder );
		}
	}
	
	if ( !builder.build() )
	{
		cout << "[Error] Failed to convert MD3 file" << endl;
		return false;
	}
	
	return true;
}

void processSubMesh( TiXmlElement *subMeshNode, MD5ModelToMesh &builder )
{
	int index = -1;
	if ( !subMeshNode->Attribute( "index", &index ) )
	{
		cout << "[Warning] Submesh with no index" << endl;
		return;
	}

	MD5ModelToMesh::SubMeshInfo &smInfo = builder.getSubMesh( index );

	const char *name;
	if ( name = subMeshNode->Attribute( "name" ) )
	{
		smInfo.name = name;
	}

	for ( TiXmlElement *node = subMeshNode->FirstChildElement(); node; node = node->NextSiblingElement() )
	{
		const string &nodeName = node->ValueStr();
		if ( nodeName == "materialname" )
		{
			smInfo.material = node->GetText();
		}
	}
}

void processSubMeshes( TiXmlElement *subMeshesNode, MD5ModelToMesh &builder )
{
	int maxWeights;
	if ( subMeshesNode->Attribute( "maxweights", &maxWeights ) )
		builder.setMaxWeights( maxWeights );

	for ( TiXmlElement *node = subMeshesNode->FirstChildElement(); node; node = node->NextSiblingElement() )
	{
		const string &nodeName = node->ValueStr();
		if ( nodeName == "submesh" )
		{
			processSubMesh( node, builder );
		}
	}
}

void processMD5Animation( TiXmlElement *animNode, MD5ModelToMesh &builder )
{
	const char *name;
	if ( !(name = animNode->Attribute( "name" )) )
	{
		cout << "[Warning] MD5 Animation without a name" << endl;
		return;
	}

	string inputfile;
	bool lockRoot = false;
	for ( TiXmlElement *node = animNode->FirstChildElement(); node; node = node->NextSiblingElement() )
	{
		const string &nodeName = node->ValueStr();
		if ( nodeName == "inputfile" )
		{
			inputfile = node->GetText();
		}
		else if ( nodeName == "lockroot" )
		{
			lockRoot = true;
		}
	}

	if ( inputfile.empty() )
	{
		cout << "[Warning] MD5 Animation '" << (*name) << "' missing input file" << endl;
		return;
	}

	MD5ModelToMesh::AnimationInfo &anim = builder.getAnimation( name );
	anim.inputFile = inputfile;
	anim.lockRoot = lockRoot;
	
	int fps;
	if ( animNode->Attribute( "changefps", &fps ) )
		anim.fps = fps;
}

void processMD5Skeleton( TiXmlElement *skelNode, MD5ModelToMesh &builder )
{
	const char *name;
	if ( !(name = skelNode->Attribute( "name" )) )
	{
		cout << "[Warning] MD5 Skeleton without a name" << endl;
		return;
	}
	builder.setSkeletonName( name );

	const char *originBone;
	if ( originBone = skelNode->Attribute( "moveorigin" ) )
		builder.setOriginBone( originBone );

	for ( TiXmlElement *node = skelNode->FirstChildElement(); node; node = node->NextSiblingElement() )
	{
		const string &nodeName = node->ValueStr();
		if ( nodeName == "md5anim" )
		{
			processMD5Animation( node, builder );
		}
	}
}

bool convertMD5Mesh( TiXmlElement *configNode )
{
	cout << "Doing MD5 Mesh conversion" << endl;
	
	MD5ModelToMesh builder( gGlobals );

	// Process the configuration XML tree
	for ( TiXmlElement *node = configNode->FirstChildElement(); node; node = node->NextSiblingElement() )
	{
		const string &nodeName = node->ValueStr();
		if ( nodeName == "inputfile" )
		{
			builder.setInputFile( node->GetText() );
		}
		else if ( nodeName == "outputfile" )
		{
			builder.setOutputFile( node->GetText() );
		}
		else if ( nodeName == "submeshes" )
		{
			processSubMeshes( node, builder );
		}
		else if ( nodeName == "md5skeleton" )
		{
			processMD5Skeleton( node, builder );
		}
	}
	
	if ( !builder.build() )
	{
		cout << "[Error] Failed to convert MD5 file" << endl;
		return false;
	}
	
	return true;
}

// Returns file name without path
static string changeToWorkingDir( string filepath )
{
	// fnsplit() isn't available on Windows, so we'll have to dissect the file path ourselves...
	size_t drivePos = filepath.find_first_of( ':' );
	if ( drivePos != string::npos )
	{
		// Change drive
		char driveLetter = filepath[0];
		int driveNumber = toupper(driveLetter) - 'A' + 1;
		_chdrive( driveNumber );

		filepath = filepath.substr( drivePos + 1 );
	}

	size_t pathPos = filepath.find_last_of( "\\/" );
	if ( pathPos != string::npos )
	{
		// Change working dir
		string dir = filepath.substr( 0, pathPos );
		_chdir( dir.c_str() );

		filepath = filepath.substr( pathPos + 1 );
	}

	return filepath;
}

bool processConfigFile( const string &filepath )
{
	// Change working directory to script file's local directory
	string filename = changeToWorkingDir( filepath );
	cout << "Loading configuration from file '" << filename << "'" << endl;

	TiXmlDocument config;
	if ( !config.LoadFile( filename ) )
	{
		cout << "[Error] Could not load configuration from file '" << filename << "', reason:" 
			<< endl << "Error " << config.ErrorId() << " on row " << config.ErrorRow() 
			<< " column " << config.ErrorCol() << ":" << endl << config.ErrorDesc() << endl;
		return false;
	}
	
	TiXmlElement *root = config.RootElement();
	if ( !root || root->ValueStr() != "quake2ogre" )
	{
		cout << "[Error] This is not a valid QuakeToOgre configuration file" << endl;
		return false;
	}
	
	gGlobals.convertCoords = root->FirstChildElement( "convertcoordinates" ) ? true : false;
	bool success = false;
	
	for ( TiXmlElement *node = root->FirstChildElement(); node; node = node->NextSiblingElement() )
	{			
		const string &nodeName = node->ValueStr();
		if ( nodeName == "md2mesh" )
		{
			success = convertMD2Mesh( node );
		}
		else if ( nodeName == "md3mesh" )
		{
			success = convertMD3Mesh( node );
		}
		else if ( nodeName == "md5mesh" )
		{
			success = convertMD5Mesh( node );
		}
	}
	
	if ( success )
	{
		cout << "Conversion succeeded!" << endl;
		return true;
	}
	else
	{
		cout << "Conversion failed..." << endl;
		return false;
	}
}

void printUsage()
{
	cout << "Usage:" << endl;
	cout << "QuakeToOgre [config file]" << endl;
	cout << "QuakeToOgre -i [mesh file]" << endl;
}

int main( int argc, char **argv )
{
	if ( argc < 2 )
	{
		printUsage();
		return 1;
	}
	
	if ( !strcmp( argv[1], "-i" ) )
	{
		if ( argc < 3 )
		{
			printUsage();
			return 1;
		}
		
		MD2Model md2model;
		MD3Model md3model;
		if ( md2model.load( argv[2] ) )
		{
			md2model.printInfo();
			return 0;
		}
		else if ( md3model.load( argv[2] ) )
		{
			md3model.printInfo();
			return 0;
		}
		else if ( MD5ModelToMesh::isMD5Mesh( argv[2] ) || MD5ModelToMesh::isMD5Anim( argv[2] ) )
		{
			MD5ModelToMesh::printInfo( argv[2] );
			return 0;
		}
		else
		{
			cout << "[Error] Unknown mesh file format" << endl;
			return 1;
		}
	}
	else
	{
		string filepath = argv[1];
		return processConfigFile( filepath ) ? 0 : 1;
	}
}
