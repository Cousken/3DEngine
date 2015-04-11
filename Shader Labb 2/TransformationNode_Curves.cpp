#include "stdafx.h"
#include "TransformationNode_Curves.h"

TransformationNode_Curves::TransformationNode_Curves()
{
	for(int i=0;i<4;i++) myRotationCurves[i]=NULL;
	for(int i=0;i<3;i++) myTranslationCurves[i]=NULL;
}

TransformationNode_Curves::~TransformationNode_Curves()
{
	for(int i=0;i<4;i++) delete(myRotationCurves[i]);
	for(int i=0;i<3;i++) delete(myTranslationCurves[i]);
}
		
void TransformationNode_Curves::Init(AnimationType aAnimationType,int aNumberOfKeyFrames,int aAnimationFPS,float aStartTime,float aEndTime)
{
	myAnimationFPS=aAnimationFPS;
	aAnimationType=myAnimationType;
	myStartTime=aStartTime;
	myEndTime=aEndTime;

}

const Matrix44f TransformationNode_Curves::GetTransformationForTime( float aTime )
{
	CurveAnimationFrame keyFrame=GetFrameForTime(aTime);
	Matrix44f temp;

	temp=myOrientation;
	temp = temp * Matrix33f::RotationAroundVector(keyFrame.myDirection.x, keyFrame.myDirection.y, keyFrame.myDirection.z ,keyFrame.myAngle);
	temp.myData[(3 * 4) + 0]=keyFrame.myTranslation.x;
	temp.myData[(3 * 4) + 1]=keyFrame.myTranslation.y;
	temp.myData[(3 * 4) + 2]=keyFrame.myTranslation.z;
	//	temp*=myOrientation;

	return(temp);
}



void TransformationNode_Curves::SetRotationCurve(int i,AnimationCurve* aCurve)
{
	myRotationCurves[i]=aCurve;
}

void TransformationNode_Curves::SetTranslationCurve(int i,AnimationCurve* aCurve)
{
	myTranslationCurves[i]=aCurve;
}

CurveAnimationFrame TransformationNode_Curves::GetFrameForTime(double aElapsedTime)
{ 

	if(myRotationCurves[0]!=NULL)	myBaseFrame.myDirection.x=myRotationCurves[0]->GetValueFromTime(aElapsedTime);
	if(myRotationCurves[1]!=NULL)	myBaseFrame.myDirection.y=myRotationCurves[1]->GetValueFromTime(aElapsedTime);
	if(myRotationCurves[2]!=NULL)	myBaseFrame.myDirection.z=myRotationCurves[2]->GetValueFromTime(aElapsedTime);
	if(myRotationCurves[3]!=NULL)	myBaseFrame.myAngle=myRotationCurves[3]->GetValueFromTime(aElapsedTime);
	if(myTranslationCurves[0]!=NULL)	myBaseFrame.myTranslation.x=myTranslationCurves[0]->GetValueFromTime(aElapsedTime);
	if(myTranslationCurves[1]!=NULL)	myBaseFrame.myTranslation.y=myTranslationCurves[1]->GetValueFromTime(aElapsedTime);
	if(myTranslationCurves[2]!=NULL)	myBaseFrame.myTranslation.z=myTranslationCurves[2]->GetValueFromTime(aElapsedTime);

	return(myBaseFrame);
}

void TransformationNode_Curves::SetBaseFrame(CU::Vector3f myDirection,CU::Vector3f aTranslation,float aAngle)
{
	myBaseFrame.myDirection=myDirection;
	myBaseFrame.myTranslation=aTranslation;
	myBaseFrame.myAngle=aAngle;
}
/*
void TransformationNode_Curves::SetBaseFrame(CU::Matrix44f& aMatrix)
{
//	myBaseFrame.myDirection=
	myBaseFrame.myTranslation.x=aMatrix.myMatrix[3][0];
	myBaseFrame.myTranslation.y=aMatrix.myMatrix[3][0];
	myBaseFrame.myTranslation.z=aMatrix.myMatrix[3][0];
}
*/