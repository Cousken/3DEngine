#ifndef _RENDER_PROCESS_TARGET_H_
#define _RENDER_PROCESS_TARGET_H_

#include "CU_GrowingArray.h"
#include "Matrix.h"
#include "debugHeader.h"

class Model;

class RenderProcessTarget
{
public:
	RenderProcessTarget(void);
	~RenderProcessTarget(void);

	void Init();
	void AddInstance( Model* aModel, Matrix44f aOrientation );
	void Render();
	void Clear();
private:
	struct ModelMeshInstancingData
	{
		ModelMeshInstancingData()
		{
			myModelMesh = NULL;
			myInstanceWorldMatrix.Init(1,1);
		}

		Model* myModelMesh;
		CommonUtilities::GrowingArray<Matrix44f> myInstanceWorldMatrix;
	};
	
	CommonUtilities::GrowingArray<ModelMeshInstancingData> myModelMeshInstancingData;
	
};

#endif