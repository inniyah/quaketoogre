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
#ifndef __ANIMATION_H__
#define __ANIMATION_H__

class AnimationInfo
{
public:
	AnimationInfo(): startFrame(0), numFrames(0), framesPerSecond(10) {}
	AnimationInfo( int startFrame, int numFrames, int fps ):
		startFrame(startFrame), numFrames(numFrames), framesPerSecond(fps) {}
	
	int startFrame;
	int numFrames;
	int framesPerSecond;
};

typedef map<string, AnimationInfo> AnimationMap;

class AnimationFile
{
public:
	bool load( const string &filename );

	const AnimationMap &getAnimations() const { return mAnimations; }
	const AnimationMap &getLowerAnimations() const { return mLowerAnimations; }
	const AnimationMap &getUpperAnimations() const { return mUpperAnimations; }

private:
	const char *getNextToken( const char *src, char *dest, int destLen );

	AnimationMap mAnimations;
	AnimationMap mLowerAnimations;
	AnimationMap mUpperAnimations;
};

#endif
