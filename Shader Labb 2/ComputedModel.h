#ifndef _COMPUTED_MODEL_H_
#define _COMPUTED_MODEL_H_

#include "Vector.h"
#include "Matrix.h"
#include "CU_GrowingArray.h"
#include "Effect.h"
#include "BufferWrapper.h"
#include "debugHeader.h"

struct VertexPosCol
{
	VertexPosCol()
	{
		myPos = Vector3f(0,0,0);
		myCol = Vector4f(0,0,0,0);
		myNormal = Vector3f(0,0,0);
	};

	VertexPosCol(const Vector3f aPosition, const Vector4f aColor, const Vector3f aNormal) 
	{
		myPos = aPosition;
		myCol = aColor;
		myNormal = aNormal;
	};

	Vector3f myPos;
	Vector4f myCol;
	Vector3f myNormal;
};

class ComputedModel
{
public:
	ComputedModel(void);
	~ComputedModel(void);

	VertexType GetMyVertexType();
	const CommonUtilities::GrowingArray<VertexPosCol>& GetVertexData();
	const CommonUtilities::GrowingArray<unsigned int>& GetVertextIndexes();

	void InitCube(Effect* aEffect);
	void InitPolygon(Effect* aEffect);
	void InitSphere( Effect* aEffect );
	void InitCone( Effect* aEffect );

	Effect* GetEffect();
	
private:
	Effect* myEffect;
	//VertexType myVertextype;
	CommonUtilities::GrowingArray<unsigned int> myVerticeIndexes;
	CommonUtilities::GrowingArray<VertexPosCol> myVertices;

};

#endif