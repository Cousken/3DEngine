#include "SystemTimer.h"

SystemTimer::SystemTimer()
{
	myCurrentTime.SetTime(0);
	myDeltaTime.SetTime(0);

	QueryPerformanceCounter(&myStartTime);
	QueryPerformanceFrequency(&myFrequency);
}

SystemTimer::~SystemTimer()
{
}

void SystemTimer::Update()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);

	double previousTime = myCurrentTime.GetTime();
	myCurrentTime.SetTime(static_cast<double>(currentTime.QuadPart-myStartTime.QuadPart)/static_cast<double>(myFrequency.QuadPart));
	myDeltaTime.SetTime(myCurrentTime.GetTime()-previousTime);
}

void SystemTimer::Reset()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	myCurrentTime.SetTime(static_cast<double>(currentTime.QuadPart-myStartTime.QuadPart)/static_cast<double>(myFrequency.QuadPart));
}

float SystemTimer::GetDeltaTime()
{
	return myDeltaTime.GetTime();
}