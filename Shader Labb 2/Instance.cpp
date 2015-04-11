#include "Instance.h"
#include "Engine.h"
#include "Light.h"
#include "EffectTechnique.h"

Instance::~Instance(void)
{
}	

bool Instance::Init( )
{
	myOrientation = Matrix44f::Identity();
	BuildHierachy(myHierarchy, &myModel);

	myBoneInstances.Init( myModel.myBones.Count(), 1 );

	if( myModel.GetAnimation() != NULL )
	{
		myAnimation = new BoneInstanceHierarchy( myModel.GetAnimation() );

		for( int i = 0; i < myModel.myBones.Count(); i++ )
		{
			BoneInstance* temp = new BoneInstance;
			temp->SetBone( &myModel.myBones[i] );
			temp->SetBoneAnimationInstance( myAnimation->GetTransformationAnimations()[i] );
			myBoneInstances.Add( temp );
		}
	}

	return true;	
}

Instance::Instance( Model& aModel )
: myModel(aModel)
{
	myEffectFile = "";
	ClearLights();

	myAnimation = NULL;
}

void Instance::Render( Camera& aCamera, EffectTechniques::TechniqueType aTechinqueType )
{
	UpdateLights();

	CommonUtilities::StaticArray< Matrix44f, 255 > boneMatrices;


	for( int i = 0; i < myBoneInstances.Count(); i++ )
	{
		boneMatrices[i] = myBoneInstances[i]->GetTransformed();
		//boneMatrices[i] = Matrix44f::Identity();
	}

	myOrientation.SetPosition(myPosition);
	RenderModel(&myModel, myOrientation, aTechinqueType, myHierarchy, boneMatrices);
}

void Instance::RenderUsingEffect(Camera& aCamera, EffectTechniques::TechniqueType aTechinqueType, Effect *anEffect)
{
	myOrientation.SetPosition(myPosition);
	RenderModelUsingEffect(&myModel, myOrientation, aTechinqueType, myHierarchy, anEffect);
}


void Instance::RenderFromLight( EffectTechniques::TechniqueType aTechinqueType )
{
	CommonUtilities::StaticArray< Matrix44f, 255 > boneMatrices;


	for( int i = 0; i < myBoneInstances.Count(); i++ )
	{
		boneMatrices[i] = myBoneInstances[i]->GetTransformed();
		//boneMatrices[i] = Matrix44f::Identity();
	}

	myOrientation.SetPosition(myPosition);
	RenderModel(&myModel, myOrientation, aTechinqueType, myHierarchy, boneMatrices);
}


void Instance::RenderToTarget( Camera& myCamera, EffectTechniques::TechniqueType aTechnique, RenderProcessTarget& aRenderTarget )
{
	CommonUtilities::StaticArray< Matrix44f, 255 > boneMatrices;

	for( int i = 0; i < myBoneInstances.Count(); i++ )
	{
		boneMatrices[i] = myBoneInstances[i]->GetTransformed();
	}

	myOrientation.SetPosition(myPosition);
	RenderModelToTarget(&myModel, myOrientation, aTechnique, myHierarchy, boneMatrices, aRenderTarget);
}

void Instance::RenderModel( Model* aModel, const Matrix44f aParentSpace, EffectTechniques::TechniqueType aTechniqueType, TransformationNodeInstance& aHierarchy, CommonUtilities::StaticArray< Matrix44f, 255 >& aBoneMatrixArray )
{
	aModel->Render(aParentSpace, aBoneMatrixArray, aTechniqueType);

	for(int i=0;i<aModel->myChilds.Count();i++)
	{
		assert(aHierarchy.GetChilds()[i]->GetTransformationNode()!=NULL);
		Matrix44f worldMatrix=aHierarchy.GetChilds()[i]->GetTransformation();
		worldMatrix*=aParentSpace;

		//myModel.Render(worldMatrix, aTechniqueType);

		RenderModel(aModel->myChilds[i], worldMatrix, aTechniqueType, (*aHierarchy.GetChilds()[i]), aBoneMatrixArray );
	}
}

