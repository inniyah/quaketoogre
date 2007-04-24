#ifndef __ANIMATION_H__
#define __ANIMATION_H__

class Animation
{
public:
	Animation();
	Animation( const string &name, int startFrame, int numFrames, int fps );
	
	string name;
	int startFrame;
	int numFrames;
	int framesPerSecond;
};

// Load function goes into AnimationList/Map

typedef vector<Animation> AnimationList;
typedef map<string, Animation> AnimationMap;

#endif
