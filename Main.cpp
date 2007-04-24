#include "Common.h"
#include "MD3Structure.h"
#include "MD3XmlWriter.h"
#include "Q3ModelToMesh.h"
#include "Animation.h"

using namespace std;

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
