#ifndef _MODEL_CONTAINER_H_
#define _MODEL_CONTAINER_H_

#include "CU_GrowingArray.h"
#include "Model.h"
#include "ColladaMeshFactory.h"

class ModelContainer
{
public:
	ModelContainer(void);
	~ModelContainer(void);

	Model* GetModel(std::string aFileName);
	Model* LoadModel(std::string aFileName);

private:
	CommonUtilities::GrowingArray<Model*> myModels;
	ColladaMeshFactory mySDKMeshFactory;
};

#endif