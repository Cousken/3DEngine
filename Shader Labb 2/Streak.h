#ifndef _STREAK_H_
#define _STREAK_H_

#include "CU_GrowingArray.h"
#include "StreakParticle.h"
#include "Effect.h"
#include "BufferWrapper.h"
#include "debugHeader.h"

class Streak
{
public:
	Streak(void);
	~Streak(void);

	void Update(const float aDeltaTime);

	void UpdateBuffers();

	void Render();
	bool Init();

	bool InitInputLayout();
	bool InitVertexBuffer();
	bool InitiIndexBuffer();
	
	void SetPosition( Vector3f aPosition );
	void Move( Vector3f aOffset );

private:
	float myEmissionRate;
	float myTimeSinceEmission;

	unsigned int myAliveParticles;
	Vector3f myPosition;

	static const int ourMaxParticles = 100;
	static const int ourNbrOfVertex = 100;
	static const int ourNbrOfVertexIndex = 100;
	
	Texture* myTexture;
	Effect* myEffect;
	ID3D10InputLayout* myVertexLayout;
	ID3D10Buffer* myVertexBuffer;
	ID3D10Buffer* myIndexBuffer;

	CommonUtilities::GrowingArray<VertexPosNorm> myVertex;
	CommonUtilities::GrowingArray<unsigned int> myVertexIndexes;
	CommonUtilities::GrowingArray<StreakParticle> myParticles;
	
};

#endif