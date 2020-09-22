#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "Model.h"
#include "Instance.h"
#include "Camera.h"
#include "SystemTimer.h"
#include "Effect.h"
#include "Input.h"
#include "Light.h"
#include "StaticArray.h"
#include "EffectInput.h"
#include "CPUParticleEmittorInstance.h"
#include "Streak.h"

class Engine;

class Application
{
public:
	Application(Engine& aGFXEngine);
	~Application();

	bool Init();

	bool Destroy();
	bool Update();

	void HandleInput( float delta );

	void Render();

	void LogicUpdate();

	static const int myNumberOfAmbientProbes = 5;
	static Vector3f ourAmbientProbePositions[5];
private:
	void UpdateFPS(const float aDeltaTime);
	std::string ConvertToString( const int aType );
	void RenderReflectionCubeMap(int anAmbientProbeIndex);
	void InitModels();
	void InitAmbientLabb();
	void AddWall(Vector3f aPosition, Matrix33f aRotation = Matrix33f());
	void AddLight();
	void RemoveLight();
	void AddBallInstance(Vector3f aPosition);
	void RemoveBallInstance();

	Model* myTinyModel;
	Streak myStreak;

	CommonUtilities::GrowingArray<Instance*> myInstances;
	CommonUtilities::GrowingArray<Light*> myPointLights;
	CommonUtilities::GrowingArray<Light*> mySpotLights;
	CPUParticleEmittorInstance* myParticleEmittor;
	CPUParticleEmittorData myParticleEmittorData;

	Effect myNormalEffect;

	Camera myCamera;
	SystemTimer mySystemTimer;
	float myTime;
	float myKeyPressCooldownTimer;
	int myCurrentAmbientProbeIndex;

	Library2D::Input* myInputHandler;
	//TI::TimeManager myTimeManager;
	Engine& myGfxEngine;

	Model* myGroundModels[myNumberOfAmbientProbes];
	Model* myBallModels[myNumberOfAmbientProbes];
	Model* myConeModel;
	Model* myLevelModel;

	Light* myBlueLight;
	Light* myRedLight;

	int myHasRenderedCube;
	Vector3f myBallPosition;
};

#endif