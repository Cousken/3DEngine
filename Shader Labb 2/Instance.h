#ifndef _INSTANCE_H_
#define _INSTANCE_H_

#include "Model.h"
#include <d3d10.h>
#include <d3dx10.h>
#include "Vector.h"
#include "Matrix.h"
#include "Camera.h"
#include "Effect.h"
#include "StaticArray.h"
#include "TransformationNodeInstance.h"
#include "EffectTechnique.h"
#include "BoneInstanceHierarchy.h"
#include "BoneInstance.h"

class Light;
class RenderProcessTarget;

class Instance
{
public:
	Instance(Model& aModel);
	~Instance(void);

	bool Init();
	void Render(Camera& aCamera, EffectTechniques::TechniqueType aTechinqueType);
	void RenderUsingEffect(Camera& aCamera, EffectTechniques::TechniqueType aTechinqueType, Effect *anEffect);
	void RenderFromLight(EffectTechniques::TechniqueType aTechinqueType);
	//void RenderToTarget( Camera& myCamera, EffectTechniques::TechniqueType aTechnique, RenderProcessTarget& aRenderTarget ) const;
	void RenderModel( Model* aModel, const Matrix44f aParentSpace, EffectTechniques::TechniqueType aTechniqueType, TransformationNodeInstance& aHierarchy, CommonUtilities::StaticArray< Matrix44f, 255 >& aBoneMatrixArray );
	void RenderModelUsingEffect( Model* aModel, const Matrix44f aParentSpace, EffectTechniques::TechniqueType aTechniqueType, TransformationNodeInstance& aHierarchy, Effect *anEffect);
	
	void RenderToTarget( Camera& myCamera, EffectTechniques::TechniqueType aTechnique, RenderProcessTarget& aRenderTarget );
	void RenderModelToTarget( Model* aModel, const Matrix44f aParentSpace, EffectTechniques::TechniqueType aTechniqueType, TransformationNodeInstance& aHierarchy, CommonUtilities::StaticArray< Matrix44f, 255 >& someBoneMatixes, RenderProcessTarget& aRenderTarget );

	void Update(float aElapsedTime);
	void TransformFrame(int aBoneId, const Matrix44f& aParentWorld);

	void PerformRotation(Matrix33f& aOrientation);
	void PerformTransformation(Matrix44f& aOrientation);

	void GetPosition(Vector3f& aPosition);
	Matrix44f& GetOrientation();
	Model& GetModel();

	void SetPosition(Vector3f& aPosition);
	void SetOrientation(Matrix44f& aOrientation);

	void TryAddLight( float aLightsStrength, Light* aLight );
	void ClearLights();

private:
	struct LightWithStrength
	{
		LightWithStrength(Light* aLight, const float aStrength)
		{
			myLight = aLight;
			myStrength = aStrength;
		};
		
		Light* myLight;
		float myStrength;
	};
	
	void BuildHierachy(TransformationNodeInstance& aHierarchy, Model* aModel);

	void TryAddSpotLight( Light* aLight, float aLightsStrength );
	void TryAddPointLight( Light* aLight, float aLightsStrength );
	void TryAddDirectionalLight( Light* aLight, float aLightsStrength );

	void UpdateLights();
	void UpdateDirectionalLights();
	void UpdatePointLights();
	void UpdateSpotLights();
	
	Model& myModel;
	std::string myEffectFile;
	TransformationNodeInstance myHierarchy;
	BoneInstanceHierarchy* myAnimation;
	CommonUtilities::GrowingArray<BoneInstance*> myBoneInstances;

	Matrix44f myOrientation;
	Vector3f myPosition;

	CommonUtilities::StaticArray<LightWithStrength*, 4> myDirectionalLights;
	CommonUtilities::StaticArray<LightWithStrength*, 4> myPointLights;
	CommonUtilities::StaticArray<LightWithStrength*, 4> mySpotLights;

	
};

#endif