#include "TreeDweller.h"

TreeDweller::TreeDweller(const TreeDweller& aTreeDweller)
{
	myCenter = aTreeDweller.myCenter;
	myRadius = aTreeDweller.myRadius;
	myInstance = aTreeDweller.myInstance;
}

TreeDweller::TreeDweller( Instance* aInstance )
{
	myInstance = aInstance;
	aInstance->GetPosition(myCenter);
	myRadius = aInstance->GetModel().GetRadius();
}

void TreeDweller::Render(Camera& aCamera)
{
	myInstance->Render(aCamera, EffectTechniques::NORMAL);
}

Vector3f TreeDweller::GetCenter() const
{
	return myCenter;
}

float TreeDweller::GetRadius() const
{
	return myRadius;
}

Instance* TreeDweller::GetObject()
{
	return myInstance;
}
