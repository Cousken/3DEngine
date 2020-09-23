#ifndef _TRANSFORMATION_NODE_INSTANCE_H_
#define _TRANSFORMATION_NODE_INSTANCE_H_

#include "TransformationNode.h"
#include "CU_GrowingArray.h"
#include "debugHeader.h"

class TransformationNodeInstance
{
public:
	TransformationNodeInstance(void);

	~TransformationNodeInstance(void);
	
	void SetTransformationNode(TransformationNode* aTransformationNode);

	CommonUtilities::GrowingArray<TransformationNodeInstance*>& GetChilds();
	const Matrix44f GetTransformation();
	TransformationNode* GetTransformationNode();
	
	TransformationNodeInstance& operator = (const TransformationNodeInstance& aTransformationNodeInstance);

	void AddChildNode(TransformationNodeInstance* aChildNode);
	void Update(float aElapsedTime);

private:
	CommonUtilities::GrowingArray<TransformationNodeInstance*> myChildren;

	double myElapsedTime;
	TransformationNode* myTransformationNode;
};

#endif