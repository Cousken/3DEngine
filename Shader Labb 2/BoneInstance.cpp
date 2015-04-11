#include "BoneInstance.h"

BoneInstance::BoneInstance(void)
{
}

BoneInstance::~BoneInstance(void)
{
}

void BoneInstance::Transform( const Matrix44f& aParentMatrix )
{
	myTransformedMatrix=GetBoneLocalTransform();
	myTransformedMatrix*=aParentMatrix;
}

void BoneInstance::ApplyBindPose()
{
	Matrix44f test1=myBone->GetInversedBindPose();
	test1*=myTransformedMatrix;
	myTransformedMatrix=test1;
}

const Matrix44f BoneInstance::GetBoneLocalTransform()
{
	if(myTransformationInstance->GetTransformationNode())
	{
		Matrix44f trans;
		trans=myTransformationInstance->GetTransformation();
		return(trans);
	}
	else
	{
		return(myBone->myOriginalBindPoseOrientation);
	}
}

const Matrix44f BoneInstance::GetTransformed()
{
	return myTransformedMatrix;
}

void BoneInstance::SetBoneAnimationInstance( TransformationNodeInstance* aTransformationInstance )
{
	myTransformationInstance = aTransformationInstance;
}

void BoneInstance::SetBone( Bone* aBone )
{
	myBone = aBone;
}