void Instance::RenderModelUsingEffect( Model* aModel, const Matrix44f aParentSpace, EffectTechniques::TechniqueType aTechniqueType, TransformationNodeInstance& aHierarchy, Effect *anEffect )
{
	aModel->RenderUsingEffect(aParentSpace, aTechniqueType, anEffect);

	for(int i=0;i<aModel->myChilds.Count();i++)
	{
		assert(aHierarchy.GetChilds()[i]->GetTransformationNode()!=NULL);
		Matrix44f worldMatrix=aHierarchy.GetChilds()[i]->GetTransformation();
		worldMatrix*=aParentSpace;

		//myModel.Render(worldMatrix, aTechniqueType);

		RenderModelUsingEffect(aModel->myChilds[i], worldMatrix, aTechniqueType, (*aHierarchy.GetChilds()[i]),  anEffect );
	}
}


void Instance::RenderModelToTarget( Model* aModel, const Matrix44f aParentSpace, EffectTechniques::TechniqueType aTechniqueType, TransformationNodeInstance& aHierarchy, CommonUtilities::StaticArray< Matrix44f, 255 >& aBoneMatrixArray, RenderProcessTarget& aRenderTarget )
{
	aModel->RenderToTarget(aParentSpace, aBoneMatrixArray, aRenderTarget, aTechniqueType);

	for(int i=0;i<aModel->myChilds.Count();i++)
	{
		assert(aHierarchy.GetChilds()[i]->GetTransformationNode()!=NULL);
		Matrix44f worldMatrix=aHierarchy.GetChilds()[i]->GetTransformation();
		worldMatrix*=aParentSpace;

		//myModel.Render(worldMatrix, aTechniqueType);

		RenderModelToTarget(aModel->myChilds[i], worldMatrix, aTechniqueType, (*aHierarchy.GetChilds()[i]), aBoneMatrixArray, aRenderTarget );
	}
}


void Instance::SetOrientation( Matrix44f& aOrientation )
{
	myOrientation = aOrientation;
	myPosition = myOrientation.GetPosition();
}

Matrix44f& Instance::GetOrientation()
{
	myOrientation.SetPosition(myPosition);
	return myOrientation;
}

void Instance::SetPosition( Vector3f& aPosition )
{
	myPosition = aPosition;
}

void Instance::GetPosition( Vector3f& aPosition )
{
	aPosition = myPosition;
}

void Instance::PerformRotation( Matrix33f& aOrientation )
{
	myOrientation = myOrientation * aOrientation;
}

void Instance::PerformTransformation( Matrix44f& aOrientation )
{
	myOrientation.SetPosition(myPosition);
	myOrientation *= aOrientation;
	myPosition = myOrientation.GetPosition();
}

Model& Instance::GetModel()
{
	return myModel;
}

void Instance::TryAddLight( float aLightsStrength, Light* aLight )
{
	if (aLight != 0 )
	{
		if (aLight->GetType() == Light::DIRECTIONAL_LIGHT_TYPE)
		{
			TryAddDirectionalLight(aLight, aLightsStrength);
		}
		else if (aLight->GetType() == Light::POINT_LIGHT_TYPE)
		{
			TryAddPointLight(aLight, aLightsStrength);
		}
		else if (aLight->GetType() == Light::SPOT_LIGHT_TYPE)
		{
			TryAddSpotLight(aLight, aLightsStrength);
		}
	}	
}

void Instance::ClearLights()
{
	myDirectionalLights[0] = 0;
	myDirectionalLights[1] = 0;
	myDirectionalLights[2] = 0;
	myDirectionalLights[3] = 0;

	myPointLights[0] = 0;
	myPointLights[1] = 0;
	myPointLights[2] = 0;
	myPointLights[3] = 0;

	mySpotLights[0] = 0;
	mySpotLights[1] = 0;
	mySpotLights[2] = 0;
	mySpotLights[3] = 0;
}

