#include "stdafx.h"
#include "TransformationNode_KeyFrames.h"

TransformationNode_KeyFrames::TransformationNode_KeyFrames()
{
}

TransformationNode_KeyFrames::~TransformationNode_KeyFrames()
{
}

void TransformationNode_KeyFrames::Init(AnimationType aAnimationType,int aNumberOfKeyFrames,int aAnimationFPS)
{
	myAnimationFPS=aAnimationFPS;
	aAnimationType=myAnimationType;
	myKeyFrames.Init(aNumberOfKeyFrames,10);
	myStartTime=0;
	myEndTime=static_cast<float>(aNumberOfKeyFrames)/static_cast<float>(aAnimationFPS);
}

void TransformationNode_KeyFrames::AddAnimationFrame(const AnimationFrame* aKeyFrame)
{
		myKeyFrames.Add(*aKeyFrame);
}

const Matrix44f TransformationNode_KeyFrames::GetTransformationForTime( float aTime )
{
	AnimationFrame keyFrame=GetFrameForTime(aTime);

	D3DXMATRIX translation;
	D3DXMATRIX orientation;
	D3DXMATRIX localTransform;
	D3DXMatrixTranslation( &translation, keyFrame.myTranslation.x, keyFrame.myTranslation.y, keyFrame.myTranslation.z );
	D3DXMatrixRotationQuaternion( &orientation, &keyFrame.myQuaternion );
	localTransform = ( orientation * translation );
	Matrix44f temp;
	temp.ConvertD3D(reinterpret_cast<float*>(&localTransform));
	return(temp);
}


AnimationFrame& TransformationNode_KeyFrames::GetFrameForTime(double aElapsedTime)
{
	int frame = static_cast<int>(myAnimationFPS * aElapsedTime );

    frame = frame % (myKeyFrames.Count() - 1 );
    frame++;
	return(myKeyFrames[frame]);
}

		
