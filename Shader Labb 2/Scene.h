#ifndef _SCENE_H_
#define _SCENE_H_

#include "Camera.h"
#include "CU_GrowingArray.h"
#include "Light.h"
#include "Instance.h"
#include "OctTree.h"
#include <vector>
#include "CPUParticleEmittorInstance.h"
#include "Streak.h"

class RenderProcessTarget;
class Renderer;

class Scene
{
	friend class Renderer;
public:
	Scene(void);
	~Scene(void);

	void AddInstance(Instance* aInstnace);
	void RemoveInstance(Instance* anInstance);
	void AddLight(Light* aLight);
	void RemoveLight(Light* aLight);
	void AddEmittor( CPUParticleEmittorInstance* myParticleEmittor );
	void AddStreak(Streak& aStreak);

	void SetCamera(Camera& aCamera);

	Camera& GetCamera();

	void Update(const float aDelta);
	void Render();
	void Render( const EffectTechniques::TechniqueType aTechnique );
	void RenderToTarget( RenderProcessTarget& aRenderTarget );
	void RenderToCube( const CommonUtilities::StaticArray< Matrix44f, 6 >& someToView );
	
private:
	void GetObjectsWithinLightRadius( std::vector<Instance*>& someObjectWithinRadius, Light& aLight );
	void ClearLightsFromInstances();
	
	void RenderInstances();
	void RenderInstances( const EffectTechniques::TechniqueType aTechnique );
	void RenderInstancesFromLight();
	void RenderStreaks();
	void RenderEmittorInstances();

	void UpdateObjects(const float aDelta);
	void UpdateLights();
	void UpdateEmittors(const float aDelta);
	void UpdateStreaks( const float aDelta );
	void RenderInstancesToTarget( RenderProcessTarget& aRenderTarget );

	
	Camera myCamera;

	CommonUtilities::GrowingArray<Instance*> myAllInstances;
	//CommonUtilities::GrowingArray<Instance*> myVisibleInstances;
	CommonUtilities::GrowingArray<Light*> myLights; 
	CommonUtilities::GrowingArray<Streak*> myStreaks;
	//CU::GrowingArray<CPUParticleEmittorInstance*, int> myEmittors;

	//OctTree myOctTree;

};

#endif