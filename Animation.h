#ifndef __ANIMATION_H__
#define __ANIMATION_H__

class Animation
{
public:
	Animation(): name(""), startFrame(0), numFrames(0), framesPerSecond(10) {}
	Animation( const string &name, int startFrame, int numFrames, int fps ):
		name(name), startFrame(startFrame), numFrames(numFrames), framesPerSecond(fps) {}
	
	string name;
	int startFrame;
	int numFrames;
	int framesPerSecond;
};

typedef vector<Animation> AnimationList;
typedef map<string, Animation> AnimationMap;

class AnimationFile
{
public:
	bool load( const string &filename );

	const AnimationMap &getLowerAnimations() { return mLowerAnimations; }
	const AnimationMap &getUpperAnimations() { return mUpperAnimations; }

private:
	const char *getNextToken( const char *src, char *dest, int destLen );

	AnimationMap mLowerAnimations;
	AnimationMap mUpperAnimations;
};

#endif
