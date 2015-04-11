#include "Scene.h"
#include "Engine.h"
#include "StaticArray.h"

Scene::Scene(void)
{
	myVisibleInstances.Init(10, 10);
	myAllInstances.Init(10,10);
	myLights.Init(10,10);
	myStreaks.Init(10, 10);
//	myEmittors.Init(10, 10);

	myOctTree.BuildTree(Vector3f(0,0,0), 5000.0f, 8, 1.0f);
}

Scene::~Scene(void)
{
}

void Scene::AddInstance( Instance* aInstnace )
{
	myAllInstances.Add(aInstnace);
	myOctTree.InsertObject(myOctTree.GetRoot(), new TreeDweller(aInstnace) );
}

void Scene::SetCamera( Camera& aCamera )
{
	myCamera = aCamera;
}

void Scene::Render()
{
	RenderInstances();
}

void Scene::Render( const EffectTechniques::TechniqueType aTechnique )
{
	Engine::GetInstance()->GetEffectInput().UpdateCamera(myCamera);

	RenderInstances( aTechnique );
}

void Scene::RenderToTarget( RenderProcessTarget& aRenderTarget )
{
	Engine::GetInstance()->GetEffectInput().UpdateCamera(myCamera);

	RenderInstancesToTarget( aRenderTarget );
}

void Scene::RenderInstancesToTarget( RenderProcessTarget& aRenderTarget )
{
	const int nbrOfInstances = myVisibleInstances.Count();
	for (int instanceRendered = 0; instanceRendered < nbrOfInstances; instanceRendered++)
	{
		myVisibleInstances[instanceRendered]->RenderToTarget(myCamera, EffectTechniques::AMBIENT, aRenderTarget);
	}
}

void Scene::RenderToCube( const CommonUtilities::StaticArray< Matrix44f, 6 >& someToView )
{
	Engine::GetInstance()->GetEffectInput().SetMatrixArray( "CubeView", &someToView[0], 6 );
	
	RenderInstances( EffectTechniques::CUBE_MAP );
}

void Scene::RenderInstances()
{
	const int nbrOfInstances = myVisibleInstances.Count();

	for (int instanceRendered = 0; instanceRendered < nbrOfInstances; instanceRendered++)
	{
		myVisibleInstances[instanceRendered]->Render(myCamera, EffectTechniques::NORMAL);
	}
}

void Scene::RenderInstances( const EffectTechniques::TechniqueType aTechnique )
{
	const int nbrOfInstances = myVisibleInstances.Count();

	for (int instanceRendered = 0; instanceRendered < nbrOfInstances; instanceRendered++)
	{
		myVisibleInstances[instanceRendered]->Render(myCamera, aTechnique );
	}
}

void Scene::AddLight( Light* aLight )
{
	myLights.Add(aLight);
}


void Scene::AddEmittor( CPUParticleEmittorInstance* myParticleEmittor )
{
//	myEmittors.Add(myParticleEmittor);
}

Camera& Scene::GetCamera()
{
	return myCamera;
}

void Scene::Update(const float aDelta)
{
	UpdateObjects(aDelta);
/*	UpdateStreaks(aDelta);*/
	//UpdateEmittors(aDelta);
	//UpdateLights();
}

void Scene::GetObjectsWithinLightRadius( std::vector<Instance*>& someObjectWithinRadius, Light& aLight )
{
	if (aLight.GetType() != Light::DIRECTIONAL_LIGHT_TYPE)
	{
		const int nbrOfInstances = myVisibleInstances.Count();
		const float lightRadius = aLight.GetMaxDistance();
		for (int instanceChecked = 0; instanceChecked < nbrOfInstances; instanceChecked++)
		{
			Vector3f instancePosition(0,0,0);
			myVisibleInstances[instanceChecked]->GetPosition(instancePosition);
			const float distnace = (aLight.GetPosition() - instancePosition).Length();
			const float sumOfRadiuses = lightRadius + myVisibleInstances[instanceChecked]->GetModel().GetRadius();
			if ( distnace <= sumOfRadiuses )
			{
				someObjectWithinRadius.push_back(myVisibleInstances[instanceChecked]);
			}
		}
	}
}

void Scene::ClearLightsFromInstances()
{
	const int nbrOfInstances = myAllInstances.Count();
	for (int instanceCleared = 0; instanceCleared < nbrOfInstances; instanceCleared++)
	{
		myAllInstances[instanceCleared]->ClearLights();
	}
}

void Scene::UpdateLights()
{
	ClearLightsFromInstances();
	const int nbrOfLight = myLights.Count();
	for (int lightUpdated = 0; lightUpdated < nbrOfLight; lightUpdated++)
	{
		std::vector<Instance*> objectWithinRadius;
		this->GetObjectsWithinLightRadius(objectWithinRadius, (*myLights[lightUpdated]));
		const int nbrOfInstances = objectWithinRadius.size();
		for (int objectUpdated = 0; objectUpdated < nbrOfInstances; objectUpdated++)
		{
			Vector3f objectPosition(0,0,0);
			objectWithinRadius[objectUpdated]->GetPosition(objectPosition);
			const float distanceBetween = (myLights[lightUpdated]->GetPosition() - objectPosition).Length();
			const float lightStrength = myLights[lightUpdated]->GetMaxDistance() / distanceBetween;
			objectWithinRadius[objectUpdated]->TryAddLight(lightStrength, myLights[lightUpdated]);
		}
	}
}

void Scene::UpdateEmittors(const float aDelta)
{
// 	const int nbrOfEmittors = myEmittors.Count();
// 	for (int emittorUpdated = 0; emittorUpdated < nbrOfEmittors; emittorUpdated++)
// 	{
// 		myEmittors[emittorUpdated]->Update(aDelta);
// 	}
}

void Scene::RenderEmittorInstances()
{
// 	const int nbrOfEmittors = myEmittors.Count();
// 	for (int emittorRenderd = 0; emittorRenderd < nbrOfEmittors; emittorRenderd++)
// 	{
// 		myEmittors[emittorRenderd]->Render();
// 	}
}

void Scene::UpdateObjects( const float aDelta )
{
	const int totalNbrInstances = myAllInstances.Count();
	for (int instanceUpdated = 0; instanceUpdated < totalNbrInstances; instanceUpdated++)
	{
		myAllInstances[instanceUpdated]->Update(aDelta);
	}	
	
	myVisibleInstances.RemoveAll();
	myOctTree.Update(myCamera.GetFrustrum(), myVisibleInstances);
}

void Scene::AddStreak( Streak& aStreak )
{
	myStreaks.Add( &aStreak );
}

void Scene::RenderStreaks()
{
	const int nbrOfStreaks = myStreaks.Count();

	for (int streakRendered = 0; streakRendered < nbrOfStreaks; streakRendered++)
	{
		myStreaks[streakRendered]->Render();
	}
}

void Scene::UpdateStreaks( const float aDelta )
{
	const int nbrStreaks = myStreaks.Count();
	for (int streakUpdated = 0; streakUpdated < nbrStreaks; streakUpdated++)
	{
		myStreaks[streakUpdated]->Update(aDelta);
	}	
}




