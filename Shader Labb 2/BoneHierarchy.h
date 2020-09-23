#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include "TransformationNode.h"
#include "CU_GrowingArray.h"
#include "debugHeader.h"

class BoneHierarchy
{
public:
	BoneHierarchy(void);
	~BoneHierarchy(void);

	void AddTransformationNode(TransformationNode* aTransformationNode);
	CommonUtilities::GrowingArray<TransformationNode*>& GetTransformationNodes();
	void Init(int aNbrOfExpectedBone);

private:
	CommonUtilities::GrowingArray<TransformationNode*> myTransformationNodes;
};

#endif