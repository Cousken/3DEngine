#ifndef _ANIMATION_INSTANCE_H_
#define _ANIMATION_INSTANCE_H_

#include "BoneHierarchy.h"
#include "TransformationNodeInstance.h"
#include "debugHeader.h"

class BoneInstanceHierarchy
{
public:
	BoneInstanceHierarchy(BoneHierarchy* aAnimation);
	~BoneInstanceHierarchy(void);

	const CommonUtilities::GrowingArray<TransformationNodeInstance*>& GetTransformationAnimations();
	void Update( float aElapsedTime );

private:
	BoneHierarchy* myAnimation;
	double myElapsedTime;
	CommonUtilities::GrowingArray<TransformationNodeInstance*> myTransformationInstances;
};

#endif