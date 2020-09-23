#ifndef _CPU_PARTICLE_EMITTOR_INSTANCE_H_
#define _CPU_PARTICLE_EMITTOR_INSTANCE_H_

#include "Matrix.h"
#include "Vector.h"
#include "CPUParticleEmittorData.h"
#include "CPUParticle.h"
#include "CU_GrowingArray.h"
#include "BufferWrapper.h"
#include <D3D10.h>
#include <D3DX10.h>
#include "debugHeader.h"

class CPUParticleEmittorInstance
{
public:
	CPUParticleEmittorInstance( CPUParticleEmittorData& aEmittor);
	~CPUParticleEmittorInstance(void);

	void SetOrientation(const Matrix44f& aOrientation);
	void SetPosition(const Vector3f& aPosition);

	const Matrix44f& GetOrientation();
	const Vector3f& GetPosition();	

	void PerformRotation( const Matrix33f aOrientation);
	void PerformTransformation( const Matrix44f& aOrientation );

	void Render();	
	void Update(const float aElapsedTime);
	

private:
	bool CreateIndexBuffer();
	bool CreateVertexBuffer();

	void EmittorUpdate(const float aElapsedTime);
	void EmittParticle(const float aElapsedTime);

	void ParticlesUpdate( const float aElapsedTime);
	void UpdateVertexBuffer();

	Matrix33f myEmissionOrientation;
	CPUParticleEmittorData& myEmittor;
	float myGravity;
	IndexBufferWrapper myIndexBufferWrapper;
	Matrix44f myOrientation;
	CommonUtilities::GrowingArray<unsigned int, unsigned int> myParticleIndexes;
	CommonUtilities::GrowingArray<CPUParticle, unsigned int> myParticles;
	Vector3f myPosition;
	float myProcessTime;
	VertexBufferWrapper myVertexBufferWrapper;
};

#endif