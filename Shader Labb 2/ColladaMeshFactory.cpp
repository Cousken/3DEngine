#include "stdafx.h"
#include "ColladaMeshFactory.h"
#include "Model.h"
#include "EffectContainer.h"
#include "FCollada.h"
#include "Engine.h"
#include "TransformationNode_Hierarchy.h"
#include "TransformationNode_curves.h"
#include "AnimationCurve.h"
#include "BufferWrapper.h"
//#include "N3_Animation.h"

#include "FCDocument\FCDocument.h"
#include "FCDocument\FCDLibrary.h"
#include "FCDocument\FCDGeometryMesh.h"
#include "FCDocument\FCDGeometry.h"
#include "FCDocument\FCDCamera.h"
#include "FCDocument\FCDMaterial.h"

#include "FCDocument\FCDGeometryPolygons.h"
#include "FCDocument\FCDGeometrySource.h"
#include "FCDocument\FCDSceneNode.h" 
#include "FCDocument\FCDLight.h" 
#include "FUtils\FUObject.h" 
#include "FCDocument\FCDControllerInstance.h"
#include "FCDocument\FCDController.h"
#include "FCDocument\FCDSkinController.h"

#include "FCDocument\FCDGeometryInstance.h"
#include "FCDocument\FCDMaterial.h" 
#include "FCDocument\FCDMaterialInstance.h"
#include "FCDocument\FCDLibrary.h"
#include "FCDocument\FCDAnimation.h"
#include "FCDocument\FCDAnimationChannel.h"
#include "FCDocument\FCDAnimationCurve.h"
#include "FCDocument\FCDGeometryPolygonsTools.h"
#include "FCDocument\FCDGeometryPolygonsInput.h"
#include "FCDocument\FCDMaterial.h"
#include "FCDocument\FCDEffect.h"
#include "FCDocument\FCDEffectProfile.h"
#include "FCDocument\FCDEffectStandard.h"
#include "FCDocument\FCDTexture.h"
#include "FCDocument\FCDImage.h"
#include "FCDocument\FCDAnimationCurve.h"
#include "FCDocument\FCDAnimated.h"
#include "FCDocument\FCDAnimation.h"
#include "FCDocument\FCDAnimationKey.h"

#include <map>

ColladaMeshFactory::ColladaMeshFactory()
{
		FCollada::Initialize();
}

ColladaMeshFactory::~ColladaMeshFactory()
{
}

bool ColladaMeshFactory::SelectEffect(Model* aModel)
{
	if(aModel->myIsNULLObject==false)
	{
		Effect* effect=Engine::GetInstance()->GetEffectContainer().GetEffect(ShaderFiles[aModel->myVertexType]);
		if(effect==NULL)
		{
			return(false);
		}
		aModel->SetEffect(effect);
		
	};
	return(true);
}


Model* ColladaMeshFactory::CreateModel(std::string aFileName)
{
	Model* tempModel = new Model();
	parentModel=tempModel;
	if(!tempModel || (LoadModel(tempModel,aFileName)==false))
	{
		delete tempModel;
		tempModel=NULL;

	}
	return(tempModel);
}

bool ColladaMeshFactory::LoadModel(Model* aModel,std::string aFileName)
{
	FCDocument* myColladaDocument;

	myColladaDocument=FCollada::NewTopDocument();
	bool sucess=FCollada::LoadDocumentFromFile(myColladaDocument,aFileName.c_str() );	

	if(sucess==false) return(false);
	if(myColladaDocument==NULL) return(false);

/*
	if(myColladaDocument->GetAsset()->GetUpAxis().z==1)
	{
		// TO DO take care of models with diffeent rotations 
	}
	else
	{
		// TO DO take care of models with diffeent rotations 
	}
*/
	FCDGeometryLibrary* geometryLibrary=myColladaDocument->GetGeometryLibrary();

	unsigned int numberOfMeshes=0;
	for (unsigned int i=0; i< geometryLibrary->GetEntityCount(); i++)
	{
		if (geometryLibrary->GetEntity(i)->IsMesh()==true) 
		{
			numberOfMeshes++;
		}
	}

	BuildHierarchy(myColladaDocument,aModel);



	FCDLightLibrary* lightLibrary=myColladaDocument->GetLightLibrary();
	unsigned int numberOfLights=lightLibrary->GetEntityCount();
	for(unsigned int i=0;i<numberOfLights;i++)
	{
		FCDLight* light;
		light=lightLibrary->GetEntity(i);
		if(light->GetLightType()==FCDLight::AMBIENT)
		{
			// TO DO HANDLE LOADING OF AMBIENT LIGHT
		}
		if(light->GetLightType()==FCDLight::SPOT)
		{
			// TO DO HANDLE LOADING OF SPOT LIGHT
		}
		if(light->GetLightType()==FCDLight::DIRECTIONAL)
		{
			// TO DO HANDLE LOADING OF DIRECTIONAL LIGHT
		}
		if(light->GetLightType()==FCDLight::POINT)
		{
			// TO DO HANDLE LOADING OF POINT LIGHT
		}
	}

/*	FCDImageLibrary* imageLibrary=myColladaDocument->GetImageLibrary();
	unsigned int numTextures= imageLibrary->GetEntityCount();
	
	for(unsigned int i=0;i<numTextures;i++)
	{
		FCDImage* image=imageLibrary->GetEntity(i);
		// TO DO RESOLVE TEXTURE LOADING
		assert(0);
	}

	FCDMaterialLibrary* materialLibrary=myColladaDocument->GetMaterialLibrary();
	unsigned int numMaterials= materialLibrary->GetEntityCount();
	for(unsigned int i=0;i<numMaterials;i++)
	{
		FCDMaterial* material=materialLibrary->GetEntity(i);
		
		// TO DO RESOLVE MATERIAL LOADING
		//assert(0);
	}
*/
	FCDCameraLibrary* cameraLibrary=myColladaDocument->GetCameraLibrary();
	unsigned int numCameras=cameraLibrary->GetEntityCount();
	for(unsigned int i=0;i<numCameras;i++)
	{
		FCDCamera* camera=cameraLibrary->GetEntity(i);
		if (camera->GetProjectionType() == FCDCamera::PERSPECTIVE) 
		{
			// TO DO RESOLVE PERSPECTIVE CAMERAS
		}
		if (camera->GetProjectionType() == FCDCamera::ORTHOGRAPHIC)
		{
			// TO DO RESOLVE ORTHOGRAPHIC CAMERAS
		}
	}

	FCDAnimationLibrary* animationLibrary=myColladaDocument->GetAnimationLibrary();
	unsigned int numAnimations=animationLibrary->GetEntityCount();
	for(unsigned int i=0;i<numAnimations;i++)
	{
		// TO DO RESOLVE ANIMATIONS
	}

	aModel->SetFileName(aFileName);
/*	if(aModel->myBones.Count()>0)
	{
		aModel->TransformBindPose(CU::Matrix44f::Identity());
	}*/
	CreateShadowMesh(aModel);
	return(true);
}


void ColladaMeshFactory::BuildHierarchy(FCDocument* aColladaDocument,Model* aModel)
{
	FCDSceneNode* sceneRoot=aColladaDocument->GetVisualSceneInstance();
	assert(sceneRoot!=NULL);

	assert(sceneRoot->GetTransformCount()==0);
	assert(sceneRoot->GetType()==FCDEntity::SCENE_NODE);

	assert(sceneRoot->GetInstanceCount()<2);

	if(sceneRoot->GetInstanceCount()==0)
	{
		aModel->myIsNULLObject=true;
		aModel->myOrientation.myData[(0 * 4) + 0]=-1;
		aModel->myOrientation.myData[(2 * 4) + 2]=-1;
	}
	else
	{
		if(sceneRoot->GetInstance(0)->GetEntityType()==FCDEntity::LIGHT)
		{
			// TO DO DECODE LIGHTS PROPERLY
		}
		else
		if(sceneRoot->GetInstance(0)->GetEntityType()==FCDEntity::CAMERA)
		{
			// TO DO DECODE LIGHTS PROPERLY
		}
		else
		if(sceneRoot->GetInstance(0)->GetEntityType()==FCDEntity::GEOMETRY)
		{
			FindAndDecodeInstance(aModel,sceneRoot->GetInstance(0),aColladaDocument,false);
		}
		else
		{
			assert(0 && "UNSUPPORTED INSTANCE TYPE");
		}

	}

	for(unsigned int childIndex=0;childIndex<sceneRoot->GetChildrenCount();childIndex++)
	{
		FCDSceneNode* childNode;
		Model* childModel= new Model();
		childNode=sceneRoot->GetChild(childIndex);
		aModel->AddChild(childModel,GetTransForm(childNode));
		BuildHierarchyRecursive(aColladaDocument,childNode,childModel);
	}
}



bool IsRealTransform(FCDTransform* aTransform)
{
	if(aTransform->IsAnimated())
	{
		return(true);
	}
	if(aTransform->GetType()==FCDTransform::ROTATION)
	{
		FCDTRotation* rotation=dynamic_cast<FCDTRotation*>(aTransform);
		if(rotation->GetAngle()!=0) 		return(true);
	}

	if(aTransform->GetType()==FCDTransform::TRANSLATION)
	{
		FCDTTranslation* translation=dynamic_cast<FCDTTranslation*>(aTransform);
		FMVector3 vec=translation->GetTranslation();
		if(vec.LengthSquared()!=0)
		{
			return(true);
		}
	}
	return(false);
}


void ColladaMeshFactory::ApplayTransformationRotation(FCDTransform* aTransform, Matrix44f& aOrientation)
{
	FCDTRotation* rotation=dynamic_cast<FCDTRotation*>(aTransform);
	Matrix44f rotMat;
	rotMat.ConvertD3D(&rotation->ToMatrix().m[0][0]);
	
	rotMat*=aOrientation;
	aOrientation=rotMat;
//	aOrientation*=rotMat;
}

