#ifndef _EFFECT_CONTAINER_H_
#define _EFFECT_CONTAINER_H_

#include "CU_GrowingArray.h"
#include "Effect.h"

class EffectContainer
{
public:
	EffectContainer(void);
	~EffectContainer(void);

	Effect* GetEffect(std::string aEffectFile);
	Effect* LoadEffect(std::string aEffectFile);

private:
	CommonUtilities::GrowingArray<Effect*> myEffects;
};

#endif