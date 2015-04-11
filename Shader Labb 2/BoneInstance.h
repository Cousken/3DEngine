#ifndef _BONE_INSTANCE_H_
#define _BONE_INSTANCE_H_

#include "Matrix.h"
#include "Bone.h"
#include "TransformationNodeInstance.h"

class BoneInstance
{
public:
	BoneInstance(void);
	~BoneInstance(void);

	void ApplyBindPose();
	const Matrix44f GetBoneLocalTransform();
	const Matrix44f GetTransformed();
	void SetBone(Bone* aBone);
	void SetBoneAnimationInstance( TransformationNodeInstance* aTransformationInstance );
	void Transform( const Matrix44f& aParentMatrix );

private:
	Bone* myBone;
	TransformationNodeInstance* myTransformationInstance;
	Matrix44f myTransformedMatrix;

};

#endif