void ColladaMeshFactory::ApplayTransformationTranslation(FCDTransform* aTransform, Matrix44f& aOrientation)
{
	FCDTTranslation* rotation=dynamic_cast<FCDTTranslation*>(aTransform);
	Matrix44f rotMat;
	rotMat.ConvertD3D(&rotation->ToMatrix().m[0][0]);
	
	
	rotMat*=aOrientation;
	aOrientation=rotMat;
	//aOrientation*=rotMat;
}


TransformationNode_Curves* ColladaMeshFactory::GetRotationTransformation(FCDTransform* aTransform)
{
	CU::Vector3f baseAxis(0,0,1);
	float baseAngle=0;

	assert(aTransform->IsAnimated());
	TransformationNode_Curves* node=new TransformationNode_Curves();

	FCDTRotation* rotation=dynamic_cast<FCDTRotation*>(aTransform);
	FMVector3 point=rotation->GetAxis();
	baseAngle=rotation->GetAngle();
	
	baseAxis.x=point.x;
	baseAxis.y=point.y;
	baseAxis.z=point.z;
	FCDAnimationCurve* rotXCurve;
	FCDAnimationCurve* rotYCurve;
	FCDAnimationCurve* rotZCurve;
	FCDAnimationCurve* rotAngleCurve;

	// look for x animation
	rotXCurve=rotation->GetAnimated()->FindCurve(".X");
	
	// look for y animation
	rotYCurve=rotation->GetAnimated()->FindCurve(".Y");

	// look for z animation
	rotZCurve=rotation->GetAnimated()->FindCurve(".Z");

	// look for angle animation
	rotAngleCurve=rotation->GetAnimated()->FindCurve(".ANGLE");

	if(rotXCurve!=NULL)			
	{
		node->SetRotationCurve(0,CreateAnimationCurve(rotXCurve));
	}
	if(rotYCurve!=NULL)			
	{
		node->SetRotationCurve(1,CreateAnimationCurve(rotYCurve));
	}
	if(rotZCurve!=NULL)			
	{
		node->SetRotationCurve(2,CreateAnimationCurve(rotZCurve));
	}
	if(rotAngleCurve!=NULL)			
	{
		node->SetRotationCurve(3,CreateAnimationCurveRot(rotAngleCurve));
	}
	node->SetBaseFrame(baseAxis,CU::Vector3f(0,0,0),baseAngle);
	return(node);
}


TransformationNode_Curves* ColladaMeshFactory::GetTranslationTransform(FCDTransform* aTransform)
{
	CU::Vector3f baseTranslation(0,0,0);


	assert(aTransform->IsAnimated());
	TransformationNode_Curves* node=new TransformationNode_Curves();

	FCDTTranslation* translatation=dynamic_cast<FCDTTranslation*>(aTransform);

	FMVector3 point=translatation->GetTranslation();

	baseTranslation.x=point.x;
	baseTranslation.y=point.y;
	baseTranslation.z=point.z;
	FCDAnimationCurve* curve;

		// look for x animation
	curve=translatation->GetAnimated()->FindCurve(".X");
	if(curve!=NULL)			
	{
		node->SetTranslationCurve(0,CreateAnimationCurve(curve));
	}

	// look for y animation
	curve=translatation->GetAnimated()->FindCurve(".Y");
	if(curve!=NULL)			
	{
		node->SetTranslationCurve(1,CreateAnimationCurve(curve));
	}

	// look for z animation
	curve=translatation->GetAnimated()->FindCurve(".Z");
	if(curve!=NULL)			
	{
		node->SetTranslationCurve(2,CreateAnimationCurve(curve));
	}
	node->SetBaseFrame(CU::Vector3f(1,0,0),baseTranslation,0);
	return(node);
}


TransformationNode* ColladaMeshFactory::GetTransForm(FCDSceneNode* aSceneNode)
{
	Matrix44f transformation= Matrix44f::Identity();

	TransformationNode* returnNode=NULL;
	TransformationNode* transformationNode=NULL;
	TransformationNode_Hierarchy* transformationNodeHierarachy=NULL;
		
	int transformationFrames=0;
	for(unsigned int i=0;i<aSceneNode->GetTransformCount();i++)
	{
		FCDTransform* transform=aSceneNode->GetTransform(i);

		if(IsRealTransform(transform)==true)
		{
			if(transform->IsAnimated())
			{
				if((transformationFrames>0) || (returnNode!=NULL))
				{
					if(transformationNodeHierarachy==NULL)
					{
						transformationNodeHierarachy= new TransformationNode_Hierarchy();
						if(returnNode!=NULL)
						{
							transformationNodeHierarachy->AddTransformation(returnNode);
						}
					}
					returnNode=transformationNodeHierarachy;
					if(transformationFrames>0)
					{
						transformationNode= new TransformationNode();
						transformationNode->SetOrientation(transformation);
						transformationNodeHierarachy->AddTransformation(transformationNode);
					}
				}
				
				if(transform->GetType()==FCDTransform::ROTATION)
				{
					if(transformationNodeHierarachy!=NULL)
					{
						transformationNodeHierarachy->AddTransformation(GetRotationTransformation(transform));
					}
					else
					{
						returnNode=GetRotationTransformation(transform);
					}
				}
				if(transform->GetType()==FCDTransform::TRANSLATION)
				{
					if(transformationNodeHierarachy!=NULL)
					{
						transformationNodeHierarachy->AddTransformation(GetTranslationTransform(transform));
					}
					else
					{
						returnNode=GetTranslationTransform(transform);
					}
				}
				transformation = Matrix44f::Identity();
				transformationFrames=0;
			}
			else
			{
				if(transform->GetType()==FCDTransform::ROTATION)
				{
					ApplayTransformationRotation(transform,transformation);
					transformationFrames++;
				}

				if(transform->GetType()==FCDTransform::TRANSLATION)
				{
					ApplayTransformationTranslation(transform,transformation);
					transformationFrames++;
				}

			}
			if(transform->GetType()==FCDTransform::SCALE)
			{
				// TO DO LATER RESOLVE SCALING
				assert(0 && "SCALING NOT IMPLEMENTED");
			}
		}
	}
	if(transformationFrames>0)
	{
		if(returnNode==NULL)
		{
			transformationNode= new TransformationNode();
			transformationNode->SetOrientation(transformation);
			return(transformationNode);
		}
		if(transformationNodeHierarachy==NULL)
		{
			transformationNodeHierarachy= new TransformationNode_Hierarchy();
			if(returnNode!=NULL)
			{
				transformationNodeHierarachy->AddTransformation(returnNode);
			}
		}
		returnNode=transformationNodeHierarachy;
		transformationNode= new TransformationNode();
		transformationNode->SetOrientation(transformation);
		transformationNodeHierarachy->AddTransformation(transformationNode);
	}
	if(returnNode==0)
	{
		transformationNode= new TransformationNode();
		transformationNode->SetOrientation(transformation);
		returnNode=transformationNode;

	}
	return(returnNode);
}


AnimationCurve* ColladaMeshFactory::CreateAnimationCurve(FCDAnimationCurve* aCurve)
{
	AnimationCurve* animCurve;
	int nrOfKeys= aCurve->GetKeyCount();

	float startTime=aCurve->GetKey(0)->input;
	float endTime=aCurve->GetKey(nrOfKeys-1)->input;
	
	float fps=20; // frecuency is 1/fps
	float currTime;
	currTime=startTime;

	animCurve = new AnimationCurve();
	while (currTime<endTime) 
	{
		KeyFrame temp;
		temp.myTime=currTime;
		temp.myValue=(aCurve->Evaluate(currTime));
		currTime+=1.0f/fps;
		animCurve->AddKeyFrame(temp);
	}
	KeyFrame temp;
	temp.myTime=endTime;
	temp.myValue=aCurve->Evaluate(endTime);
	animCurve->AddKeyFrame(temp);
	animCurve->FinalizeCurve();
	return(animCurve);
}


AnimationCurve* ColladaMeshFactory::CreateAnimationCurveRot(FCDAnimationCurve* aCurve)
{
	AnimationCurve* animCurve;
	int nrOfKeys= aCurve->GetKeyCount();

	float startTime=aCurve->GetKey(0)->input;
	float endTime=aCurve->GetKey(nrOfKeys-1)->input;
	
	float fps=20; // frecuency is 1/fps
	float currTime;
	currTime=startTime;

	animCurve = new AnimationCurve();
	while (currTime<endTime) 
	{
		KeyFrame temp;
		temp.myTime=currTime;
		temp.myValue=(aCurve->Evaluate(currTime))*3.14159265/180.0;
		currTime+=1.0f/fps;
		animCurve->AddKeyFrame(temp);
	}
	KeyFrame temp;
	temp.myTime=endTime;
	temp.myValue=aCurve->Evaluate(endTime);
	animCurve->AddKeyFrame(temp);
	animCurve->FinalizeCurve();
	return(animCurve);
}

