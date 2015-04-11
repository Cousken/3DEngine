#ifndef _TRANSFORMATION_NODE_HIERARCHY_H_
#define _TRANSFORMATION_NODE_HIERARCHY_H_

#include "TransformationNode.h"
#include "CU_GrowingArray.h"
#include "Matrix.h"

class TransformationNode_Hierarchy : public TransformationNode
{

public:
	TransformationNode_Hierarchy(void);
	~TransformationNode_Hierarchy(void);

	void AddTransformation(TransformationNode* aTransformation);
	const Matrix44f GetTransformationForTime( float aTime );


private:
	CommonUtilities::GrowingArray<TransformationNode*> myNodes;

};

#endif