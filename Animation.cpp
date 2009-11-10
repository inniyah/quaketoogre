/*
-------------------------------------------------------------------------------
Copyright (c) 2009 Nico de Poel

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
#include "Animation.h"

#define PM_ANIMATION_COUNT	25

static const char *pmAnimationNames[PM_ANIMATION_COUNT] =
{
	"BOTH_DEATH1", "BOTH_DEAD1", "BOTH_DEATH2", "BOTH_DEAD2", "BOTH_DEATH3", 
	"BOTH_DEAD3", "TORSO_GESTURE", "TORSO_ATTACK", "TORSO_ATTACK2", "TORSO_DROP", 
	"TORSO_RAISE", "TORSO_STAND", "TORSO_STAND2", "LEGS_WALKCR", "LEGS_WALK", 
	"LEGS_RUN", "LEGS_BACK", "LEGS_SWIM", "LEGS_JUMP", "LEGS_LAND",
	"LEGS_JUMPB", "LEGS_LANDB", "LEGS_IDLE", "LEGS_IDLECR", "LEGS_TURN"
};
static const int pmTorsoStart = 6;
static const int pmLegsStart = 13;

bool AnimationFile::load( const string &filename )
{
	FILE *f = fopen( filename.c_str(), "r" );
	if ( !f )
		return false;

	char line[256], token[16];
	const char *linePtr;
	int animationIndex = 0;
	int numBothFrames = 0, numTorsoFrames = 0;

	while ( !feof( f ) )
	{
		linePtr = fgets( line, 256, f );
		if ( !linePtr )
			break;

		linePtr = getNextToken( linePtr, token, 16 );
		if ( !*token )	// empty line
			continue;
		if ( !isdigit( *token ) )	// non-numeric characters
			continue;
		if ( token[0] == '/' && token[1] == '/' )	// comment
			continue;
		if ( !strcmp( token, "sex" ) )	// gender info
			continue;
		if ( !strcmp( token, "headoffset" ) )	// head offset info
			continue;
		if ( !strcmp( token, "footsteps" ) )	// footstep sound info
			continue;

		if ( animationIndex >= PM_ANIMATION_COUNT )
		{
			// With proper animation files we shouldn't get here
			// Let's just give off a warning
			cout << "[Warning] Too many animations in animation file, "
				<< "the resulting animation list may not be correct." << endl;
			break;
		}

		string animName = pmAnimationNames[animationIndex];

		AnimationInfo anim;
		anim.startFrame = atoi( token );

		linePtr = getNextToken( linePtr, token, 16 );
		anim.numFrames = atoi( token );

		linePtr = getNextToken( linePtr, token, 16 );	// looping frames, not interesting
		linePtr = getNextToken( linePtr, token, 16 );
		anim.framesPerSecond = atoi( token );

		if ( animationIndex < pmTorsoStart )
		{
			// BOTH_ animation
			numBothFrames = anim.startFrame + anim.numFrames;
			mLowerAnimations[animName] = anim;
			mUpperAnimations[animName] = anim;
		}
		else if ( animationIndex < pmLegsStart )
		{
			// TORSO_ animation
			numTorsoFrames = anim.startFrame + anim.numFrames;
			mUpperAnimations[animName] = anim;
		}
		else
		{
			// LEGS_ animation
			anim.startFrame -= (numTorsoFrames - numBothFrames);
			mLowerAnimations[animName] = anim;
		}
		
		mAnimations[animName] = anim;
		animationIndex++;
	}

	fclose( f );
	return true;
}

const char *AnimationFile::getNextToken( const char *src, char *dest, int destLen )
{
	while ( *src && isspace( *src ) )
		*src++;
	while ( destLen-- && *src && !isspace( *src ) )
		*dest++ = *src++;
	*dest = '\0';
	return src;
}
