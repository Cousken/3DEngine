#ifndef N3_TRANSFORMATIONNODE_KEYFRAMES_HEADER
#define N3_TRANSFORMATIONNODE_KEYFRAMES_HEADER

//#include "namespaces.h"
#include "transformationnode.h"
#include "cu_Vector.h"
#include <d3dx10.h>
//#include "n3_enums.h"
#include "CU_GrowingArray.h"
#include "AnimationEnums.h"


struct AnimationFrame
{
	CU::Vector3f myTranslation;
	CU::Vector4f myOrientation;
	CU::Vector3f myScaling;
	D3DXQUATERNION myQuaternion;
};


class TransformationNode_KeyFrames : public TransformationNode
{
public:
	TransformationNode_KeyFrames();
	~TransformationNode_KeyFrames();

	void Init(AnimationType aAnimationType,int aNumberOfKeyFrames,int aAnimationFPS);
	void AddAnimationFrame(const AnimationFrame* aKeyFrame);


	const Matrix44f GetTransformationForTime( float aTime );
	

protected:
	AnimationFrame& GetFrameForTime(double aElapsedTime);
	
	float myStartTime;
	float myEndTime;
	int myAnimationFPS;
	AnimationType myAnimationType;
	CU::GrowingArray<AnimationFrame> myKeyFrames;
};


#endif