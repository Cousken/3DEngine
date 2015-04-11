#include "EffectContainer.h"

EffectContainer::EffectContainer(void)
{
	myEffects.Init(10, 1);
}

EffectContainer::~EffectContainer(void)
{
}

Effect* EffectContainer::GetEffect( std::string aEffectFile )
{
	Effect* result = 0;
	const int nbrOfEffects = myEffects.Count();
	for ( int effectChecked = 0; effectChecked < nbrOfEffects; effectChecked++)
	{
		if (myEffects[effectChecked]->GetFileName() == aEffectFile)
		{
			result = myEffects[effectChecked];
			break;
		}

	}
	if ( result == 0 )
	{
		result = LoadEffect(aEffectFile);
	}
	return result;
}

Effect* EffectContainer::LoadEffect( std::string aEffectFile )
{
	Effect* newEffect = new Effect();


	if (newEffect->InitEffect(aEffectFile) == false )
	{
		return 0;
	}

	myEffects.Add(newEffect);

	return newEffect;
}
