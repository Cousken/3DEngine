#include "StreakParticle.h"

StreakParticle::StreakParticle(void)
{
	myElapsedTime = 0;
	myLifeTime = 5;
}

StreakParticle::~StreakParticle(void)
{
}

void StreakParticle::Update( const float aDeltaTime )
{
	myElapsedTime += aDeltaTime;
}

bool StreakParticle::IsDead() const
{
	if ( myElapsedTime >= myLifeTime )
	{
		return true;
	}
	return false;
}

