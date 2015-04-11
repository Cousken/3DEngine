#include "TransformationNodeInstance.h"

TransformationNodeInstance::TransformationNodeInstance(void)
{
	myElapsedTime = 0;
	myChildren.Init(1,1);

}

TransformationNodeInstance::~TransformationNodeInstance(void)
{
}

const Matrix44f TransformationNodeInstance::GetTransformation()
{
	return myTransformationNode->GetTransformationForTime(myElapsedTime);
}

TransformationNode* TransformationNodeInstance::GetTransformationNode()
{
	return myTransformationNode;
}

CommonUtilities::GrowingArray<TransformationNodeInstance*>& TransformationNodeInstance::GetChilds()
{
	return myChildren;
}

void TransformationNodeInstance::AddChildNode( TransformationNodeInstance* aChildNode )
{	
	myChildren.Add(aChildNode);
}

TransformationNodeInstance& TransformationNodeInstance::operator=( const TransformationNodeInstance& aTransformationNodeInstance )
{
	myChildren = aTransformationNodeInstance.myChildren;
	myElapsedTime = aTransformationNodeInstance.myElapsedTime;
	myTransformationNode = aTransformationNodeInstance.myTransformationNode;

	return (*this);
}

void TransformationNodeInstance::SetTransformationNode( TransformationNode* aTransformationNode )
{
	myTransformationNode = aTransformationNode;
}

void TransformationNodeInstance::Update( float aElapsedTime )
{
	myElapsedTime += aElapsedTime;

	const int nbrOfChildren = myChildren.Count();
	for ( int childUpdated = 0; childUpdated < nbrOfChildren; childUpdated++)
	{
		myChildren[childUpdated]->Update(aElapsedTime);
	}
}
