#include "ModelContainer.h"

ModelContainer::ModelContainer(void)
{
	myModels.Init(10, 1);
}

ModelContainer::~ModelContainer(void)
{
}

Model* ModelContainer::GetModel( std::string aFileName )
{
	Model* result = 0;
	const int nbrOfModels = myModels.Count();
	for ( int modelChecked = 0; modelChecked < nbrOfModels; modelChecked++)
	{
		if (myModels[modelChecked]->GetFileName() == aFileName)
		{
			result = myModels[modelChecked];
			break;
		}

	}
	if ( result == 0 )
	{
		result = LoadModel(aFileName);
	}
	return result;
}

Model* ModelContainer::LoadModel( std::string aFileName )
{
	Model* newModel = mySDKMeshFactory.CreateModel(aFileName);	

	if ( newModel == 0 )
	{
		return 0;
	}

	myModels.Add(newModel);

	return newModel;
}