void Instance::UpdateLights()
{
	UpdateDirectionalLights();
	UpdatePointLights();
	UpdateSpotLights();
}

void Instance::TryAddDirectionalLight( Light* aLight, float aLightsStrength )
{
	const int nbrOfLights = 4;
	for (int lightChecked = 0; lightChecked < nbrOfLights; lightChecked++)
	{
		if (myDirectionalLights[lightChecked] == 0)
		{
			LightWithStrength* newStruct = new LightWithStrength(aLight, aLightsStrength);
			myDirectionalLights.Insert(lightChecked, newStruct);
			break;
		}
		else  
		{			
			if ( myDirectionalLights[lightChecked]->myStrength < aLightsStrength )
			{
				LightWithStrength* newStruct = new LightWithStrength(aLight, aLightsStrength);
				myDirectionalLights.Insert(lightChecked, newStruct);
				break;
			}			
		}
	}
}

void Instance::TryAddPointLight( Light* aLight, float aLightsStrength )
{
	const int nbrOfLights = 4;
	for (int lightChecked = 0; lightChecked < nbrOfLights; lightChecked++)
	{
		if (myPointLights[lightChecked] == 0)
		{
			LightWithStrength* newStruct = new LightWithStrength(aLight, aLightsStrength);
			myPointLights.Insert(lightChecked, newStruct);
			break;
		}
		else  
		{			
			if ( myPointLights[lightChecked]->myStrength < aLightsStrength )
			{
				LightWithStrength* newStruct = new LightWithStrength(aLight, aLightsStrength);
				myPointLights.Insert(lightChecked, newStruct);
				break;
			}			
		}
	}
}

void Instance::TryAddSpotLight( Light* aLight, float aLightsStrength )
{
	const int nbrOfLights = 4;
	for (int lightChecked = 0; lightChecked < nbrOfLights; lightChecked++)
	{
		if (mySpotLights[lightChecked] == 0)
		{
			LightWithStrength* newStruct = new LightWithStrength(aLight, aLightsStrength);
			mySpotLights.Insert(lightChecked, newStruct);
			break;
		}
		else  
		{			
			if ( mySpotLights[lightChecked]->myStrength < aLightsStrength )
			{
				LightWithStrength* newStruct = new LightWithStrength(aLight, aLightsStrength);
				mySpotLights.Insert(lightChecked, newStruct);
				break;
			}			
		}
	}
}

void Instance::UpdateDirectionalLights()
{
	//CommonUtilities::StaticArray<Vector4f,9> lightVectors;
	//CommonUtilities::StaticArray<Vector4f,9> lightColours;
	//int nbrOfLights = 0;

	//for (int lightUpdated = 0; lightUpdated < 4; lightUpdated++)
	//{
	//	if (myDirectionalLights[lightUpdated] != 0)
	//	{
	//		lightVectors[lightUpdated] = myDirectionalLights[lightUpdated]->myLight->GetCurrentLightDir();
	//		lightColours[lightUpdated] = myDirectionalLights[lightUpdated]->myLight->GetLightColor();
	//		nbrOfLights++;
	//	}
	//}
	//Engine::GetInstance()->GetEffectInput().UpdateDirectionalLights(lightVectors, lightColours, nbrOfLights);
}

