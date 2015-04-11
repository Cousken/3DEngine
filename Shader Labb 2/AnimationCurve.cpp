#include "stdafx.h"
#include "AnimationCurve.h"

AnimationCurve::AnimationCurve()
{
	myKeyFrames.Init(60,60);
	myStartTime=0;
	myEndTime=0;
}

AnimationCurve::~AnimationCurve()
{
}

void AnimationCurve::AddKeyFrame(KeyFrame aFrame)
{
	myKeyFrames.Add(aFrame);
}

void AnimationCurve::FinalizeCurve()
{
	myStartTime=myKeyFrames[0].myTime;
	myEndTime=myKeyFrames[myKeyFrames.Count()-1].myTime;
}

double AnimationCurve::GetValueFromTime(double aTime)
{
	while(aTime>myEndTime) 
	{
		aTime-=myEndTime;
	}
	if(aTime<=myStartTime) return(myKeyFrames[0].myValue);
	if(aTime>myEndTime) return(myKeyFrames[myKeyFrames.Count()-1].myValue);

	for(int i=0;i<myKeyFrames.Count()-1;i++)
	{
		if(myKeyFrames[i].myTime>=aTime)
		{
			assert(i!=0);
//			return(myKeyFrames[i].myTime);
			double progress=(aTime-myKeyFrames[i-1].myTime)/(myKeyFrames[i].myTime-myKeyFrames[i-1].myTime);

			return(myKeyFrames[i].myValue+(myKeyFrames[i].myValue-myKeyFrames[i-1].myValue)*progress);
		}
	}
	return(myKeyFrames[0].myValue);
}
