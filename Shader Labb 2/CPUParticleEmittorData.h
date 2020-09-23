#ifndef _CPU_PARTICLE_EMITTOR_DATA_H_
#define _CPU_PARTICLE_EMITTOR_DATA_H_

#include <string>
#include "Vector.h"
#include "CPUParticleData.h"
#include <D3D10.h>
#include "debugHeader.h"

class Texture;
class Effect;

class CPUParticleEmittorData
{
friend class CPUParticleEmittorInstance;

public:
	CPUParticleEmittorData(void);
	~CPUParticleEmittorData(void);

	bool LoadDataFile(std::string aDataFile);

private:
	bool InitLayoutBuffer();

	Effect* myEffect;
	std::string myEffectName;
	Vector3f myEmissionExtents;
	float myEmissionLifeTime;
	float myEmissionRate;
	float myEmissionRateDelta;
	float myEmissionSpeed;
	Vector3f myEmissionVelocityDelta;
	int myMaxNrOfParticles;
	Vector3f myMaxRotation;
	Vector3f myMaxVelocity;
	Vector3f myMinRotation;
	Vector3f myMixVelocity;
	CPUParticleData myParticleData;
	ID3D10InputLayout* myParticleInputLayout;
	Vector3f myRotationDelta;
	Texture* myTexture;
	std::string myTextureName;
};

#endif