void ColladaMeshFactory::BuildBoneHierarchy(Model* aModel,FCDocument* aColladaDocument, FCDSceneNode* aSceneNode,int aParentIndex,FCDController* aController, FCDControllerInstance* aControllerInstance)
{
	/*
	Bone tempBone;
	int boneIndex = parentModel->myBones.Count();

	assert(aSceneNode!=NULL);

	assert(aSceneNode->GetType()==FCDEntity::SCENE_NODE);

	assert(aSceneNode->GetInstanceCount()<2);
	bool validBone=false;
	
	
	TransformationNode* testNode=NULL;


	

	if(aSceneNode->IsJoint()==true)
	{
		if(parentModel->myAnimation==NULL)
		{
			parentModel->myAnimation=new Animation();
			parentModel->myAnimation->Init(parentModel->myBones.Count());
		}

		testNode=GetTransForm(aSceneNode);
		if(aParentIndex!=-1)
		{
			parentModel->myBones[aParentIndex].AddChild(boneIndex);
		}
		FCDSkinController* skinControl=NULL;
		skinControl=aController->GetSkinController();
		for(int i=0;i<skinControl->GetJointCount();i++)
		{
			if(skinControl->GetJoint(i)->GetId()==aSceneNode->GetDaeId())
			{


				assert(i==boneIndex);
				parentModel->myBones.Add(tempBone);
				aModel->myBoneList.Add(boneIndex);
				validBone=true;
				FMMatrix44 invBindPos= skinControl->GetJoint(boneIndex)->GetBindPoseInverse();
				FMMatrix44 bindPos= invBindPos.Inverted();
				CU::Matrix44f matrix;
				matrix.ConvertD3D(&bindPos.m[0][0]);
				parentModel->myBones[boneIndex].SetBindPose(matrix);
				FCDAnimationLibrary*  animationLibrary = aColladaDocument->GetAnimationLibrary();
				bool isAnimated=false;
				for(int animationIndex=0;animationIndex<animationLibrary->GetEntityCount();animationIndex++)
				{
					FCDEntity* entity=animationLibrary->GetEntity(animationIndex);
					fm::string name=entity->GetDaeId();
					name.erase(skinControl->GetJoint(i)->GetId().length(),name.length());
					if(name==skinControl->GetJoint(i)->GetId())
					{
						TransformationNode_Curves* transformationNode=NULL;
						transformationNode=new TransformationNode_Curves();
						isAnimated=true;

						testNode=GetTransForm(aSceneNode);
						FCDAnimation* anim= dynamic_cast<FCDAnimation*>(entity);
						// THIS IS MY ANIMATION PREPARE FOR GLORY
						fm::string name=entity->GetDaeId();
						name.erase(0,skinControl->GetJoint(i)->GetId().length()+1);
						name.erase(name.length()-(skinControl->GetJoint(i)->GetId().length()+1),name.length());
						// The hack of the century but it WORKS !
						if(name=="translate")
						{
							FCDAnimationChannel* channel;
							for(int channelIndex=0;channelIndex<anim->GetChannelCount();channelIndex++)
							{
								channel=anim->GetChannel(channelIndex);
								assert(channel->GetCurveCount()==1);
								FCDAnimationCurve* curve=channel->GetCurve(0);
								if(channel->myQualifier==".X")
								{
									transformationNode->SetTranslationCurve(0,CreateAnimationCurve(curve));
								}
								else
								if(channel->myQualifier==".Y")
								{
									transformationNode->SetTranslationCurve(1,CreateAnimationCurve(curve));
								}
								else
								if(channel->myQualifier==".Z")
								{
									transformationNode->SetTranslationCurve(2,CreateAnimationCurve(curve));
								}
								else
									assert(0);
								}
						}
						else
						if((name=="rotateX") || (name=="rotateY") || (name=="rotateZ"))
						{
							FCDAnimationChannel* channel;
							for(int channelIndex=0;channelIndex<anim->GetChannelCount();channelIndex++)
							{
								channel=anim->GetChannel(channelIndex);
								assert(channel->GetCurveCount()==1);
								FCDAnimationCurve* curve=channel->GetCurve(0);

								if(channel->myQualifier==".X")
								{
									transformationNode->SetRotationCurve(0,CreateAnimationCurve(curve));
								}
								else
								if(channel->myQualifier==".Y")
								{
									transformationNode->SetRotationCurve(1,CreateAnimationCurve(curve));
								}
								else
								if(channel->myQualifier==".Z")
								{
									transformationNode->SetRotationCurve(2,CreateAnimationCurve(curve));
								}
								else
								if(channel->myQualifier==".ANGLE")
								{
									transformationNode->SetRotationCurve(3,CreateAnimationCurveRot(curve));
								}
								else
									assert(0);
							}
						}
						else
							assert(0);
						if(parentModel->myAnimation==NULL)
						{
							parentModel->myAnimation=new Animation();
							parentModel->myAnimation->Init(parentModel->myBones.Count());
						}
						transformationNode->SetBaseFrame(CU::Vector3f(0,0,1),CU::Vector3f(0,0,0),0);

//						parentModel->myAnimation->AddTransformationNode(transformationNode);
						CU::Matrix44f test=testNode->GetTransformationForTime(0);
						test*=parentModel->myAnimation->GetTransformationNodes()[0]->GetTransformationForTime(0);

	//					parentModel->myAnimation->AddTransformationNode(testNode);

					TransformationNode* node=new TransformationNode();

	
					node->SetOrientation(test); // THIS IS BONE MATRIX WEIRD ???
					parentModel->myAnimation->AddTransformationNode(testNode);


					}
				}
				if(isAnimated==false)
				{
					if(parentModel->myAnimation==NULL)
					{
						parentModel->myAnimation=new Animation();
						parentModel->myAnimation->Init(parentModel->myBones.Count());
					}

					TransformationNode* node=new TransformationNode();


					node->SetOrientation(matrix);
					parentModel->myAnimation->AddTransformationNode(testNode);
				}

			}
		}
	}
	if(parentModel->myBones.Count()==0) boneIndex=-1;
	for(unsigned int childIndex=0;childIndex<aSceneNode->GetChildrenCount();childIndex++)
	{
		FCDSceneNode* childNode;
		childNode=aSceneNode->GetChild(childIndex);

//		if(childNode->IsJoint()==true)
		{
			BuildBoneHierarchy(aModel,aColladaDocument,childNode,boneIndex,aController,aControllerInstance);
		}
	}
*/
}

void ColladaMeshFactory::BuildHierarchyRecursive(FCDocument* aColladaDocument, FCDSceneNode* aSceneNode,Model* aModel)
{
	assert(aSceneNode!=NULL);

	assert(aSceneNode->GetType()==FCDEntity::SCENE_NODE);

	assert(aSceneNode->GetInstanceCount()<2);



	if(aSceneNode->GetInstanceCount()==0)
	{
		aModel->myIsNULLObject=true;
	}
	else
	{
		if(aSceneNode->GetInstance(0)->GetType()==FCDEntityInstance::GEOMETRY)
		{
			FindAndDecodeInstance(aModel,aSceneNode->GetInstance(0),aColladaDocument,false);
		}
		else
		{
			if(aSceneNode->GetInstance(0)->GetType()==FCDEntityInstance::CONTROLLER)
			{
				FCDControllerInstance* inst = static_cast<FCDControllerInstance*>(aSceneNode->GetInstance(0));
				FindAndDecodeInstance(aModel,inst,aColladaDocument,true);


				FCDControllerLibrary* controllerLibrary=aColladaDocument->GetControllerLibrary();

				for (unsigned int i=0; i< controllerLibrary->GetEntityCount(); i++)
				{
					if(controllerLibrary->GetEntity(i)->GetDaeId()==aSceneNode->GetInstance(0)->GetEntity()->GetDaeId())
					{
//						BuildBoneHierarchy(aModel,aColladaDocument,aColladaDocument->GetVisualSceneInstance(),-1,dynamic_cast<FCDController*>(controllerLibrary->GetEntity(i)),dynamic_cast<FCDControllerInstance*>(aSceneNode->GetInstance(0)));
					}
				}

				
			}
		}
	}
	for(unsigned int childIndex=0;childIndex<aSceneNode->GetChildrenCount();childIndex++)
	{
		FCDSceneNode* childNode;
		childNode=aSceneNode->GetChild(childIndex);

		if(childNode->IsJoint()==false)
		{
			Model* childModel= new Model();
			aModel->AddChild(childModel,GetTransForm(childNode));
			BuildHierarchyRecursive(aColladaDocument,childNode,childModel);
		}
	}
}



void ColladaMeshFactory::FindAndDecodeInstance(Model* aModel,FCDEntityInstance* aInstance,FCDocument* aColladaDocument,bool aHasBoneWeightsFlag)
{
	FCDGeometryLibrary* geometryLibrary=aColladaDocument->GetGeometryLibrary();
	FCDControllerLibrary* controllerLibrary=aColladaDocument->GetControllerLibrary();

	unsigned int numberOfMeshes=0;
	for (unsigned int i=0; i< geometryLibrary->GetEntityCount(); i++)
	{
		if (geometryLibrary->GetEntity(i)->IsMesh()==true) 
		{
			if(geometryLibrary->GetEntity(i)->GetDaeId()==aInstance->GetEntity()->GetDaeId())
			{
				DecodeInstance(aModel,geometryLibrary->GetEntity(i),dynamic_cast<FCDGeometryInstance*>(aInstance),aColladaDocument,false,NULL,NULL);
				// GET READY TO LOAD
				SelectEffect(aModel);

			}
		}
	}
	for (unsigned int i=0; i< controllerLibrary->GetEntityCount(); i++)
	{
		if(controllerLibrary->GetEntity(i)->GetDaeId()==aInstance->GetEntity()->GetDaeId())
		{
			for (unsigned int j=0; j< geometryLibrary->GetEntityCount(); j++)
			{
				if(geometryLibrary->GetEntity(i)->GetDaeId()==controllerLibrary->GetEntity(i)->GetBaseTarget()->GetDaeId())
				{
					DecodeInstance(aModel,geometryLibrary->GetEntity(i),dynamic_cast<FCDGeometryInstance*>(aInstance),aColladaDocument,true,dynamic_cast<FCDController*>(controllerLibrary->GetEntity(i)),dynamic_cast<FCDControllerInstance*>(aInstance));
					// GET READY TO LOAD
					SelectEffect(aModel);
					
				}
			}
		}
	}
}


