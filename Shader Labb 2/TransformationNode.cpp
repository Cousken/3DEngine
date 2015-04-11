#include "TransformationNode.h"

TransformationNode::TransformationNode(void)
{
	myOrientation = Matrix44f::Identity();
}

TransformationNode::TransformationNode( const Matrix44f& aMatrix )
{
	myOrientation = aMatrix;
}

TransformationNode::~TransformationNode(void)
{
}

const Matrix44f TransformationNode::GetTransformationForTime( float aTime )
{
	return myOrientation;
}

void TransformationNode::SetOrientation( const Matrix44f& aMatrix )
{
	myOrientation = aMatrix;
}
