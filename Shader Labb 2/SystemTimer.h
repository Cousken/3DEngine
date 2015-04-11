#ifndef _SYSTEM_TIMER_H_
#define _SYSTEM_TIMER_H_

#include <vector>
#include "Time.h"
#include <windows.h>

class SystemTimer
{
public:

	SystemTimer();
	~SystemTimer();

	void Update();
	float GetDeltaTime();
	void Reset();
	
private:

	Time myCurrentTime;
	Time myDeltaTime;
	LARGE_INTEGER myStartTime;
	LARGE_INTEGER myFrequency;
};

#endif // _SYSTEM_TIMER_H_