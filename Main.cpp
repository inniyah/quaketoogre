#include "Common.h"
#include "MD2Structure.h"
#include "MD3Structure.h"
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

bool processAnimationFile( TiXmlElement *animFileNode, AnimationList &dest )
{
	cout << "Processing animation file" << endl;

	TiXmlElement *filenameNode = animFileNode->FirstChildElement( "filename" );
	if ( !filenameNode )
	{
		cout << "[Warning] Animation file declaration misses filename" << endl;
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
				dest.push_back( i->second );
			}
			break;
		}
		else if ( nodeName == "converttorso" )
		{
			cout << "Converting all torso animations from animation file" << endl;
			const AnimationMap &animMap = animFile.getUpperAnimations();
			for ( AnimationMap::const_iterator i = animMap.begin(); i != animMap.end(); ++i )
			{
				dest.push_back( i->second );
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
						dest.push_back( i->second );
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

bool processAnimations( TiXmlElement *animsNode, AnimationList &dest )
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
			
			Animation anim;
			anim.name = animNameNode->GetText();
			anim.startFrame = atoi( startFrameNode->GetText() );
			anim.numFrames = atoi( numFramesNode->GetText() );
			anim.framesPerSecond = atoi( fpsNode->GetText() );
			
			cout << "Adding animation '" << anim.name << "'" << endl;
			dest.push_back( anim );
		}
	}

	return true;
}

bool processMaterials( TiXmlElement *matsNode, StringMap &dest )
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
			dest[key] = value;
		}
	}
	
	return true;
}

bool convertMD2Mesh( TiXmlElement *configNode, bool convertCoordinates )
{
	cout << "Doing MD2 Mesh conversion" << endl;
			
	string inputFile, outputFile;
	AnimationList animList;
	const char *material = NULL;
	int referenceFrame = 0;
	
	// Process the configuration XML tree
	for ( TiXmlElement *node = configNode->FirstChildElement(); node; node = node->NextSiblingElement() )
	{
		const string &nodeName = node->ValueStr();
		if ( nodeName == "inputfile" )
		{
			TiXmlElement *filenameNode = node->FirstChildElement( "filename" );
			if ( filenameNode )
				inputFile = filenameNode->GetText();
		}
		else if ( nodeName == "outputfile" )
		{
			TiXmlElement *filenameNode = node->FirstChildElement( "filename" );
			if ( filenameNode )
				outputFile = filenameNode->GetText();		
		}
		else if ( nodeName == "referenceframe" )
		{
			referenceFrame = atoi( node->GetText() );
		}
		else if ( nodeName == "animations" )
		{
			processAnimations( node, animList );
		}
		else if ( nodeName == "materialname" )
		{
			material = node->GetText();
		}
	}
	
	// Try to load the MD3 file
	MD2Structure md2Struct;
	if ( !md2Struct.load( inputFile ) )
	{
		cout << "[Error] Could not load input file '" << inputFile << "'" << endl;
		return false;
	}
	
	Q2ModelToMesh converter( md2Struct, animList, material, referenceFrame, convertCoordinates );
	if ( !converter.saveFile( outputFile ) )
	{
		cout << "[Error] Could not save to file '" << outputFile << "'" << endl;
		return false;
	}
			
	return true;
}

bool convertMD3Mesh( TiXmlElement *configNode, bool convertCoordinates )
{
	cout << "Doing MD3 Mesh conversion" << endl;
	
	string inputFile, outputFile;
	AnimationList animList;
	StringMap materialNames;
	int referenceFrame = 0;
	
	// Process the configuration XML tree
	for ( TiXmlElement *node = configNode->FirstChildElement(); node; node = node->NextSiblingElement() )
	{
		const string &nodeName = node->ValueStr();
		if ( nodeName == "inputfile" )
		{
			TiXmlElement *filenameNode = node->FirstChildElement( "filename" );
			if ( filenameNode )
				inputFile = filenameNode->GetText();
		}
		else if ( nodeName == "outputfile" )
		{
			TiXmlElement *filenameNode = node->FirstChildElement( "filename" );
			if ( filenameNode )
				outputFile = filenameNode->GetText();		
		}
		else if ( nodeName == "referenceframe" )
		{
			referenceFrame = atoi( node->GetText() );
		}
		else if ( nodeName == "animationfile" )
		{
			if ( !processAnimationFile( node, animList ) )
				cout << "[Warning] Failed to process animation file" << endl;
		}
		else if ( nodeName == "animations" )
		{
			processAnimations( node, animList );
		}
		else if ( nodeName == "materials" )
		{
			processMaterials( node, materialNames );
		}
	}
	
	// Try to load the MD3 file
	MD3Structure md3Struct;
	if ( !md3Struct.load( inputFile ) )
	{
		cout << "[Error] Could not load input file '" << inputFile << "'" << endl;
		return false;
	}
	
	Q3ModelToMesh converter( md3Struct, animList, materialNames, referenceFrame, convertCoordinates );
	if ( !converter.saveFile( outputFile ) )
	{
		cout << "[Error] Could not save to file '" << outputFile << "'" << endl;
		return false;
	}
	
	return true;
}

bool convertMD5Mesh( TiXmlElement *configNode, bool convertCoordinates )
{
	cout << "Doing MD5 Mesh conversion" << endl;
	
	MD5ModelToMesh builder;
	builder.setConvertCoordinates( convertCoordinates );

	// Process the configuration XML tree
	for ( TiXmlElement *node = configNode->FirstChildElement(); node; node = node->NextSiblingElement() )
	{
		const string &nodeName = node->ValueStr();
		if ( nodeName == "inputfile" )
		{
			TiXmlElement *filenameNode = node->FirstChildElement( "filename" );
			if ( filenameNode )
				builder.setInputFile( filenameNode->GetText() );
		}
		else if ( nodeName == "outputfile" )
		{
			TiXmlElement *filenameNode = node->FirstChildElement( "filename" );
			if ( filenameNode )
				builder.setOutputFile( filenameNode->GetText() );
		}
		else if ( nodeName == "md5animations" )
		{
			//processAnimations( node, animList );
		}
		else if ( nodeName == "materials" )
		{
			//processMaterials( node, materialNames );
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
string changeToWorkingDir( string filepath )
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
	if ( !root || root->ValueStr() != "conversion" )
	{
		cout << "[Error] This is not a valid QuakeToOgre configuration file" << endl;
		return false;
	}
	
	bool convertCoordinates = root->FirstChildElement( "convertcoordinates" ) ? true : false;
	bool success = false;
	
	for ( TiXmlElement *node = root->FirstChildElement(); node; node = node->NextSiblingElement() )
	{			
		const string &nodeName = node->ValueStr();
		if ( nodeName == "md2mesh" )
		{
			success = convertMD2Mesh( node, convertCoordinates );
		}
		else if ( nodeName == "md3mesh" )
		{
			success = convertMD3Mesh( node, convertCoordinates );
		}
		else if ( nodeName == "md5mesh" )
		{
			success = convertMD5Mesh( node, convertCoordinates );
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
		
		MD2Structure md2Struct;
		MD3Structure md3Struct;
		if ( md2Struct.load( argv[2] ) )
		{
			md2Struct.printInfo();
			return 0;
		}
		else if ( md3Struct.load( argv[2] ) )
		{
			md3Struct.printInfo();
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
