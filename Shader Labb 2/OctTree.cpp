#include "OctTree.h"
#include <assert.h>
#include "Frustum.h"

void OctTree::InsertObject( TreeNode *aTreeNode, TreeDweller *anObject )
{
	assert((myRoot != 0) && "This tree is not built yet.");
	int index = 0;
	bool straddle = false;
	float deltaX, deltaY, deltaZ;

	deltaX = (anObject->GetCenter().x - aTreeNode->myCenter.x);
	straddle = fabs(deltaX) < anObject->GetRadius();
	if ( deltaX > 0.0f ) index = 1;

	if ( !straddle ) {
		deltaY = anObject->GetCenter().y - aTreeNode->myCenter.y;
		straddle = fabs(deltaY) < anObject->GetRadius();
		if ( deltaY > 0.0f ) index += 2;
	}

	if ( !straddle ) {
		deltaZ = anObject->GetCenter().z - aTreeNode->myCenter.z;
		straddle = fabs(deltaZ) < anObject->GetRadius();
		if ( deltaZ > 0.0f ) index += 4;
	}

	if ( !straddle && (aTreeNode->myChildren[index] != 0 ) ) {
		InsertObject( aTreeNode->myChildren[index], anObject );
	} else {
		aTreeNode->myObjects.push_back(anObject);
	}
}

TreeNode* OctTree::GetRoot() const
{
	return myRoot;
}

void OctTree::BuildTree( Vector3f &aCenter, float aHalfWidth, int aStopDepth, const float aLoosnes )
{
	myRoot = new TreeNode();
	myRoot->myCenter = aCenter;
	myRoot->myHalfWidth = aHalfWidth;
	myLoosnes = aLoosnes;

	float step = aHalfWidth * 0.5f;
	Vector3f newCenter;
	for (int i = 0; i < 4; i++) {
		newCenter.x = aCenter.x + ((i & 1) ? step : -step);
		newCenter.y = aCenter.y + ((i & 2) ? step : -step);
		newCenter.z = aCenter.z + ((i & 4) ? step : -step);
		myRoot->myChildren[i] = BuildTreeRecursive(newCenter, step, (aStopDepth - 1) );
	}
}

TreeNode* OctTree::BuildTreeRecursive( Vector3f &aCenter, float aHalfWidth, int aStopDepth )
{
	if( aStopDepth == 0 ) return 0;

	TreeNode *newNode = new TreeNode();
	newNode->myCenter = aCenter;
	newNode->myHalfWidth = aHalfWidth;

	float step = aHalfWidth * 0.5f;
	Vector3f newCenter;
	for (int i = 0; i < 4; i++) {
		newCenter.x = aCenter.x + ((i & 1) ? step : -step);
		newCenter.y = aCenter.y + ((i & 2) ? step : -step);
		newCenter.z = aCenter.z + ((i & 4) ? step : -step);
		newNode->myChildren[i] = BuildTreeRecursive(newCenter, step, (aStopDepth - 1) );
	}
	return newNode;
}

// void OctTree::Render(Camera& aCamera) const
// {
// 	myRoot->Render(aCamera);
// }

void OctTree::Update( Frustum& aFrusturm, CommonUtilities::GrowingArray<Instance*>& someObjectsInView )
{
	myRoot->Update(aFrusturm, someObjectsInView);
}

