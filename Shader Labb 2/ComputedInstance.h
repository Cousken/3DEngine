#ifndef _COMPUTED_INSTANCE_H_
#define _COMPUTED_INSTANCE_H_

#include "ComputedModel.h"
#include <d3d10.h>
#include <d3dx10.h>
#include "Vector.h"
#include "Matrix.h"
#include "Camera.h"
#include "Effect.h"
#include "debugHeader.h"

class ComputedInstance
{
public:
	ComputedInstance(ComputedModel& aModel);
	~ComputedInstance(void);
	bool Init();
	void Render(Camera& aCamera);

	void PerformRotation(Matrix33f& aOrientation);
	void PerformTransformation(Matrix44f& aOrientation);

	void GetPosition(Vector3f& aPosition);
	Matrix44f& GetOrientation();
	ComputedModel& GetModel();

	void SetPosition(Vector3f& aPosition);
	void SetOrientation(Matrix44f& aOrientation);

	

private:
	bool InitiIndexBuffer();
	bool InitVertexBuffer();

	ComputedModel& myModel;
	std::string myEffectFile;

	Matrix44f myOrientation;
	Vector3f myPosition;

	ID3D10Buffer* myVertexBuffer;
	ID3D10Buffer* myIndexBuffer;
	ID3D10InputLayout* myVertexLayout;
};

#endif