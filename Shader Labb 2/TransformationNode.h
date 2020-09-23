#ifndef _TRANSFORMATION_NODE_H_
#define _TRANSFORMATION_NODE_H_

#include "Matrix.h"
#include "debugHeader.h"

class TransformationNode
{
public:
	TransformationNode(void);
	TransformationNode(const Matrix44f& aMatrix);

	~TransformationNode(void);

	virtual const Matrix44f GetTransformationForTime( float aTime );
	void SetOrientation(const Matrix44f& aMatrix);


protected:
	Matrix44f myOrientation;
};

#endif