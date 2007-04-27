#include "Common.h"
#include "MD3Structure.h"
#include "Q3ModelToMesh.h"
#include "Animation.h"

using namespace std;

bool processAnimationFile( TiXmlHandle animFileHandle, AnimationList &dest )
{
	cout << "Processing animation file" << endl;

	TiXmlElement *filenameNode = animFileHandle.FirstChild( "filename" ).ToElement();
	if ( !filenameNode )
	{
		cout << "[Warning] Animation file declaration misses filename" << endl;
		return false;
	}

	string animFile = filenameNode->GetText();
	AnimationLoader animLoader;
	if ( !animLoader.load( animFile ) )
	{
		cout << "[Warning] Could not load animation file '" << animFile << "'" << endl;
		return false;
	}

	TiXmlElement *groupNode = animFileHandle.FirstChild( "animationgroup" ).FirstChild().ToElement();
	if ( !groupNode )
	{
		cout << "[Warning] Animation file declaration misses animation group" << endl;
		return false;
	}

	const AnimationMap *loadedAnims = NULL;
	const string &group = groupNode->ValueStr();
	if ( group == "grouplower" )
		loadedAnims = &animLoader.getLowerAnimations();
	else if ( group == "groupupper" )
		loadedAnims = &animLoader.getUpperAnimations();

	if ( !loadedAnims )
	{
		cout << "[Warning] Invalid animation group '" << group << "'" << endl;
		return false;
	}
	
	// Either convert all animations, or a selection
	if ( animFileHandle.FirstChild( "convertall" ).ToElement() )
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
		cout << "Converting individual animations" << endl;

		TiXmlHandle childHandle = animFileHandle.FirstChildElement( "animationname" );
		for ( TiXmlElement *child = childHandle.ToElement(); child; 
			child = child->NextSiblingElement( "animationname" ) )
		{
			string animName = child->GetText();
			AnimationMap::const_iterator i = loadedAnims->find( animName );
			if ( i != loadedAnims->end() )
				dest.push_back( i->second );
			else
				cout << "[Warning] Cannot find animation '" << animName << "'" << endl;
		}
	}

	return true;
}

bool processAnimations( TiXmlHandle animsHandle, AnimationList &dest )
{
	// TODO
	return false;
}

bool convertMD2Mesh( TiXmlHandle configHandle, bool convertCoordinates )
{
	cout << "Doing MD2 Mesh conversion" << endl;
			
	return false;
}

bool convertMD3Mesh( TiXmlHandle configHandle, bool convertCoordinates )
{
	cout << "Doing MD3 Mesh conversion" << endl;
	
	// Find input and output file names
	TiXmlElement *inputFileNode = configHandle.FirstChild( "inputfile" ).FirstChild( "filename" ).ToElement();
	if ( !inputFileNode )
	{
		cout << "[Error] No input file defined" << endl;
		return false;
	}
	TiXmlElement *outputFileNode = configHandle.FirstChild( "outputfile" ).FirstChild( "filename" ).ToElement();
	if ( !outputFileNode )
	{
		cout << "[Error] No output file defined" << endl;
		return false;
	}
	
	string inputFile = inputFileNode->GetText();
	string outputFile = outputFileNode->GetText();
	
	// Try to load the MD3 file
	MD3Structure md3Struct;
	if ( !md3Struct.load( inputFile ) )
	{
		cout << "[Error] Could not load input file '" << inputFile << "'" << endl;
		return false;
	}
	
	// Process animations
	AnimationList animList;
	
	// Try to load animations from an animation file
	TiXmlHandle animFileHandle = configHandle.FirstChild( "animationfile" );
	if ( animFileHandle.ToElement() )
	{
		if ( !processAnimationFile( animFileHandle, animList ) )
		{
			cout << "[Warning] Failed to process animation file" << endl;
		}
	}
	
	// Process animation definitions
	TiXmlHandle animsHandle = configHandle.FirstChild( "animations" );
	if ( animsHandle.ToElement() )
	{
		processAnimations( animsHandle, animList );
	}
	
	// TODO process materials

	Q3ModelToMesh converter( md3Struct, animList, 0, convertCoordinates );
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
	
	const TiXmlElement *root = config.RootElement();
	if ( !root || root->ValueStr() != "conversion" )
	{
		cout << "[Error] This is not a valid QuakeToOgre configuration file" << endl;
		return 1;
	}
	
	bool convertCoordinates = false;
	bool (*conversionFunc)( TiXmlHandle, bool ) = NULL;
	const TiXmlElement *configNode = NULL;
	
	for ( const TiXmlNode *child = root->FirstChild(); child; child = child->NextSibling() )
	{
		const TiXmlElement *node = child->ToElement();
		if ( !node )
			continue;
			
		const string &name = node->ValueStr();
		if ( name == "convertcoordinates" )
		{
			convertCoordinates = true;
		}
		else if ( name == "md2mesh" )
		{
			conversionFunc = &convertMD2Mesh;
			configNode = node;
		}
		else if ( name == "md3mesh" )
		{
			conversionFunc = &convertMD3Mesh;
			configNode = node;
		}
	}

	if ( !conversionFunc || !configNode )
	{
		cout << "[Error] No conversion method found" << endl;
		return 1;
	}

	// Casting away const is evil, but it is necessary if we want to use TiXmlHandles
	TiXmlHandle configHandle( const_cast<TiXmlElement*>(configNode) );

	if ( conversionFunc( configHandle, convertCoordinates ) )
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

#if 0
int main( int argc, char **argv )
{
	if ( argc < 4 )
	{
		printf( "Usage: QuakeToOgre [infile] [animation file] [outfile]\n" );
		return 1;
	}

	char *infile = argv[1];
	char *animfile = argv[2];
	char *outfile = argv[3];

	MD3Structure md3Struct;
	if ( !md3Struct.load( infile ) )
	{
		printf( "Failed to load MD3 model from file '%s'\n", infile );
		return 1;
	}

	AnimationLoader animLoader;
	if ( !animLoader.load( animfile ) )
	{
		printf( "Failed to load animation definitions from file '%s'\n", animfile );
		return 1;
	}

	FILE *f;
	f = fopen( "md3.xml", "w" );
	if ( f )
	{
		string str = MD3XmlWriter( md3Struct );
		fputs( str.c_str(), f );
		fclose( f );
	}

	f = fopen( outfile, "w" );
	if ( f )
	{
//		AnimationList animations;
//		animations.push_back( Animation( "walk", 98, 12, 20 ) );		// visor walk
//		animations.push_back( Animation( "idle", 1666, 990, 35 ) );		// dragon idle
//		animations.push_back( Animation( "idlecr", 2656, 495, 30 ) );	// dragon idle crouched
		
		const AnimationList &animList = animLoader.getLowerAnimations();

		string str = Q3ModelToMesh( md3Struct, animList, 0, true );
		fputs( str.c_str(), f );
		fclose( f );
	}

	return 0;
}
#endif
