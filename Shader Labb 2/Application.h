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
private:
	void UpdateFPS(const float aDeltaTime);
	std::string ConvertToString( const int aType );
	void RenderReflectionCubeMap();
	void InitModels();
	void InitCubeRoomMap();
	void InitPlaneWithBallsMap();
	void InitThreeRooms();

	void InitRoomOne( const float aDistance );

	Model* myTinyModel;
	Streak myStreak;

	CommonUtilities::StaticArray<Instance*, 1001> myInstances;
	CommonUtilities::StaticArray<Light*, 500> myLights;
	CPUParticleEmittorInstance* myParticleEmittor;
	CPUParticleEmittorData myParticleEmittorData;

	Effect myNormalEffect;

	Camera myCamera;
	SystemTimer mySystemTimer;
	float myTime;
	float myKeyPressCooldownTimer;

	Library2D::Input* myInputHandler;
	//TI::TimeManager myTimeManager;
	Engine& myGfxEngine;

	Model* myGroundModel;
	Model* myBallModel;
	Model* myLevelModel;

	Light* myBlueLight;
	Light* myRedLight;

	int myHasRenderedCube;
	Vector3f myBallPosition;
};

#endif