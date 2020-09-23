#ifndef N3_ANIMATIONCURVE_HEADER
#define N3_ANIMATIONCURVE_HEADER

#include "CU_GrowingArray.h"
#include "debugHeader.h"

struct KeyFrame
{
	double myTime;
	double myValue;
};

class AnimationCurve
{
public:
	AnimationCurve();
	~AnimationCurve();

	void AddKeyFrame(KeyFrame aFrame);
	void FinalizeCurve();

	double GetValueFromTime(double aTime);
protected:
	double myStartTime;
	double myEndTime;
	CommonUtilities::GrowingArray<KeyFrame> myKeyFrames;
	
};


#endif