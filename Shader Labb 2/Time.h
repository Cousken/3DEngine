#ifndef _TIME_H_
#define _TIME_H_

#include "debugHeader.h"

class Time
{
public:
	Time();
	~Time();

	void SetTime( double aTime );
	float GetTime();
		
private:

	double myTime;
};

#endif // _TIME_H_