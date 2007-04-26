#include "Common.h"
#include "MD3Structure.h"
#include "Q3ModelToMesh.h"
#include "Animation.h"

using namespace std;

bool convertMD2Mesh( TiXmlElement *configNode, bool convertCoordinates )
{
	printf( "Doing MD2 Mesh conversion\n" );
			
	return false;
}

bool convertMD3Mesh( TiXmlElement *configNode, bool convertCoordinates )
{
	TiXmlHandle configHandle( configNode );
	printf( "Doing MD3 Mesh conversion\n" );
	
	// Find input and output file names
	TiXmlElement *inputFileNode = configHandle.FirstChild( "inputfile" ).FirstChild( "filename" ).ToElement();
	if ( !inputFileNode )
	{
		printf( "Error: No input file defined\n" );
		return false;
	}
	TiXmlElement *outputFileNode = configHandle.FirstChild( "outputfile" ).FirstChild( "filename" ).ToElement();
	if ( !outputFileNode )
	{
		printf( "Error: No output file defined\n" );
		return false;
	}
	
	const char *inputFile = inputFileNode->GetText();
	const char *outputFile = outputFileNode->GetText();
	
	// Try to load the MD3 file
	MD3Structure md3Struct;
	if ( !md3Struct.load( inputFile ) )
	{
		printf( "Error: Could not load input file '%s'\n", inputFile );
		return false;
	}
	
	// Process animations
	AnimationList animList;
	
	// Try to load animations from an animation file
	TiXmlHandle animFileHandle = configHandle.FirstChild( "animationfile" );
	TiXmlHandle animFilenameHandle = animFileHandle.FirstChild( "filename" );
	TiXmlHandle animGroupHandle = animFileHandle.FirstChild( "animationgroup" );
	if ( animFileHandle.ToElement() && animFilenameHandle.ToElement() && animGroupHandle.ToElement() )
	{
		const char *animFile = animFilenameHandle.ToElement()->GetText();
		
		AnimationLoader animLoader;
		if ( animLoader.load( animFile ) )
		{
			const AnimationMap &(AnimationLoader::*retrievalFunc)() = NULL;
			TiXmlElement *animGroupNode = animGroupHandle.FirstChild().ToElement();
			if ( animGroupNode )
			{
				const string &value = animGroupNode->ValueStr();
				if ( value == "grouplower" )
					retrievalFunc = &AnimationLoader::getLowerAnimations;
				else if ( value == "groupupper" )
					retrievalFunc = &AnimationLoader::getUpperAnimations;
			}
			
			if ( retrievalFunc )
			{
				const AnimationMap &loadedAnims = (animLoader.*retrievalFunc)();
				if ( animFileHandle.FirstChild( "convertall" ).ToElement() )
				{
					printf( "Converting all animations from animation file\n" );
					
					for ( AnimationMap::const_iterator i = loadedAnims.begin();
						i != loadedAnims.end(); ++i )
					{
						animList.push_back( i->second );
					}
				}
				else
				{
					// TODO
					printf( "Converting individual animations\n" );
				}
			}
			else
			{
				printf( "Warning: No animation group given for animation file\n" );
			}
		}
		else
		{
			printf( "Warning: Could not load animation file '%s'\n", animFile );
		}
	}
	
	// TODO process animation defs
	
	// TODO process materials
	
	FILE *f = fopen( outputFile, "w" );
	if ( !f )
	{
		printf( "Error: Could not write to file '%s'\n", outputFile );
		return false;
	}
	
	string str = Q3ModelToMesh( md3Struct, animList, 0, true );
	fputs( str.c_str(), f );
	fclose( f );
		
	return true;
}

int main( int argc, char **argv )
{
	if ( argc < 2 )
	{
		printf( "Usage: QuakeToOgre [config file]\n" );
		return 1;
	}
	
	TiXmlDocument config( argv[1] );
	if ( !config.LoadFile() )
	{
		printf( "Error: Could not load configuration from file '%s', reason:\n", argv[1] );
		printf( "Error %i on row %i column %i:\n%s\n", config.ErrorId(), 
			config.ErrorRow(), config.ErrorCol(), config.ErrorDesc() );
		return 1;
	}
	
	const TiXmlElement *root = config.RootElement();
	if ( !root || root->ValueStr() != "conversion" )
	{
		printf( "Error: This is not a valid QuakeToOgre configuration file\n" );
		return 1;
	}
	
	bool convertCoordinates = false;
	bool (*conversionFunc)( TiXmlElement *, bool ) = NULL;
	const TiXmlElement *conversionNode = NULL;
	
	for ( const TiXmlNode *child = root->FirstChild(); child; child = child->NextSibling() )
	{
		const TiXmlElement *node = child->ToElement();
		if ( !node )
			continue;
			
		const string &name = node->ValueStr();
		if ( name == "convertcoordinates" )
			convertCoordinates = true;	
		else if ( name == "md2mesh" )
		{
			conversionFunc = &convertMD2Mesh;
			conversionNode = node;
		}
		else if ( name == "md3mesh" )
		{
			conversionFunc = &convertMD3Mesh;
			conversionNode = node;
		}
	}

	if ( !conversionFunc || !conversionNode )
	{
		printf( "Error: No conversion method found\n" );
		return 1;
	}

	// Casting away const is evil, but it is necessary if we want to use TiXmlHandles
	if ( conversionFunc( const_cast<TiXmlElement*>(conversionNode), convertCoordinates ) )
	{
		printf( "Conversion succeeded!\n" );
		return 0;
	}
	else
	{
		printf( "Conversion failed...\n" );
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