void Instance::UpdatePointLights()
{
	//CommonUtilities::StaticArray<Vector4f,9> lightColours;
	//CommonUtilities::StaticArray<Vector3f, 9> lightPositions;
	//CommonUtilities::StaticArray<float, 9> lightMaxDistnaces;
	//int nbrOfLights = 0;

	//for (int lightUpdated = 0; lightUpdated < 4; lightUpdated++)
	//{
	//	if (myPointLights[lightUpdated] != 0)
	//	{
	//		lightColours[lightUpdated] = myPointLights[lightUpdated]->myLight->GetLightColor();
	//		lightPositions[lightUpdated] = myPointLights[lightUpdated]->myLight->GetPosition();
	//		lightMaxDistnaces[lightUpdated] = myPointLights[lightUpdated]->myLight->GetMaxDistance();
	//		nbrOfLights++;
	//	}
	//}
	//Engine::GetInstance()->GetEffectInput().UpdatePointLights(lightPositions, lightColours, lightMaxDistnaces, nbrOfLights);
}

void Instance::UpdateSpotLights()
{
	//CommonUtilities::StaticArray<Vector4f,9> lightVectors;
	//CommonUtilities::StaticArray<Vector4f,9> lightColours;
	//CommonUtilities::StaticArray<Vector3f, 9> lightPositions;
	//CommonUtilities::StaticArray<float, 9> lightMaxDistnaces;
	//CommonUtilities::StaticArray<float, 9> lightInnerFallofs;
	//CommonUtilities::StaticArray<float, 9> lightsOuterFallofs;
	//int nbrOfLights = 0;

	//for (int lightUpdated = 0; lightUpdated < 4; lightUpdated++)
	//{
	//	if (mySpotLights[lightUpdated] != 0)
	//	{
	//		lightVectors[lightUpdated] = mySpotLights[lightUpdated]->myLight->GetCurrentLightDir();
	//		lightColours[lightUpdated] = mySpotLights[lightUpdated]->myLight->GetLightColor();
	//		lightPositions[lightUpdated] = mySpotLights[lightUpdated]->myLight->GetPosition();
	//		lightMaxDistnaces[lightUpdated] = mySpotLights[lightUpdated]->myLight->GetMaxDistance();
	//		lightInnerFallofs[lightUpdated] = mySpotLights[lightUpdated]->myLight->GetInnerFallofAngle();
	//		lightsOuterFallofs[lightUpdated] = mySpotLights[lightUpdated]->myLight->GetOuterFallofAngle();
	//		nbrOfLights++;
	//	}
	//}
	//Engine::GetInstance()->GetEffectInput().UpdateSpotLights(lightPositions, lightColours, lightMaxDistnaces
	//	, lightVectors, lightInnerFallofs, lightsOuterFallofs, nbrOfLights);
}

void Instance::BuildHierachy( TransformationNodeInstance& aHierarchy, Model* aModel )
{
	const int nbrOfChildren = aModel->myChildsTransformations.Count();
	for ( int copyCreated = 0; copyCreated < nbrOfChildren; copyCreated++)
	{
		TransformationNodeInstance* newChildNode = new TransformationNodeInstance();
		newChildNode->SetTransformationNode(aModel->myChildsTransformations[copyCreated]);
		BuildHierachy( (*newChildNode) , myModel.myChilds[copyCreated]);

		aHierarchy.AddChildNode(newChildNode);
	}
}	

void Instance::Update( float aElapsedTime )
{
	myHierarchy.Update(aElapsedTime);
	if(myAnimation!=NULL)
	{
		myAnimation->Update(aElapsedTime);
	}
	if(myBoneInstances.Count()>0)
	{
		Matrix44f identity= Matrix44f::Identity();
		TransformFrame(0,identity);
		for( int i = 0; i < myBoneInstances.Count(); i++ )
		{
			myBoneInstances[i]->ApplyBindPose();
		}

	};
}

void Instance::TransformFrame( int aBoneId, const Matrix44f& aParentWorld )
{
	myBoneInstances[aBoneId]->Transform(aParentWorld);
	for(int i=0; i < myModel.myBones[aBoneId].GetChildren().Count(); i++)
	{
		TransformFrame( myModel.myBones[aBoneId].GetChildren()[i], myBoneInstances[aBoneId]->GetTransformed());
	}
}
