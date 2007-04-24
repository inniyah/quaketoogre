#include "Common.h"
#include "Q3ModelToSkeleton.h"

Q3ModelToSkeleton::Q3ModelToSkeleton( 
	const MD3Structure &lower, 
	const MD3Structure &upper, 
	const MD3Structure &head,
	const AnimationMap &animMap ):
	
	mLower(lower),
	mUpper(upper), 
	mHead(head), 
	mAnimationMap(animMap)
{
	convert();
}

void Q3ModelToSkeleton::convert()
{
	openTag( "skeleton" );
	
	closeTag();
}
