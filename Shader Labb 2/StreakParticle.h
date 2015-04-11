#ifndef _STREAK_PARTICLE_H_
#define _STREAK_PARTICLE_H_

class StreakParticle
{
public:
	StreakParticle(void);
	~StreakParticle(void);

	void Update(const float aDeltaTime);
	bool IsDead() const;

private:
	float myElapsedTime;
	float myLifeTime;
};

#endif