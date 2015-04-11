#include "BoneHierarchy.h"

BoneHierarchy::BoneHierarchy(void)
{
}

BoneHierarchy::~BoneHierarchy(void)
{
}

CommonUtilities::GrowingArray<TransformationNode*>& BoneHierarchy::GetTransformationNodes()
{
	return myTransformationNodes;
}

void BoneHierarchy::AddTransformationNode( TransformationNode* aTransformationNode )
{
	myTransformationNodes.Add( aTransformationNode );
}

void BoneHierarchy::Init( int aNbrOfExpectedBone )
{
	myTransformationNodes.Init(aNbrOfExpectedBone, 10);
}
