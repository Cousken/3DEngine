#pragma once

#include "Vector.h"
#include "Instance.h"
#include "debugHeader.h"

class Camera;

class TreeDweller
{
public:
	TreeDweller(Instance* aInstance);
	TreeDweller(const TreeDweller& aTreeDweller);

	Vector3f GetCenter() const;
	float GetRadius() const;

	void Render(Camera& aCamera);
	Instance* GetObject();

private:
	Vector3f 	myCenter;
	float		myRadius;
	Instance* myInstance;
};