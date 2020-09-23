#pragma once

#include "TreeNode.h"
#include "debugHeader.h"

class Frustum;
class Camera;

class OctTree
{
public:
	void BuildTree( Vector3f &aCenter, float aHalfWidth, int aStopDepth, const float aLoosnes );
	void InsertObject( TreeNode *aTreeNode, TreeDweller *anObject );

	//void Render(Camera& aCamera) const;
	void Update(Frustum& aFrusturm, CommonUtilities::GrowingArray<Instance*>& someObjectsInView);

	TreeNode* GetRoot() const;

private:
	TreeNode* BuildTreeRecursive( Vector3f &aCenter, float aHalfWidth, int aStopDepth );

	TreeNode *myRoot;
	float myLoosnes;


};