bool ColladaMeshFactory::DetermineVertexType(Model* aModel)
{
	if(strcmp(aModel->myVertexFormat[0].SemanticName,"POSITION")!=0)  return(false);
	if(aModel->myVertexFormat[0].Format!=DXGI_FORMAT_R32G32B32_FLOAT) return(false);
	if(aModel->myVertexFormat.Count()==2)
	{
		if(strcmp(aModel->myVertexFormat[1].SemanticName,"TEXCOORD")!=0)  return(false);
		if(aModel->myVertexFormat[1].Format!=DXGI_FORMAT_R32G32_FLOAT) return(false);
		aModel->myVertexType=VertexTypePosUV;
		return(true);
	}
	if(aModel->myVertexFormat.Count()==3)
	{
		if(strcmp(aModel->myVertexFormat[2].SemanticName,"TEXCOORD")!=0)  return(false);
		if(aModel->myVertexFormat[2].Format!=DXGI_FORMAT_R32G32_FLOAT) return(false);


		if(strcmp(aModel->myVertexFormat[1].SemanticName,"NORMAL")==0)
		{
			if(aModel->myVertexFormat[1].Format!=DXGI_FORMAT_R32G32B32_FLOAT) return(false);
			aModel->myVertexType=VertexTypePosNormUV;
			return(true);
		}
		else
		if(strcmp(aModel->myVertexFormat[1].SemanticName,"COLOR")==0)
		{
			if(aModel->myVertexFormat[1].Format!=DXGI_FORMAT_R32G32B32A32_FLOAT) return(false);
			aModel->myVertexType=VertexTypePosColUV;
			return(true);
		}
	}
	if(aModel->myVertexFormat.Count()==4)
	{
		if(strcmp(aModel->myVertexFormat[1].SemanticName,"NORMAL")!=0) return(false);
		if(aModel->myVertexFormat[1].Format!=DXGI_FORMAT_R32G32B32_FLOAT) return(false);

		if(strcmp(aModel->myVertexFormat[2].SemanticName,"TEXCOORD")!=0)  return(false);
		if(aModel->myVertexFormat[2].Format!=DXGI_FORMAT_R32G32_FLOAT) return(false);
	
		if(strcmp(aModel->myVertexFormat[3].SemanticName,"TANGENT")!=0)  return(false);
		if(aModel->myVertexFormat[3].Format!=DXGI_FORMAT_R32G32B32_FLOAT) return(false);

		aModel->myVertexType=VertexTypePosNormUVTan;
		return(true);

	}
	if(aModel->myVertexFormat.Count()==5)
	{
		if(strcmp(aModel->myVertexFormat[1].SemanticName,"NORMAL")!=0) return(false);
		if(aModel->myVertexFormat[1].Format!=DXGI_FORMAT_R32G32B32_FLOAT) return(false);

		if(strcmp(aModel->myVertexFormat[2].SemanticName,"TEXCOORD")!=0)  return(false);
		if(aModel->myVertexFormat[2].Format!=DXGI_FORMAT_R32G32_FLOAT) return(false);
	
		if(strcmp(aModel->myVertexFormat[3].SemanticName,"TANGENT")!=0)  return(false);
		if(aModel->myVertexFormat[3].Format!=DXGI_FORMAT_R32G32B32_FLOAT) return(false);

		if(strcmp(aModel->myVertexFormat[4].SemanticName,"BINORMAL")!=0)  return(false);
		if(aModel->myVertexFormat[4].Format!=DXGI_FORMAT_R32G32B32_FLOAT) return(false);

		aModel->myVertexType=VertexTypePosNormUVTanBiNorm;
		return(true);

	}
	if(aModel->myVertexFormat.Count()==5)
	{

		if(strcmp(aModel->myVertexFormat[1].SemanticName,"WEIGHTS")!=0) return(false);
		if(aModel->myVertexFormat[1].Format!=DXGI_FORMAT_R8G8B8A8_UNORM) return(false);
		if(strcmp(aModel->myVertexFormat[2].SemanticName,"BONES")!=0) return(false);
		if(aModel->myVertexFormat[2].Format!=DXGI_FORMAT_R8G8B8A8_UINT) return(false);
		if(strcmp(aModel->myVertexFormat[3].SemanticName,"NORMAL")!=0) return(false);
		if(aModel->myVertexFormat[3].Format!=DXGI_FORMAT_R32G32B32_FLOAT) return(false);

		if(strcmp(aModel->myVertexFormat[4].SemanticName,"TEXCOORD")!=0)  return(false);
		if(aModel->myVertexFormat[4].Format!=DXGI_FORMAT_R32G32_FLOAT) return(false);
	
		aModel->myVertexType=VertexTypePosWeightBonesNormUV;
		return(true);

	}


	if(aModel->myVertexFormat.Count()==6)
	{

		if(strcmp(aModel->myVertexFormat[1].SemanticName,"WEIGHTS")!=0) return(false);
		if(aModel->myVertexFormat[1].Format!=DXGI_FORMAT_R8G8B8A8_UNORM) return(false);
		if(strcmp(aModel->myVertexFormat[2].SemanticName,"BONES")!=0) return(false);
		if(aModel->myVertexFormat[2].Format!=DXGI_FORMAT_R8G8B8A8_UINT) return(false);
		if(strcmp(aModel->myVertexFormat[3].SemanticName,"NORMAL")!=0) return(false);
		if(aModel->myVertexFormat[3].Format!=DXGI_FORMAT_R32G32B32_FLOAT) return(false);

		if(strcmp(aModel->myVertexFormat[4].SemanticName,"TEXCOORD")!=0)  return(false);
		if(aModel->myVertexFormat[4].Format!=DXGI_FORMAT_R32G32_FLOAT) return(false);
	
		if(strcmp(aModel->myVertexFormat[5].SemanticName,"TANGENT")!=0)  return(false);
		if(aModel->myVertexFormat[5].Format!=DXGI_FORMAT_R32G32B32_FLOAT) return(false);

		aModel->myVertexType=VertexTypePosWeightBonesNormUVTan;
		return(true);

	}

	return(false);
}


