#include "TransformationNode_Hierarchy.h"

TransformationNode_Hierarchy::TransformationNode_Hierarchy(void)
{
	myNodes.Init(1,1);
}

TransformationNode_Hierarchy::~TransformationNode_Hierarchy(void)
{
}

const Matrix44f TransformationNode_Hierarchy::GetTransformationForTime( float aTime )
{
	Matrix44f result = Matrix44f::Identity();

	const int nbrOfNodes = myNodes.Count();
	for ( int nodeAdded = 0; nodeAdded < nbrOfNodes; nodeAdded++)
	{
		result = result * myNodes[nodeAdded]->GetTransformationForTime(aTime);
	}

	return result;
}

void TransformationNode_Hierarchy::AddTransformation( TransformationNode* aTransformation )
{
	myNodes.Add(aTransformation);
}
