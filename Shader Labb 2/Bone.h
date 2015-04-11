#ifndef _BONE_H_
#define _BONE_H_

#include "CU_GrowingArray.h"
#include "Matrix.h"

class Bone
{
	friend class BoneInstance;

public:
	Bone(void);
	~Bone(void);
	
	void AddChild(int aChildIndex);
	void TransformBindPose(const Matrix44f& aMatrix);

	const Matrix44f& GetInversedBindPose();
	const Matrix44f& GetBindPose();
	const CommonUtilities::GrowingArray<int>& GetChildren();

	void SetBindPose(const Matrix44f& aMatrix);	

private:
	CommonUtilities::GrowingArray<int> myChildObjects;
	Matrix44f myInverseTransformedBindPoseOrientation;
	Matrix44f myOriginalBindPoseOrientation;
	Matrix44f myTransformedBindPoseOrientation;


};

#endif