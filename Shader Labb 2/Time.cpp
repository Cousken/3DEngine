#include "Time.h"
#include <iostream>

Time::Time()
{
	myTime = 0;
}

Time::~Time()
{
}

void Time::SetTime( double aTime )
{
	myTime = aTime;
}

float Time::GetTime()
{
	return static_cast<float>( myTime );
}
