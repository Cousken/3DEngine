#ifndef _CPU_PARTICLE_H_
#define _CPU_PARTICLE_H_

#include "Vector.h"
#include "debugHeader.h"

class CPUParticle
{
friend class CPUParticleEmittorInstance;

public:
	CPUParticle(void);
	~CPUParticle(void);

private:
	float myAlpha;
	Vector3f myPosition;
	float mySize;
	float myTime;
	Vector3f myVelocity;
};

#endif