void ColladaMeshFactory::DecodeInstance(Model* aModel,FCDGeometry* aGeometry,FCDGeometryInstance* aGeometryInstance,FCDocument* aColladaDocument,bool aHasBoneWeightsFlag,FCDController* aController,FCDControllerInstance* aControllerInstance)
{
	FCDGeometryMesh* mesh=aGeometry->GetMesh();
	if (!mesh->IsTriangles()) FCDGeometryPolygonsTools::Triangulate(mesh);
	
	FCDGeometrySource* positionSource=mesh->FindSourceByType(FUDaeGeometryInput::POSITION);
	FCDGeometrySource* normalSource=mesh->FindSourceByType(FUDaeGeometryInput::NORMAL);
	FCDGeometrySource* geoTangentSource=mesh->FindSourceByType(FUDaeGeometryInput::GEOTANGENT);
	FCDGeometrySource* texTangentSource=mesh->FindSourceByType(FUDaeGeometryInput::TEXTANGENT);
	FCDGeometrySource* colorSource=mesh->FindSourceByType(FUDaeGeometryInput::COLOR);
	FCDGeometrySource* textureCordinatesSource=mesh->FindSourceByType(FUDaeGeometryInput::TEXCOORD);
	FCDGeometrySource* geoBiNormalSource=mesh->FindSourceByType(FUDaeGeometryInput::GEOBINORMAL);
	FCDGeometrySource* texBiNormalSource=mesh->FindSourceByType(FUDaeGeometryInput::TEXBINORMAL);

	assert(positionSource);

	int numVertices=positionSource->GetValueCount();

	int elementCount=0;
	int totalStride;
	int positionStride;
	int normalStride;
	int colorStride;
	int textureCordinatesStride;
	int tangentStride;

	if(positionSource!=NULL)
	{
		D3D10_INPUT_ELEMENT_DESC element= { "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, elementCount, D3D10_INPUT_PER_VERTEX_DATA, 0 };
		aModel->myVertexFormat.Add(element);
		positionStride=elementCount;

		elementCount+=12;
	}

/*	if(aHasBoneWeightsFlag)
	{
        D3D10_INPUT_ELEMENT_DESC element={ "WEIGHTS", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0,      elementCount, D3D10_INPUT_PER_VERTEX_DATA, 0 };
        aModel->myVertexFormat.Add(element);
		elementCount+=4;
		D3D10_INPUT_ELEMENT_DESC element2={ "BONES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0,         elementCount, D3D10_INPUT_PER_VERTEX_DATA, 0 };
		aModel->myVertexFormat.Add(element2);
		elementCount+=4;
	}*/

	if(normalSource!=NULL)
	{
		D3D10_INPUT_ELEMENT_DESC element= { "NORMAL",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, elementCount, D3D10_INPUT_PER_VERTEX_DATA, 0 };
		aModel->myVertexFormat.Add(element);
		normalStride=elementCount;
		elementCount+=12;
	}
	if(colorSource!=NULL)
	{
		D3D10_INPUT_ELEMENT_DESC element= { "COLOR",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, elementCount, D3D10_INPUT_PER_VERTEX_DATA, 0 };
		aModel->myVertexFormat.Add(element);
		colorStride=elementCount;

		elementCount+=16;
	}
	if(textureCordinatesSource!=NULL)
	{
		D3D10_INPUT_ELEMENT_DESC element= { "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT, 0, elementCount, D3D10_INPUT_PER_VERTEX_DATA, 0 };
		aModel->myVertexFormat.Add(element);
		textureCordinatesStride=elementCount;

		elementCount+=8;
	}

	assert(((geoTangentSource!=NULL) && (texTangentSource!=NULL))==false);

	if(geoTangentSource!=NULL)
	{
		D3D10_INPUT_ELEMENT_DESC element= { "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, elementCount, D3D10_INPUT_PER_VERTEX_DATA, 0 };
		aModel->myVertexFormat.Add(element);
		tangentStride=elementCount;
		elementCount+=12;
	}
	if(texTangentSource!=NULL)
	{
		D3D10_INPUT_ELEMENT_DESC element= { "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, elementCount, D3D10_INPUT_PER_VERTEX_DATA, 0 };
		aModel->myVertexFormat.Add(element);
		tangentStride=elementCount;
		elementCount+=12;
	}

	if(geoBiNormalSource!=NULL)
	{
		D3D10_INPUT_ELEMENT_DESC element= { "BINORMAL",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, elementCount, D3D10_INPUT_PER_VERTEX_DATA, 0 };
		aModel->myVertexFormat.Add(element);
		tangentStride=elementCount;
		elementCount+=12;
	}
	if(texBiNormalSource!=NULL)
	{
		D3D10_INPUT_ELEMENT_DESC element= { "BINORMAL",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, elementCount, D3D10_INPUT_PER_VERTEX_DATA, 0 };
		aModel->myVertexFormat.Add(element);
		tangentStride=elementCount;
		elementCount+=12;
	}


	totalStride=elementCount;
	DetermineVertexType(aModel);


	aModel->myVertexData.myStride=totalStride;
	aModel->myVertexData.myType=aModel->myVertexType;

// TO DO ADD SUPPORT FOR MORE SOURCES

	unsigned int polygonalMeshes= mesh->GetPolygonsCount();
	
	assert(polygonalMeshes==1); // how to handle multiple meshes ? handle later

	// To do add support for multiple meshes ?
	for(int i=0;i<polygonalMeshes;i++)
	{
		aModel->myIsNULLObject=false;
		assert(polygonalMeshes<=1);
		GenerateMeshDatafromPolygons(aModel,mesh->GetPolygons(i),mesh,aColladaDocument,aHasBoneWeightsFlag,aController,aControllerInstance);
	
		for (unsigned int k=0; k<aGeometryInstance->GetMaterialInstanceCount(); k++) 
		{
			FCDMaterialInstance* materialInstance;
			materialInstance=aGeometryInstance->GetMaterialInstance(k);
			if(mesh->GetPolygons(i)->GetMaterialSemantic()==materialInstance->GetSemantic())
			{
				LoadMaterial(aModel,materialInstance->GetMaterial(),aColladaDocument);
			}
		}
	}
}

void ColladaMeshFactory::LoadMaterial(Model* aModel,FCDMaterial* aMaterial,FCDocument* aColladaDocument)
{
	Surface surface;
	surface.SetVertexStart(0);
	surface.SetIndexStart(0);
	surface.SetIndexCount(aModel->myVertexIndexData.myNrOfIndexes);

	surface.SetPrimologyType(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	FCDEffect* fx = aMaterial->GetEffect();
	FCDEffectProfile* profile = fx->FindProfile(FUDaeProfileType::COMMON);
	FCDEffectStandard* standardProfile=dynamic_cast<FCDEffectStandard*>(profile);


	if (standardProfile->GetTextureCount(FUDaeTextureChannel::REFLECTION)>0) 
	{
		FCDTexture* texture=standardProfile->GetTexture(FUDaeTextureChannel::REFLECTION,0);
		FCDImage* image=texture->GetImage();
		std::string fileName=image->GetFilename();
		surface.SetEnviromentalCubeMapTexture(fileName);
	}
	if (standardProfile->GetTextureCount(FUDaeTextureChannel::DIFFUSE)>0) 
	{
		FCDTexture* texture=standardProfile->GetTexture(FUDaeTextureChannel::DIFFUSE,0);
		FCDImage* image=texture->GetImage();
		std::string fileName=image->GetFilename();
		surface.SetDiffuseTexture(fileName);
	}
	if (standardProfile->GetTextureCount(FUDaeTextureChannel::BUMP)>0) 
	{
		FCDTexture* texture=standardProfile->GetTexture(FUDaeTextureChannel::BUMP,0);
		FCDImage* image=texture->GetImage();
		std::string fileName=image->GetFilename();
		surface.SetNormalMapTexture(fileName);
	}
	if (standardProfile->GetTextureCount(FUDaeTextureChannel::SPECULAR)>0) 
	{
		FCDTexture* texture=standardProfile->GetTexture(FUDaeTextureChannel::SPECULAR,0);
		FCDImage* image=texture->GetImage();
		std::string fileName=image->GetFilename();
		surface.SetSpecularMapTexture(fileName);
	}
	if (standardProfile->GetTextureCount(FUDaeTextureChannel::AMBIENT)>0) 
	{
		FCDTexture* texture=standardProfile->GetTexture(FUDaeTextureChannel::AMBIENT,0);
		FCDImage* image=texture->GetImage();
		std::string fileName=image->GetFilename();
		surface.SetAmbientOcclusionMapTexture(fileName);
	}



	aModel->mySurfaces.Add(surface);
}


struct VertexIndexmapKey
{
	int myKeys[8];

	VertexIndexmapKey()
	{
		for(int i=0;i<8;i++)
		{
			myKeys[i]=-1;
		}
	}


	bool operator!=(const VertexIndexmapKey& aRightKey)
	{
		for(int i=0;i<8;i++)
		{
			if(myKeys[i]!=aRightKey.myKeys[i]) return(true);
		}
		return(false);
	}

	bool operator==(const VertexIndexmapKey& aRightKey)
	{
		for(int i=0;i<8;i++)
		{
			if(myKeys[i]!=aRightKey.myKeys[i]) return(false);
		}
		return(true);
	}

};

bool operator<(const VertexIndexmapKey& aLeftKey,const VertexIndexmapKey& aRightKey)
{
	for(int i=0;i<8;i++)
	{
		if(aLeftKey.myKeys[i]<aRightKey.myKeys[i])
		{
			return(true);
		}
		else
		{
			if(aLeftKey.myKeys[i]>aRightKey.myKeys[i])
			{
				return(false);
			}
		}
	}
	return(false);
}




void ColladaMeshFactory::GenerateMeshDatafromPolygons(Model* aModel,FCDGeometryPolygons* somePolygons,FCDGeometryMesh* mesh,FCDocument* aColladaDocument,bool aHasBoneWeightsFlag,FCDController* aController,FCDControllerInstance* aControllerInstance)
{
	FCDGeometryPolygonsInput* positionInput=somePolygons->FindInput(FUDaeGeometryInput::POSITION);
	FCDGeometryPolygonsInput* normalInput=somePolygons->FindInput(FUDaeGeometryInput::NORMAL);
	FCDGeometryPolygonsInput* colorInput=somePolygons->FindInput(FUDaeGeometryInput::COLOR);
	FCDGeometryPolygonsInput* texCoordInput=somePolygons->FindInput(FUDaeGeometryInput::TEXCOORD);
	FCDGeometryPolygonsInput* geoTangentInput=somePolygons->FindInput(FUDaeGeometryInput::GEOTANGENT);
	FCDGeometryPolygonsInput* texTangentInput=somePolygons->FindInput(FUDaeGeometryInput::TEXTANGENT);
	FCDGeometryPolygonsInput* geoBiNormalInput=somePolygons->FindInput(FUDaeGeometryInput::GEOBINORMAL);
	FCDGeometryPolygonsInput* texBiNormalInput=somePolygons->FindInput(FUDaeGeometryInput::TEXBINORMAL);


	FCDGeometrySource* positionSource=mesh->FindSourceByType(FUDaeGeometryInput::POSITION);
	FCDGeometrySource* normalSource=mesh->FindSourceByType(FUDaeGeometryInput::NORMAL);
	FCDGeometrySource* colorSource=mesh->FindSourceByType(FUDaeGeometryInput::COLOR);
	FCDGeometrySource* textureCordinatesSource=mesh->FindSourceByType(FUDaeGeometryInput::TEXCOORD);
	FCDGeometrySource* geoTangentSource=mesh->FindSourceByType(FUDaeGeometryInput::GEOTANGENT);
	FCDGeometrySource* texTangentSource=mesh->FindSourceByType(FUDaeGeometryInput::TEXTANGENT);
	FCDGeometrySource* geoBiNormalSource=mesh->FindSourceByType(FUDaeGeometryInput::GEOBINORMAL);
	FCDGeometrySource* texBiNormalSource=mesh->FindSourceByType(FUDaeGeometryInput::TEXBINORMAL);


/*	FCDSkinController* skinControl=NULL;
	if(aHasBoneWeightsFlag==true)
	{
		skinControl=aController->GetSkinController();
		skinControl->ReduceInfluences(4);
	}
*/

	
	int myVertexCount=0;
	int duplicatesCount=0;



	std::map<VertexIndexmapKey,int> vertexList;
	std::map<VertexIndexmapKey,int>::iterator vertexIterator;
	std::vector<VertexIndexmapKey> vertexes;
	aModel->myVertexIndexData.myNrOfIndexes=positionInput->GetIndexCount();
	aModel->myVertexIndexData.myFormat=DXGI_FORMAT_R32_UINT;
	aModel->myVertexIndexData.mySize=aModel->myVertexIndexData.myNrOfIndexes*sizeof(unsigned int);
	
	aModel->myVertexIndexData.myIndexData= new  char [aModel->myVertexIndexData.mySize] ();
	unsigned int* indexArray=reinterpret_cast<unsigned int*>(aModel->myVertexIndexData.myIndexData);

	for(int i=0;i<positionInput->GetIndexCount();i++)
	{
		VertexIndexmapKey key;
		if(positionInput!=NULL)
		{
			key.myKeys[0]=positionInput->GetIndices()[i];
		}
		if(normalInput!=NULL)
		{
			key.myKeys[1]=normalInput->GetIndices()[i];
		}
		if(colorInput!=NULL)
		{
			key.myKeys[2]=colorInput->GetIndices()[i];
		}
		if(texCoordInput!=NULL)
		{
			key.myKeys[3]=texCoordInput->GetIndices()[i];
		}
		if(geoTangentInput!=NULL)
		{
			key.myKeys[4]=geoTangentInput->GetIndices()[i];
		}
		if(texTangentInput!=NULL)
		{
			key.myKeys[5]=texTangentInput->GetIndices()[i];
		}

		if(geoBiNormalInput!=NULL)
		{
			key.myKeys[6]=geoBiNormalInput->GetIndices()[i];
		}

		if(texBiNormalInput!=NULL)
		{
			key.myKeys[7]=texBiNormalInput->GetIndices()[i];
		}


		vertexIterator=vertexList.find(key);
		if(vertexIterator==vertexList.end())
		{
			indexArray[i]=myVertexCount;
			std::pair<VertexIndexmapKey,int> tempPair(key,myVertexCount);
			vertexList.insert(tempPair);
			vertexes.push_back(key);
			myVertexCount++;
			// create indexes
		}
		else
		{
			indexArray[i]=(*vertexIterator).second;
			duplicatesCount++;
		    // Read Created Vertex
		}
	}

	aModel->myVertexData.myNrOfVertexes=myVertexCount;
	aModel->myVertexData.mySize=aModel->myVertexData.myStride*aModel->myVertexData.myNrOfVertexes;
	aModel->myVertexData.myVertexData=new char [aModel->myVertexData.mySize] ();

/*

	

	for(int i=0;i<vertexsInfluence->GetInfluenceCount();i++)
	{
		FCDSkinControllerVertex* vertexsInfluence;
	
		vertexsInfluence=skinControl->GetVertexInfluence(i);

		for(int j=0;j<vertexsInfluence->GetPairCount();j++)
		{
			FCDJointWeightPair pair=vertexsInfluence->GetPair(j);
			
		}
	}
		

	

	for(int i=0;i<skinControl->GetJointCount();i++)
	{
	}


*/

	CU::Vector3f min(1000,1000,1000);
	CU::Vector3f max(-1000,-1000,-1000);

	myVertexCount=0;
	for(int i=0;i<vertexes.size();i++)
	{
		VertexIndexmapKey key=vertexes[i];
		
		int offset=0;
		std::pair<VertexIndexmapKey,int> tempPair(key,myVertexCount);
		vertexList.insert(tempPair);

		// Create Vertex
		if(key.myKeys[0]!=-1)
		{
			CU::Vector3f pos;
			memcpy(&pos,
				&positionSource->GetData()[key.myKeys[0]*positionSource->GetStride()],12);

//			pos.y=-pos.y;
//			pos.z=-pos.z;
			memcpy(&aModel->myVertexData.myVertexData[myVertexCount*aModel->myVertexData.myStride+offset],
				&pos,12);

			offset+=12;
		}
/*		if(aHasBoneWeightsFlag==true)
		{
			assert(key.myKeys[0]!=-1);
			FCDSkinControllerVertex* vertexsInfluence;
		
			vertexsInfluence=skinControl->GetVertexInfluence(key.myKeys[0]);

			int boneData=0;
			int weightData=0;

			assert(vertexsInfluence->GetPairCount()<=4);
			for(int j=0;j<vertexsInfluence->GetPairCount();j++)
			{
				assert(j<4);
				FCDJointWeightPair* pair=vertexsInfluence->GetPair(j);
				
				assert(pair->jointIndex<256);
				boneData |= pair->jointIndex << (j*8);
				
				int weight=static_cast<int>((pair->weight*255.0f+0.5));
				assert(weight>=0);
				assert(weight<=255);
				weightData |= weight << (j*8);
			}
			memcpy(&aModel->myVertexData.myVertexData[myVertexCount*aModel->myVertexData.myStride+offset],&weightData,4);
			offset+=4;
			memcpy(&aModel->myVertexData.myVertexData[myVertexCount*aModel->myVertexData.myStride+offset],&boneData,4);
			offset+=4;

		}*/
		if(key.myKeys[1]!=-1)
		{
			CU::Vector3f normal;
			memcpy(&normal,
				&normalSource->GetData()[key.myKeys[1]*normalSource->GetStride()],12);

//			normal.y=-normal.y;
			memcpy(&aModel->myVertexData.myVertexData[myVertexCount*aModel->myVertexData.myStride+offset],
				&normal,12);
			offset+=12;
		}
		if(key.myKeys[2]!=-1)
		{
			memcpy(&aModel->myVertexData.myVertexData[myVertexCount*aModel->myVertexData.myStride+offset],
				&colorSource->GetData()[key.myKeys[2]*colorSource->GetStride()],16);
			offset+=16;
		}
		if(key.myKeys[3]!=-1)
		{
			CU::Vector2f UV;
			memcpy(&UV,
				&textureCordinatesSource->GetData()[key.myKeys[3]*textureCordinatesSource->GetStride()],8);
			
				UV.v=1.0f-UV.v;

				


			memcpy(&aModel->myVertexData.myVertexData[myVertexCount*aModel->myVertexData.myStride+offset],
				&UV,8);
			offset+=8;
		}
		if(key.myKeys[4]!=-1)
		{
			CU::Vector3f tangent;
			memcpy(&tangent,
				&geoTangentSource->GetData()[key.myKeys[4]*geoTangentSource->GetStride()],12);
//			tangent.y=-tangent.y;
			memcpy(&aModel->myVertexData.myVertexData[myVertexCount*aModel->myVertexData.myStride+offset],
				&tangent,12);
			offset+=12;
		}
		if(key.myKeys[5]!=-1)
		{
			CU::Vector3f tangent;
			memcpy(&tangent,
				&texTangentSource->GetData()[key.myKeys[5]*texTangentSource->GetStride()],12);
//			tangent.y=-tangent.y;
			memcpy(&aModel->myVertexData.myVertexData[myVertexCount*aModel->myVertexData.myStride+offset],
				&tangent,12);
			offset+=12;
		}

		if(key.myKeys[6]!=-1)
		{
			memcpy(&aModel->myVertexData.myVertexData[myVertexCount*aModel->myVertexData.myStride+offset],
				&geoBiNormalSource->GetData()[key.myKeys[6]*geoBiNormalSource->GetStride()],12);
			offset+=12;
		}
		if(key.myKeys[7]!=-1)
		{
			memcpy(&aModel->myVertexData.myVertexData[myVertexCount*aModel->myVertexData.myStride+offset],
				&texBiNormalSource->GetData()[key.myKeys[7]*texBiNormalSource->GetStride()],12);
			offset+=12;
		}

		assert(offset==aModel->myVertexData.myStride);
		myVertexCount++;
	}
}





struct SHADOWVERT
{
    D3DXVECTOR3 Position;
    D3DXVECTOR3 Normal;
};


		struct EdgeMapping
		{
			int m_anOldEdge[2];  // vertex index of the original edge
			int m_aanNewEdge[2][2]; // vertex indexes of the new edge
			// First subscript = index of the new edge
			// Second subscript = index of the vertex for the edge

		public:
				EdgeMapping()
				{
					FillMemory( m_anOldEdge, sizeof( m_anOldEdge ), -1 );
					FillMemory( m_aanNewEdge, sizeof( m_aanNewEdge ), -1 );
				}
		};



int ColladaMeshFactory::FindEdgeInMappingTable( int nV1, int nV2, EdgeMapping* pMapping, int nCount )
{
    for( int i = 0; i < nCount; ++i )
    {
        // If both vertex indexes of the old edge in mapping entry are -1, then
        // we have searched every valid entry without finding a match.  Return
        // this index as a newly created entry.
        if( ( pMapping[i].m_anOldEdge[0] == -1 && pMapping[i].m_anOldEdge[1] == -1 ) ||

            // Or if we find a match, return the index.
            ( pMapping[i].m_anOldEdge[1] == nV1 && pMapping[i].m_anOldEdge[0] == nV2 ) )
        {
            return i;
        }
    }

    return -1;  // We should never reach this line
}



bool ColladaMeshFactory::CreateShadowMesh(Model* aModel)
{
	for(int i=0;i<aModel->myChilds.Count();i++)
	{
		CreateShadowMesh(aModel->myChilds[i]);
	}

	if(aModel->myIsNULLObject==true) return(true);
	int i;
    ID3DX10Mesh* tempMesh = NULL;

	const D3D10_INPUT_ELEMENT_DESC shadowVertLayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
    };

   	SHADOWVERT* baseVertexBuffer=new SHADOWVERT[aModel->myVertexData.myNrOfVertexes] ();
	unsigned int* baseIndexBuffer=  new unsigned int [aModel->myVertexIndexData.myNrOfIndexes] ();

	unsigned char* copybaseVertexBuffer=(unsigned char*)baseVertexBuffer;
	unsigned short* copybaseInitBuffer=(unsigned short*)aModel->myVertexIndexData.myIndexData;
	if(aModel->myVertexIndexData.myFormat!=DXGI_FORMAT_R32_UINT )
	{
		for(i=0;i<aModel->myVertexIndexData.myNrOfIndexes;i++)
		{
				baseIndexBuffer[i]=copybaseInitBuffer[i];
		}
	}
	else
	{
		memcpy(baseIndexBuffer,copybaseInitBuffer,4*aModel->myVertexIndexData.myNrOfIndexes);
	}

	for(i=0;i<aModel->myVertexData.myNrOfVertexes;i++)
	{
		memcpy((void*)&baseVertexBuffer[i],(void*)&aModel->myVertexData.myVertexData[i*aModel->myVertexData.myStride],sizeof(SHADOWVERT));
	}


	


	
	

	unsigned int options=0;
//    if( 1==  meshIndexHeader[IBIndex].IndexType )
        options |= D3DX10_MESH_32_BIT;
	D3DX10CreateMesh( Engine::GetInstance()->GetDevice(),
								shadowVertLayout,
                                2,
                                shadowVertLayout[0].SemanticName,
                                aModel->myVertexData.myNrOfVertexes,
								aModel->myVertexIndexData.myNrOfIndexes / 3,
                                options,
                                &tempMesh );

    //set the VB // got to convert for real :/
	tempMesh->SetVertexData( 0, ( void* )(baseVertexBuffer) );
	 //set the IB
    tempMesh->SetIndexData( ( void* ) (baseIndexBuffer ),aModel->myVertexIndexData.myNrOfIndexes);
    //generate adjacency
	HRESULT hr = S_OK;
    hr =tempMesh->GenerateAdjacencyAndPointReps( 0.0001f );
    //generate adjacency indices
    tempMesh->GenerateGSAdjacency();


  
	
	//get the adjacency data out of the mesh

    ID3DX10MeshBuffer* adjacentBuffer = NULL;
    unsigned int* adjacentData = NULL;
    SIZE_T bufferSize = 0;
    hr =tempMesh->GetAdjacencyBuffer(&adjacentBuffer) ;
    hr =adjacentBuffer->Map( ( void** )&adjacentData, &bufferSize);


    ID3DX10MeshBuffer* adjacentPointRepBuffer = NULL;
    unsigned int* adjacentPointRepData = NULL;
    SIZE_T bufferSize2 = 0;
    hr =tempMesh->GetPointRepBuffer(&adjacentPointRepBuffer) ;
    hr =adjacentPointRepBuffer->Map( ( void** )&adjacentPointRepData, &bufferSize2);


	


	
	int numEdges=tempMesh->GetFaceCount()*3;
	int numMaps=0;
	int nextIndex=0;

    EdgeMapping* edgeMapping = new EdgeMapping[numEdges];


	int faceCount=tempMesh->GetFaceCount();
	SHADOWVERT* newVertBuffer=new SHADOWVERT [(faceCount * 3)] ();
	unsigned int* newIndexBuffer= new unsigned int[((faceCount + numEdges * 2)*3)] ();

    ZeroMemory( newVertBuffer,sizeof(SHADOWVERT)*(faceCount * 3) );
    ZeroMemory( newIndexBuffer,4*((faceCount + numEdges * 2)*3));

	int newVert=faceCount * 3;
	int newIndex=(faceCount + numEdges * 2)*3;


     SHADOWVERT* nextOutVertex = (SHADOWVERT*)newVertBuffer;



    // Iterate through the faces.  For each face, output new
    // vertices and face in the new mesh, and write its edges
    // to the mapping table.

    for( int f = 0; f < faceCount; ++f )
    {
        // Copy the vertex data for all 3 vertices
        CopyMemory( nextOutVertex, baseVertexBuffer + baseIndexBuffer[f * 3], sizeof( SHADOWVERT ) );
        CopyMemory( nextOutVertex + 1, baseVertexBuffer + baseIndexBuffer[f * 3 + 1], sizeof( SHADOWVERT ) );
        CopyMemory( nextOutVertex + 2, baseVertexBuffer + baseIndexBuffer[f * 3 + 2], sizeof( SHADOWVERT ) );

        // Write out the face
        newIndexBuffer[nextIndex++] = f * 3;
        newIndexBuffer[nextIndex++] = f * 3 + 1;
        newIndexBuffer[nextIndex++] = f * 3 + 2;

        // Compute the face normal and assign it to
        // the normals of the vertices.
        D3DXVECTOR3 v1, v2;  // v1 and v2 are the edge vectors of the face
        D3DXVECTOR3 vNormal;
        v1 = *( D3DXVECTOR3* )( nextOutVertex + 1 ) - *( D3DXVECTOR3* )nextOutVertex;
        v2 = *( D3DXVECTOR3* )( nextOutVertex + 2 ) - *( D3DXVECTOR3* )( nextOutVertex + 1 );
        D3DXVec3Cross( &vNormal, &v1, &v2 );
        D3DXVec3Normalize( &vNormal, &vNormal );

        nextOutVertex->Normal = -vNormal;
        ( nextOutVertex + 1 )->Normal = -vNormal;
        ( nextOutVertex + 2 )->Normal = -vNormal;

        nextOutVertex += 3;

        // Add the face's edges to the edge mapping table

        // Edge 1
        int nIndex;
        int nVertIndex[3] =
        {
            adjacentPointRepData[baseIndexBuffer[f * 3]],
            adjacentPointRepData[baseIndexBuffer[f * 3 + 1]],
            adjacentPointRepData[baseIndexBuffer[f * 3 + 2]]
        };
        nIndex = FindEdgeInMappingTable( nVertIndex[0], nVertIndex[1], edgeMapping, numEdges );

        // If error, we are not able to proceed, so abort.
        if( -1 == nIndex )
        {
            hr = E_INVALIDARG;
			assert(0);
        }

        if( edgeMapping[nIndex].m_anOldEdge[0] == -1 && edgeMapping[nIndex].m_anOldEdge[1] == -1 )
        {
            // No entry for this edge yet.  Initialize one.
            edgeMapping[nIndex].m_anOldEdge[0] = nVertIndex[0];
            edgeMapping[nIndex].m_anOldEdge[1] = nVertIndex[1];
            edgeMapping[nIndex].m_aanNewEdge[0][0] = f * 3;
            edgeMapping[nIndex].m_aanNewEdge[0][1] = f * 3 + 1;

            ++numMaps;
        }
        else
        {
            // An entry is found for this edge.  Create
            // a quad and output it.
            assert( numMaps > 0 );

            edgeMapping[nIndex].m_aanNewEdge[1][0] = f * 3;      // For clarity
            edgeMapping[nIndex].m_aanNewEdge[1][1] = f * 3 + 1;

            // First triangle
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[0][1];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[0][0];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[1][0];

            // Second triangle
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[1][1];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[1][0];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[0][0];

            // edgeMapping[nIndex] is no longer needed. Copy the last map entry
            // over and decrement the map count.

            edgeMapping[nIndex] = edgeMapping[numMaps - 1];
            FillMemory( &edgeMapping[numMaps - 1], sizeof( edgeMapping[numMaps - 1] ), 0xFF );
            --numMaps;
        }

        // Edge 2
        nIndex = FindEdgeInMappingTable( nVertIndex[1], nVertIndex[2], edgeMapping, numEdges );

        // If error, we are not able to proceed, so abort.
        if( -1 == nIndex )
        {
            hr = E_INVALIDARG;
            assert(0);
        }

        if( edgeMapping[nIndex].m_anOldEdge[0] == -1 && edgeMapping[nIndex].m_anOldEdge[1] == -1 )
        {
            edgeMapping[nIndex].m_anOldEdge[0] = nVertIndex[1];
            edgeMapping[nIndex].m_anOldEdge[1] = nVertIndex[2];
            edgeMapping[nIndex].m_aanNewEdge[0][0] = f * 3 + 1;
            edgeMapping[nIndex].m_aanNewEdge[0][1] = f * 3 + 2;

            ++numMaps;
        }
        else
        {
            // An entry is found for this edge.  Create
            // a quad and output it.
            assert( numMaps > 0 );

            edgeMapping[nIndex].m_aanNewEdge[1][0] = f * 3 + 1;
            edgeMapping[nIndex].m_aanNewEdge[1][1] = f * 3 + 2;

            // First triangle
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[0][1];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[0][0];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[1][0];

            // Second triangle
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[1][1];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[1][0];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[0][0];

            // edgeMapping[nIndex] is no longer needed. Copy the last map entry
            // over and decrement the map count.

            edgeMapping[nIndex] = edgeMapping[numMaps - 1];
            FillMemory( &edgeMapping[numMaps - 1], sizeof( edgeMapping[numMaps - 1] ), 0xFF );
            --numMaps;
        }

        // Edge 3
        nIndex = FindEdgeInMappingTable( nVertIndex[2], nVertIndex[0], edgeMapping, numEdges );

        // If error, we are not able to proceed, so abort.
        if( -1 == nIndex )
        {
			assert(0);
        }

        if( edgeMapping[nIndex].m_anOldEdge[0] == -1 && edgeMapping[nIndex].m_anOldEdge[1] == -1 )
        {
            edgeMapping[nIndex].m_anOldEdge[0] = nVertIndex[2];
            edgeMapping[nIndex].m_anOldEdge[1] = nVertIndex[0];
            edgeMapping[nIndex].m_aanNewEdge[0][0] = f * 3 + 2;
            edgeMapping[nIndex].m_aanNewEdge[0][1] = f * 3;

            ++numMaps;
        }
        else
        {
            // An entry is found for this edge.  Create
            // a quad and output it.
            assert( numMaps > 0 );

            edgeMapping[nIndex].m_aanNewEdge[1][0] = f * 3 + 2;
            edgeMapping[nIndex].m_aanNewEdge[1][1] = f * 3;

            // First triangle
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[0][1];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[0][0];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[1][0];

            // Second triangle
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[1][1];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[1][0];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[0][0];

            // edgeMapping[nIndex] is no longer needed. Copy the last map entry
            // over and decrement the map count.

            edgeMapping[nIndex] = edgeMapping[numMaps - 1];
            FillMemory( &edgeMapping[numMaps - 1], sizeof( edgeMapping[numMaps - 1] ), 0xFF );
            --numMaps;
        }
    }

    // Now the entries in the edge mapping table represent
    // non-shared edges.  What they mean is that the original
    // mesh has openings (holes), so we attempt to patch them.
    // First we need to recreate our mesh with a larger vertex
    // and index buffers so the patching geometry could fit.


	SHADOWVERT* patchVertBuffer=new SHADOWVERT[(( faceCount + numMaps ) *3)] ();
	unsigned int* patchIndexBuffer= new unsigned int[( nextIndex + 3 * numMaps * 7 )] ();

    ZeroMemory( patchVertBuffer,sizeof(SHADOWVERT)*(( faceCount + numMaps ) *3));
    ZeroMemory( patchIndexBuffer, 4 * ( nextIndex + 3 * numMaps * 7 ) );

    CopyMemory( patchVertBuffer, newVertBuffer, sizeof( SHADOWVERT ) * faceCount * 3 );
    CopyMemory( patchIndexBuffer, newIndexBuffer, sizeof( DWORD ) * nextIndex );

	int patchVert=( faceCount + numMaps ) *3;
	int patchIndex=( nextIndex + 3 * numMaps * 7 );

	delete(newIndexBuffer);
	delete(newVertBuffer);
    newVertBuffer = patchVertBuffer;
    newIndexBuffer = patchIndexBuffer;
	patchVertBuffer=NULL;
	patchIndexBuffer=NULL;
	



	int nextVertex = faceCount * 3;

	for( int i = 0; i < numMaps; ++i )
	{
		if( edgeMapping[i].m_anOldEdge[0] != -1 &&
			edgeMapping[i].m_anOldEdge[1] != -1 )
		{
			// If the 2nd new edge indexes is -1,
			// this edge is a non-shared one.
			// We patch the opening by creating new
			// faces.
			if( edgeMapping[i].m_aanNewEdge[1][0] == -1 ||  // must have only one new edge
				edgeMapping[i].m_aanNewEdge[1][1] == -1 )
			{
				// Find another non-shared edge that
				// shares a vertex with the current edge.
				for( int i2 = i + 1; i2 < numMaps; ++i2 )
				{
					if( edgeMapping[i2].m_anOldEdge[0] != -1 &&       // must have a valid old edge
						edgeMapping[i2].m_anOldEdge[1] != -1 &&
						( edgeMapping[i2].m_aanNewEdge[1][0] == -1 || // must have only one new edge
						  edgeMapping[i2].m_aanNewEdge[1][1] == -1 ) )
					{
						int nVertShared = 0;
						if( edgeMapping[i2].m_anOldEdge[0] == edgeMapping[i].m_anOldEdge[1] )
							++nVertShared;
						if( edgeMapping[i2].m_anOldEdge[1] == edgeMapping[i].m_anOldEdge[0] )
							++nVertShared;

						if( 2 == nVertShared )
						{
							// These are the last two edges of this particular
							// opening. Mark this edge as shared so that a degenerate
							// quad can be created for it.

							edgeMapping[i2].m_aanNewEdge[1][0] = edgeMapping[i].m_aanNewEdge[0][0];
							edgeMapping[i2].m_aanNewEdge[1][1] = edgeMapping[i].m_aanNewEdge[0][1];
							break;
						}
						else if( 1 == nVertShared )
						{
							// nBefore and nAfter tell us which edge comes before the other.
							int nBefore, nAfter;
							if( edgeMapping[i2].m_anOldEdge[0] == edgeMapping[i].m_anOldEdge[1] )
							{
								nBefore = i;
								nAfter = i2;
							}
							else
							{
								nBefore = i2;
								nAfter = i;
							}

							// Found such an edge. Now create a face along with two
							// degenerate quads from these two edges.

							newVertBuffer[nextVertex] = newVertBuffer[edgeMapping[nAfter].m_aanNewEdge[0][1]];
							newVertBuffer[nextVertex +
								1] = newVertBuffer[edgeMapping[nBefore].m_aanNewEdge[0][1]];
							newVertBuffer[nextVertex +
								2] = newVertBuffer[edgeMapping[nBefore].m_aanNewEdge[0][0]];
							// Recompute the normal
							D3DXVECTOR3 v1 = newVertBuffer[nextVertex + 1].Position -
								newVertBuffer[nextVertex].Position;
							D3DXVECTOR3 v2 = newVertBuffer[nextVertex + 2].Position -
								newVertBuffer[nextVertex + 1].Position;
							D3DXVec3Normalize( &v1, &v1 );
							D3DXVec3Normalize( &v2, &v2 );
							D3DXVec3Cross( &newVertBuffer[nextVertex].Normal, &v1, &v2 );
							newVertBuffer[nextVertex + 1].Normal = newVertBuffer[nextVertex +
								2].Normal = newVertBuffer[nextVertex].Normal;

							newIndexBuffer[nextIndex] = nextVertex;
							newIndexBuffer[nextIndex + 1] = nextVertex + 1;
							newIndexBuffer[nextIndex + 2] = nextVertex + 2;

							// 1st quad

							newIndexBuffer[nextIndex + 3] = edgeMapping[nBefore].m_aanNewEdge[0][1];
							newIndexBuffer[nextIndex + 4] = edgeMapping[nBefore].m_aanNewEdge[0][0];
							newIndexBuffer[nextIndex + 5] = nextVertex + 1;

							newIndexBuffer[nextIndex + 6] = nextVertex + 2;
							newIndexBuffer[nextIndex + 7] = nextVertex + 1;
							newIndexBuffer[nextIndex + 8] = edgeMapping[nBefore].m_aanNewEdge[0][0];

							// 2nd quad

							newIndexBuffer[nextIndex + 9] = edgeMapping[nAfter].m_aanNewEdge[0][1];
							newIndexBuffer[nextIndex + 10] = edgeMapping[nAfter].m_aanNewEdge[0][0];
							newIndexBuffer[nextIndex + 11] = nextVertex;

							newIndexBuffer[nextIndex + 12] = nextVertex + 1;
							newIndexBuffer[nextIndex + 13] = nextVertex;
							newIndexBuffer[nextIndex + 14] = edgeMapping[nAfter].m_aanNewEdge[0][0];

							// Modify mapping entry i2 to reflect the third edge
							// of the newly added face.

							if( edgeMapping[i2].m_anOldEdge[0] == edgeMapping[i].m_anOldEdge[1] )
							{
								edgeMapping[i2].m_anOldEdge[0] = edgeMapping[i].m_anOldEdge[0];
							}
							else
							{
								edgeMapping[i2].m_anOldEdge[1] = edgeMapping[i].m_anOldEdge[1];
							}
							edgeMapping[i2].m_aanNewEdge[0][0] = nextVertex + 2;
							edgeMapping[i2].m_aanNewEdge[0][1] = nextVertex;

							// Update next vertex/index positions

							nextVertex += 3;
							nextIndex += 15;

							break;
						}
					}
				}
			}
			else
			{
				// This is a shared edge.  Create the degenerate quad.

				// First triangle
				newIndexBuffer[nextIndex++] = edgeMapping[i].m_aanNewEdge[0][1];
				newIndexBuffer[nextIndex++] = edgeMapping[i].m_aanNewEdge[0][0];
				newIndexBuffer[nextIndex++] = edgeMapping[i].m_aanNewEdge[1][0];

				// Second triangle
				newIndexBuffer[nextIndex++] = edgeMapping[i].m_aanNewEdge[1][1];
				newIndexBuffer[nextIndex++] = edgeMapping[i].m_aanNewEdge[1][0];
				newIndexBuffer[nextIndex++] = edgeMapping[i].m_aanNewEdge[0][0];
			}
		}
	}



    // At this time, the output mesh may have an index buffer
    // bigger than what is actually needed, so we create yet
    // another mesh with the exact IB size that we need and
    // output it.  This mesh also uses 16-bit index if
    // 32-bit is not necessary.


	int numVertices=(( faceCount + numMaps ) *3);
	int numIndices=( nextIndex );
	SHADOWVERT* finalVertBuffer=new SHADOWVERT[(( faceCount + numMaps ) *3)] ();
	unsigned int* finalIndexBuffer= new unsigned int[( nextIndex)] ();

    ZeroMemory( finalVertBuffer,sizeof(SHADOWVERT)*(( faceCount + numMaps ) *3));
    ZeroMemory( finalIndexBuffer, 4 * ( nextIndex) );

    CopyMemory( finalVertBuffer, newVertBuffer, sizeof( SHADOWVERT ) * (( faceCount + numMaps ) *3));
    CopyMemory( finalIndexBuffer, newIndexBuffer, sizeof( DWORD ) * nextIndex );

      
	int finalVert=( faceCount + numMaps ) *3;
	int finalIndex=nextIndex;
    //release the ID3DX10Mesh
    adjacentPointRepBuffer->Unmap();
    adjacentPointRepBuffer->Release();

    tempMesh->Release();
	delete(edgeMapping);
	
	delete(newIndexBuffer);
	delete(newVertBuffer);
	delete(patchIndexBuffer);
	delete(patchVertBuffer);







	aModel->myShadowModel= new Model();
	aModel->myShadowModel->myCastsShadowFlag=aModel->myCastsShadowFlag;
	aModel->myShadowModel->myIsNULLObject=aModel->myIsNULLObject;
	aModel->myShadowModel->myFileName=aModel->myFileName;

	
	aModel->myShadowModel->myOrientation=aModel->myOrientation;

	aModel->myShadowModel->myVertexIndexData.myNrOfIndexes=numIndices;
	aModel->myShadowModel->myVertexIndexData.myFormat=DXGI_FORMAT_R32_UINT;
	aModel->myShadowModel->myVertexIndexData.mySize=aModel->myShadowModel->myVertexIndexData.myNrOfIndexes*sizeof(unsigned int);
	
	aModel->myShadowModel->myVertexIndexData.myIndexData= new  char [aModel->myShadowModel->myVertexIndexData.mySize] ();
	memcpy(aModel->myShadowModel->myVertexIndexData.myIndexData,finalIndexBuffer,aModel->myShadowModel->myVertexIndexData.mySize);

	aModel->myShadowModel->myVertexData.myNrOfVertexes=numVertices;
	aModel->myShadowModel->myVertexData.mySize=aModel->myShadowModel->myVertexData.myNrOfVertexes*sizeof(SHADOWVERT);
	aModel->myShadowModel->myVertexData.myStride=sizeof(SHADOWVERT);
	aModel->myShadowModel->myVertexData.myType=VertexTypePosNorm;
	aModel->myShadowModel->myVertexData.myVertexData = new  char [aModel->myShadowModel->myVertexData.mySize] ();
	memcpy(aModel->myShadowModel->myVertexData.myVertexData,finalVertBuffer,aModel->myShadowModel->myVertexData.mySize);

	aModel->myShadowModel->myVertexFormat.Add(shadowVertLayout[0]);
	aModel->myShadowModel->myVertexFormat.Add(shadowVertLayout[1]);

	aModel->myShadowModel->myVertexType=VertexTypePosNorm;


	Surface surf;
	surf.SetDiffuseTexture(aModel->mySurfaces[0].GetDiffuseTexture()->GetFileName());

	surf.SetPrimologyType(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	surf.SetVertexStart(0);
	surf.SetIndexStart(0);
	surf.SetIndexCount(numIndices);
	surf.SetAdjacentPrimologyType(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ);
	aModel->myShadowModel->mySurfaces.Add(surf);
	
	SelectEffect(aModel->myShadowModel);

	delete(finalIndexBuffer);
	delete(finalVertBuffer);

	return(true);

}
