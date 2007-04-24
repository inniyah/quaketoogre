#ifndef __Q3MODELTOSKELETON_H__
#define __Q3MODELTOSKELETON_H__

#include "XmlWriter.h"
#include "MD3Structure.h"
#include "Animation.h"

class Q3ModelToSkeleton: public XmlWriter
{
public:
	Q3ModelToSkeleton( 
		const MD3Structure &lower, 
		const MD3Structure &upper, 
		const MD3Structure &head,
		const AnimationMap &animMap );

private:
	void convert();

	const MD3Structure &mLower;
	const MD3Structure &mUpper;
	const MD3Structure &mHead;
	const AnimationMap &mAnimationMap;
};

#endif
