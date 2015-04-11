#ifndef _CPU_PARTICLE_DATA_H_
#define _CPU_PARTICLE_DATA_H_

#include "Vector.h"

class CPUParticleData
{
friend class CPUParticleEmittorData;

public:
	CPUParticleData(void);
	~CPUParticleData(void);

private:


	bool myAffectedByGravityFlag;
	float myAlphaDelta;
	float myLifeTime;
	float myMaxStartSize;
	Vector3f myMaxVelocity;
	float myMinStartSize;
	Vector3f myMinVelocity;
	float mySizeDelta;
	float myStartAlpha;
};

#endif
