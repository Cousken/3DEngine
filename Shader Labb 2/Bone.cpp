#include "Bone.h"
#include <D3DX10.h>

Bone::Bone(void)
{
	myChildObjects.Init(10,10);
}

Bone::~Bone(void)
{
}

void Bone::SetBindPose( const Matrix44f& aMatrix )
{
	myOriginalBindPoseOrientation=aMatrix;
	myTransformedBindPoseOrientation=aMatrix;
	D3DXMATRIX* orgMatrix=reinterpret_cast<D3DXMATRIX*>(&myTransformedBindPoseOrientation);
	D3DXMATRIX* invMatrix=reinterpret_cast<D3DXMATRIX*>(&myInverseTransformedBindPoseOrientation);
	D3DXMatrixInverse(invMatrix,NULL,orgMatrix);
}

void Bone::AddChild( int aChildIndex )
{
	myChildObjects.Add( aChildIndex );
}

void Bone::TransformBindPose( const Matrix44f& aMatrix )
{
	myTransformedBindPoseOrientation=myOriginalBindPoseOrientation*aMatrix;
	D3DXMATRIX* orgMatrix=reinterpret_cast<D3DXMATRIX*>(&myTransformedBindPoseOrientation);
	D3DXMATRIX* invMatrix=reinterpret_cast<D3DXMATRIX*>(&myInverseTransformedBindPoseOrientation);
	D3DXMatrixInverse(invMatrix,NULL,orgMatrix);
}

const Matrix44f& Bone::GetBindPose()
{
	return myTransformedBindPoseOrientation;
}

const CommonUtilities::GrowingArray<int>& Bone::GetChildren()
{
	return myChildObjects;
}

const Matrix44f& Bone::GetInversedBindPose()
{
 return myInverseTransformedBindPoseOrientation;
}
