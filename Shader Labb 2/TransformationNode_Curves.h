#ifndef N3_TRANSFORMATIONNODE_CURVES_HEADER
#define N3_TRANSFORMATIONNODE_CURVES_HEADER

#include "TransformationNode.h"
#include "AnimationCurve.h"
#include "CU_GrowingArray.h"
#include "cu_vector.h"
#include "AnimationEnums.h"

//#include "enums.h"


struct CurveAnimationFrame
{
	CommonUtilities::Vector3f myDirection;
	CommonUtilities::Vector3f myTranslation;
	float myAngle;
};

class TransformationNode_Curves : public TransformationNode
{
public:
	TransformationNode_Curves();
	~TransformationNode_Curves();

	void Init(AnimationType aAnimationType,int aNumberOfKeyFrames,int aAnimationFPS,float aStartTime,float aEndTime);
	
	const Matrix44f GetTransformationForTime( float aTime );
	
	void SetBaseFrame(CommonUtilities::Vector3f myDirection,CommonUtilities::Vector3f aTranslation,float aAngle);
	void SetRotationCurve(int i,AnimationCurve* aCurve);
	void SetTranslationCurve(int i,AnimationCurve* aCurve);

protected:
	CurveAnimationFrame GetFrameForTime(double aElapsedTime);
	
	int myAnimationFPS;
	float myStartTime;
	float myEndTime;
	AnimationType myAnimationType;
	AnimationCurve* myRotationCurves[4];
	AnimationCurve* myTranslationCurves[3];
	CurveAnimationFrame myBaseFrame;
};


#endif