#include "Common.h"
#include "MD3Structure.h"
#include "Q3ModelToMesh.h"
#include "Animation.h"

using namespace std;

bool processAnimationFile( TiXmlElement *animFileNode, AnimationList &dest )
{
	cout << "Processing animation file" << endl;

	TiXmlElement *filenameNode = animFileNode->FirstChildElement( "filename" );
	if ( !filenameNode )
	{
		cout << "[Warning] Animation file declaration misses filename" << endl;
		return false;
	}

	string animFilename = filenameNode->GetText();
	AnimationFile animFile;
	if ( !animFile.load( animFilename ) )
	{
		cout << "[Warning] Could not load animation file '" << animFilename << "'" << endl;
		return false;
	}

	// FIXME doesn't check if animationgroup node exists
	TiXmlElement *groupNode = animFileNode->FirstChildElement( "animationgroup" )->FirstChildElement();
	if ( !groupNode )
	{
		cout << "[Warning] Animation file declaration misses animation group" << endl;
		return false;
	}

	const AnimationMap *loadedAnims = NULL;
	const string &group = groupNode->ValueStr();
	if ( group == "grouplower" )
		loadedAnims = &animFile.getLowerAnimations();
	else if ( group == "groupupper" )
		loadedAnims = &animFile.getUpperAnimations();

	if ( !loadedAnims )
	{
		cout << "[Warning] Invalid animation group '" << group << "'" << endl;
		return false;
	}
	
	// Either convert all animations, or a selection
	if ( animFileNode->FirstChildElement( "convertall" ) )
	{
		cout << "Converting all animations from animation file" << endl;
		
		for ( AnimationMap::const_iterator i = loadedAnims->begin();
			i != loadedAnims->end(); ++i )
		{
			dest.push_back( i->second );
		}
	}
	else
	{
		cout << "Converting a selection of animations from animation file" << endl;

		for ( TiXmlElement *child = animFileNode->FirstChildElement(); child; child = child->NextSiblingElement() )
		{
			const string &nodeName = child->ValueStr();
			if ( nodeName == "animationname" )
			{
				string animName = child->GetText();
				AnimationMap::const_iterator i = loadedAnims->find( animName );
				if ( i != loadedAnims->end() )
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

bool convertMD2Mesh( TiXmlElement *configNode, bool convertCoordinates )
{
	cout << "Doing MD2 Mesh conversion" << endl;
			
	return false;
}

bool convertMD3Mesh( TiXmlElement *configNode, bool convertCoordinates )
{
	cout << "Doing MD3 Mesh conversion" << endl;
	
	string inputFile, outputFile;
	int referenceFrame = 0;
	AnimationList animList;
	
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
			// TODO
		}
	}
	
	// Try to load the MD3 file
	MD3Structure md3Struct;
	if ( !md3Struct.load( inputFile ) )
	{
		cout << "[Error] Could not load input file '" << inputFile << "'" << endl;
		return false;
	}
	
	Q3ModelToMesh converter( md3Struct, animList, referenceFrame, convertCoordinates );
	if ( !converter.saveFile( outputFile ) )
	{
		cout << "[Error] Could not save to file '" << outputFile << "'" << endl;
		return false;
	}
	
	return true;
}

int main( int argc, char **argv )
{
	if ( argc < 2 )
	{
		cout << "Usage: QuakeToOgre [config file]" << endl;
		return 1;
	}
	
	TiXmlDocument config( argv[1] );
	if ( !config.LoadFile() )
	{
		cout << "[Error] Could not load configuration from file '" << argv[1] << "', reason:" 
			<< endl << "Error " << config.ErrorId() << " on row " << config.ErrorRow() 
			<< " column " << config.ErrorCol() << ":" << endl << config.ErrorDesc();
		return 1;
	}
	
	// Yes, casting away const is evil, but it's convenient in this case
	TiXmlElement *root = const_cast<TiXmlElement*>(config.RootElement());
	if ( !root || root->ValueStr() != "conversion" )
	{
		cout << "[Error] This is not a valid QuakeToOgre configuration file" << endl;
		return 1;
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
	}
	
	if ( success )
	{
		cout << "Conversion succeeded!" << endl;
		return 0;
	}
	else
	{
		cout << "Conversion failed..." << endl;
		return 1;
	}
}
