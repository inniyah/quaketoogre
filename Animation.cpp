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

Animation::Animation():
	name(""), startFrame(0), numFrames(0), framesPerSecond(10)
{
}

Animation::Animation( const string &name, int startFrame, int numFrames, int fps ):
	name(name), startFrame(startFrame), numFrames(numFrames), framesPerSecond(fps)
{
}
