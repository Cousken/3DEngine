#include "TreeNode.h"
#include "OOBB.h"

TreeNode::TreeNode()
{
	myCenter = Vector3f(0,0,0);
	myChildren[0] = 0;
	myChildren[1] = 0;
	myChildren[2] = 0;
	myChildren[3] = 0;
	myChildren[4] = 0;
	myChildren[5] = 0;
	myChildren[6] = 0;
	myChildren[7] = 0;
	myHalfWidth = 0;
	myObjects.clear();
}

TreeNode::TreeNode(const TreeNode& aNode)
{
	myCenter = aNode.myCenter;
	for ( int childCopied = 0; childCopied < 8; childCopied++)
	{
		myChildren[childCopied] = aNode.myChildren[childCopied];
	}

	myHalfWidth = aNode.myHalfWidth;
	myObjects = aNode.myObjects;
}

void TreeNode::Render(Camera& aCamera)
{
	Frustum& frustrum = aCamera.GetFrustrum();
	if ( frustrum.BoxInside(OOBB(myCenter, (2*myHalfWidth))) ){
		
		const int nbrOfObjects = myObjects.size();
		for (int objectRendered = 0; objectRendered < nbrOfObjects; objectRendered++)
		{
			if ( frustrum.SphereInside(myObjects[objectRendered]->GetCenter(), myObjects[objectRendered]->GetRadius()) == true)
			{
				myObjects[objectRendered]->Render(aCamera);
			}
		}

		for(int renderedChild = 0; renderedChild < 8; renderedChild++)
		{
			if ( myChildren[renderedChild] != 0)
			{
				myChildren[renderedChild]->Render(aCamera);
			}
		}
	}

// 	const int nbrOfObjects = myObjects.size();
// 	for (int objectRendered = 0; objectRendered < nbrOfObjects; objectRendered++)
// 	{
// 		//if ( frustrum.ColidesWithObject(myObjects[objectRendered]->GetCenter(), myObjects[objectRendered]->GetRadius()) == true)
// 		{
// 			myObjects[objectRendered]->Render(aCamera);
// 		}
// 	}
// 
// 	for(int renderedChild = 0; renderedChild < 8; renderedChild++)
// 	{
// 		if ( myChildren[renderedChild] != 0)
// 		{
// 			myChildren[renderedChild]->Render(aCamera);
// 		}
// 	}

}

void TreeNode::Update( Frustum aFrustrum, CommonUtilities::GrowingArray<Instance*>& someObjectsInView )
{
	if ( aFrustrum.BoxInside(OOBB(myCenter, (2*myHalfWidth))) )
	{
		const int nbrOfObjects = myObjects.size();
		for ( int objectChecked = 0; objectChecked < nbrOfObjects; objectChecked++)
		{
			if ( aFrustrum.SphereInside(myObjects[objectChecked]->GetCenter(), myObjects[objectChecked]->GetRadius()) == true)
			{
				someObjectsInView.Add(myObjects[objectChecked]->GetObject());
			}
		}

		for(int updatedChild = 0; updatedChild < 4; updatedChild++)
		{
			if ( myChildren[updatedChild] != 0)
			{
				myChildren[updatedChild]->Update(aFrustrum, someObjectsInView);
			}
		}
	}
}

void TreeNode::ClearDwellers()
{
	myObjects.clear();
	
	for(int cleanedChild = 0; cleanedChild < 8; cleanedChild++)
	{
		if ( myChildren[cleanedChild] != 0)
		{
			myChildren[cleanedChild]->ClearDwellers();
		}
	}
}

float TreeNode::GetWidth() const
{
	return myHalfWidth * 2;
}

std::vector<Vector3f> TreeNode::GetAABBPoints() const
{
	std::vector<Vector3f> result;
	result.resize(8);

	result[FUR] = Vector3<float>(myCenter.x+myHalfWidth, myCenter.y-myHalfWidth, myCenter.z-myHalfWidth);
	result[FUL] = Vector3<float>(myCenter.x-myHalfWidth, myCenter.y-myHalfWidth, myCenter.z-myHalfWidth);
	result[FLR] = Vector3<float>(myCenter.x+myHalfWidth, myCenter.y+myHalfWidth, myCenter.z-myHalfWidth);
	result[FLL] = Vector3<float>(myCenter.x-myHalfWidth, myCenter.y+myHalfWidth, myCenter.z-myHalfWidth);	

	result[BUR] = Vector3<float>(myCenter.x+myHalfWidth, myCenter.y-myHalfWidth, myCenter.z+myHalfWidth);
	result[BUL] = Vector3<float>(myCenter.x-myHalfWidth, myCenter.y-myHalfWidth, myCenter.z+myHalfWidth);
	result[BLR] = Vector3<float>(myCenter.x+myHalfWidth, myCenter.y+myHalfWidth, myCenter.z+myHalfWidth);
	result[BLL] = Vector3<float>(myCenter.x-myHalfWidth, myCenter.y+myHalfWidth, myCenter.z+myHalfWidth);

	return result;
}