#pragma once

#include "Vector.h"
#include <vector>
#include "TreeDweller.h"
#include "debugHeader.h"

class Frustum;
class Camera;

class TreeNode
{
public:
	enum Corners
	{
		FUR,
		FUL,
		FLR,
		FLL,
		BUR,
		BUL,
		BLR,
		BLL
	};

	TreeNode();
	TreeNode(const TreeNode& aNode);
	
	void Render(Camera& aCamera);
	void Update(Frustum aFrustrum, CommonUtilities::GrowingArray<Instance*>& someObjectsInView);
	void ClearDwellers();
	float GetWidth() const;

	std::vector<Vector3f> GetAABBPoints() const;

	Vector3f myCenter;
	float myHalfWidth;
	TreeNode *myChildren[8];
	std::vector<TreeDweller*> myObjects;

};