#include "CPUParticle.h"

CPUParticle::CPUParticle(void)
{
	myAlpha = 1.0f;
	myPosition = Vector3f(0,0,0);
	mySize = 1.0f;
	myTime = 0;
	myVelocity = Vector3f(0,0,0);
}

CPUParticle::~CPUParticle(void)
{
}
