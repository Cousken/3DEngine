#include "Application.h"
#include "Engine.h"
#include "Renderer.h"
#include "StaticArray.h"
#include "ColladaMeshFactory.h"
#include <math.h>
#include <sstream>

#define CAMERA_SPEED 50.0f
#define PI 3.14159265f

const float LocKeyPressCooldown = 0.2f;
int locNumberOfBounces = 0;
int locBounceIndex = 0;
int locProbeIndex = 0;
Vector3f Application::ourAmbientProbePositions[5];
Vector3f locMovement = Vector3f(0.0f, 0.0f, 0.0f);
float locAccelerator = 0.0f;

bool Application::Init()
{
	myCurrentAmbientProbeIndex = -1;

	myPointLights.Init(500, 1);
	mySpotLights.Init(500, 1);
	myInstances.Init(500, 1);

	myInputHandler = new Library2D::Input(GetModuleHandle(NULL), Engine::GetInstance()->GetWindowHandler());

	InitModels();
	InitAmbientLabb();

	Engine::GetInstance()->GetRenderer().mySphereMesh = new Instance(*myBallModels[0]);
	Engine::GetInstance()->GetRenderer().mySphereMesh->Init();
	
	Engine::GetInstance()->GetScene().SetCamera(myCamera);	

	return true;
}

bool Application::Destroy()
{
	return true;
}

bool Application::Update()
{
	mySystemTimer.Update();
	float delta = mySystemTimer.GetDeltaTime();	
	if(delta > 0.05f)
	{
		delta = 0.05f;
	}
	myKeyPressCooldownTimer += delta;

	HandleInput(delta);

	myTime += delta;
	Engine::GetInstance()->GetEffectInput().SetScalar("time", myTime);
	Engine::GetInstance()->GetEffectInput().SetScalar("DeltaTime", delta);
	Engine::GetInstance()->GetScene().Update(delta);

	UpdateFPS( delta );

	//if ( myBlueLight != NULL)
	//{
	//	myBlueLight->Rotate( Matrix33f::CreateRotateAroundY(delta*2) );
	//}
	//if ( myRedLight != NULL)
	//{
	//	myRedLight->Rotate( Matrix33f::CreateRotateAroundY( -delta) );
	//}
	if(myGfxEngine.GetRenderer().myTogglePointOrSpotLight == true)
	{
		locAccelerator += delta;
		if(locAccelerator > 6.28f)
		{
			locAccelerator = -6.28f;
		}
		locMovement.y = sinf(locAccelerator) * delta * 3.14f;

		for(int index = 0; index < myInstances.Count(); index++)
		{
			if(&myInstances[index]->myModel == myBallModels[0])
			{
				Vector3f newPosition = myInstances[index]->GetPosition2() + locMovement;
				myInstances[index]->SetPosition(newPosition);
			}
		}
	}

	myGfxEngine.SwitchBuffers();
	return(true);
}

Application::Application( Engine& aGFXEngine )
: myGfxEngine(aGFXEngine)
{
	myCamera = Camera();
	myTime = 0;
	myLevelModel = NULL;
	myBlueLight = NULL;
	myRedLight = NULL;
	myConeModel = NULL;

	myHasRenderedCube = 0;

	myKeyPressCooldownTimer = 0.0f;
}

Application::~Application()
{

}

void Application::Render()
{
	Engine::GetInstance()->GetScene().SetCamera(myCamera);

	if ( myHasRenderedCube < 1 )
	{
		Engine::GetInstance()->GetRenderer().ResetStuff();

		const float wallSize = 33.3333f;
		const float halfWallSize = wallSize / 2.0f;

		const int numberOfBounces = locNumberOfBounces;
		const int numberOfAmbientProbes = 5;

		ourAmbientProbePositions[0] = Vector3f(halfWallSize, wallSize, halfWallSize);	//room1
		ourAmbientProbePositions[1] = Vector3f(0.0f, wallSize, wallSize * 2.5f);//corridoor 1
		ourAmbientProbePositions[2] = Vector3f(halfWallSize, wallSize, wallSize * 4.5f);//room 2
		ourAmbientProbePositions[3] = Vector3f(0.0f, wallSize, wallSize * 6.5f);//corridoor 2
		ourAmbientProbePositions[4] = Vector3f(halfWallSize, wallSize, wallSize * 8.5f);//room 3

		for(int bounceIndex = 0; bounceIndex < numberOfBounces; bounceIndex++)
		{
			for(int probeIndex = 0; probeIndex < numberOfAmbientProbes; probeIndex++)
			{
				Engine::GetInstance()->GetRenderer().RenderToCube( ourAmbientProbePositions[probeIndex], probeIndex );
			}
		}
		myHasRenderedCube++;
	}
	else
	{
		Engine::GetInstance()->GetRenderer().Render();
	}
}

void Application::HandleInput( float delta )
{
	myInputHandler->Update();

	float speedBoost = 1.0f;
	if(myInputHandler->IsKeyDown(Keys::LShift) == true)
	{
		speedBoost = 5.0f;
	}
	Vector3f cameraMovement = Vector3f(0,0,0);
	if (myInputHandler->IsKeyDown(Keys::W) == true)
	{
		cameraMovement.z += CAMERA_SPEED * delta * speedBoost;
	}
	if (myInputHandler->IsKeyDown(Keys::S) == true)
	{
		cameraMovement.z -= CAMERA_SPEED  * delta * speedBoost;
	}
	if (myInputHandler->IsKeyDown(Keys::A) == true)
	{
		cameraMovement.x -= CAMERA_SPEED  * delta * speedBoost;
	}
	if (myInputHandler->IsKeyDown(Keys::D) == true)
	{
		cameraMovement.x += CAMERA_SPEED * delta * speedBoost;
	}
	if (myInputHandler->IsKeyDown(Keys::Q) == true)
	{
		myCamera.Rotate(Matrix33f::CreateRotateAroundZ(-1.0f * delta));
	}
	if (myInputHandler->IsKeyDown(Keys::E) == true)
	{
		myCamera.Rotate(Matrix33f::CreateRotateAroundZ(1.0f * delta));
	}
	cameraMovement = cameraMovement * myCamera.GetOrientation().Get33();
	myCamera.Translate(cameraMovement);

	Matrix33f cameraRotation = Matrix33f::Identity();
	if (myInputHandler->IsKeyDown(Keys::Up) == true)
	{
		cameraRotation *= Matrix33f::CreateRotateAroundX(CAMERA_SPEED * 0.1f * delta);
	}
	if (myInputHandler->IsKeyDown(Keys::Down) == true)
	{
		cameraRotation *= Matrix33f::CreateRotateAroundX(-(CAMERA_SPEED * 0.1f * delta));
	}
	if (myInputHandler->IsKeyDown(Keys::Left) == true)
	{
		cameraRotation *= Matrix33f::CreateRotateAroundY(CAMERA_SPEED * 0.1f * delta);
	}
	if (myInputHandler->IsKeyDown(Keys::Right) == true)
	{
		cameraRotation *= Matrix33f::CreateRotateAroundY(-(CAMERA_SPEED * 0.1f * delta));
	}	

	if (myInputHandler->IsKeyDown(Keys::F1) == true)
	{
		if(myKeyPressCooldownTimer > LocKeyPressCooldown)
		{
			if(locProbeIndex == 0 && locBounceIndex == 0)
			{
				myKeyPressCooldownTimer = 0.0f;
				//myGfxEngine.GetRenderer().myToggleAdvancedCulling = false;
				myHasRenderedCube = 0;
				locNumberOfBounces++;
				if(locNumberOfBounces > 20)
				{
					locNumberOfBounces = 20;
				}
			}
		}
	}
	if (myInputHandler->IsKeyDown(Keys::F2) == true)
	{
		if(myKeyPressCooldownTimer > LocKeyPressCooldown)
		{
			if(locProbeIndex == 0 && locBounceIndex == 0)
			{
				myKeyPressCooldownTimer = 0.0f;
				//myGfxEngine.GetRenderer().myToggleAdvancedCulling = true;
				myHasRenderedCube = 0;
				locNumberOfBounces--;
				if(locNumberOfBounces < 0)
				{
					locNumberOfBounces = 0;
				}
			}
		}
	}
	if (myInputHandler->IsKeyDown(Keys::F3) == true)
	{
		if(myKeyPressCooldownTimer > LocKeyPressCooldown)
		{
			myKeyPressCooldownTimer = 0.0f;
			myGfxEngine.GetRenderer().myTogglePointOrSpotLight = !myGfxEngine.GetRenderer().myTogglePointOrSpotLight;
		}
	}
	if (myInputHandler->IsKeyDown(Keys::F5) == true)
	{
		if(myKeyPressCooldownTimer > LocKeyPressCooldown)
		{
			myKeyPressCooldownTimer = 0.0f;
			//myGfxEngine.GetRenderer().WriteAllToDisk();
		}
	}

	if(myInputHandler->IsKeyDown(Keys::Escape) == true)
	{
		exit(0);
	}

	myCamera.Rotate(cameraRotation);
}

void Application::UpdateFPS(const float aDeltaTime)
{
	static float timer = 0;
	static int frameCounter = 1.0f / aDeltaTime;
	timer += aDeltaTime;

	if ( timer >= 1.0f)
	{
		timer = 0.0f;
		frameCounter = 1.0f / aDeltaTime;
	}

	//std::string stringToDisplay = "Cam X: " + ConvertToString(myCamera.GetPositionNotRetarded().myX) + " Cam Y: " + ConvertToString(myCamera.GetPositionNotRetarded().myY) + " Cam Z: " + ConvertToString(myCamera.GetPositionNotRetarded().myZ);
	//std::string stringToDisplay = "Current Light Bounces (LBs): " + ConvertToString(locNumberOfBounces) + " || F1: LBs + 1 || F2: LBs - 1 || F3: Toggle dir light";
	std::string stringToDisplay =  "Exponential Shadow Maps || F3: Toggle Sphere Movement || Current Light Bounces: " + ConvertToString(locNumberOfBounces) + " || LShift: Speed Boost || FPS: " + ConvertToString(frameCounter);
	Engine::GetInstance()->SetTitle( stringToDisplay );
}

std::string Application::ConvertToString( const int aType )
{
	std::stringstream converter;
	converter << aType;
	return converter.str();
}

void Application::RenderReflectionCubeMap(int anAmbientProbeIndex)
{
	Engine::GetInstance()->GetRenderer().RenderToCube( myBallPosition, anAmbientProbeIndex );
}

void Application::InitModels()
{
	ColladaMeshFactory factory;

	for(int index = 0; index < 1; index++)
	{
		myGroundModels[index] = factory.CreateModel("Plane_Checker.dae");
		bool success = myGroundModels[index]->InitBuffers();
		assert( (success == true) && "Could not initiate the model!");
	}

	for(int index = 0; index < 1; index++)
	{
		myBallModels[index] = factory.CreateModel("Sphere.dae");
		bool success = myGroundModels[index]->InitBuffers();
		assert( (success == true) && "Could not initiate the model!");
	}
}

void Application::InitAmbientLabb()
{
	const float distance = 33.34f;

	myCamera.SetPosition(Vector3f(distance * 0.5f, distance, -50.0f));

	//Room 1 
	myCurrentAmbientProbeIndex = 0;
	AddWall(Vector3f(0.0f, 0.0f, 0.0f));
	AddWall(Vector3f(distance, 0.0f, 0.0f));
	AddWall(Vector3f(0.0f, 0.0f, distance));
	AddWall(Vector3f(distance, 0.0f, distance));

	AddWall(Vector3f(0.0f, distance * 2.0f, 0.0f), Matrix33f::CreateRotateAroundZ(3.14f));
	AddWall(Vector3f(distance, distance * 2.0f, 0.0f), Matrix33f::CreateRotateAroundZ(3.14f));
	AddWall(Vector3f(0.0f, distance * 2.0f, distance), Matrix33f::CreateRotateAroundZ(3.14f));
	AddWall(Vector3f(distance, distance * 2.0f, distance), Matrix33f::CreateRotateAroundZ(3.14f));

	AddWall(Vector3f(-distance * 0.5f, distance * 0.5f, 0.0f), Matrix33f::CreateRotateAroundZ(1.57f));
	AddWall(Vector3f(-distance * 0.5f, distance * 0.5f, distance), Matrix33f::CreateRotateAroundZ(1.57f));
	AddWall(Vector3f(distance * 1.5f, distance * 0.5f, 0.0f), Matrix33f::CreateRotateAroundZ(-1.57f));
	AddWall(Vector3f(distance * 1.5f, distance * 0.5f, distance), Matrix33f::CreateRotateAroundZ(-1.57f));
	AddWall(Vector3f(distance, distance * 0.5f, distance * 1.5f), Matrix33f::CreateRotateAroundX(1.57f));
	AddWall(Vector3f(0.0f, distance * 0.5f, -distance * 0.5f), Matrix33f::CreateRotateAroundX(-1.57f));
	AddWall(Vector3f(distance, distance * 0.5f, -distance * 0.5f), Matrix33f::CreateRotateAroundX(-1.57f));

	AddWall(Vector3f(-distance * 0.5f, distance * 1.5f, 0.0f), Matrix33f::CreateRotateAroundZ(1.57f));
	AddWall(Vector3f(-distance * 0.5f, distance * 1.5f, distance), Matrix33f::CreateRotateAroundZ(1.57f));
	AddWall(Vector3f(distance * 1.5f, distance * 1.5f, 0.0f), Matrix33f::CreateRotateAroundZ(-1.57f));
	AddWall(Vector3f(distance * 1.5f, distance * 1.5f, distance), Matrix33f::CreateRotateAroundZ(-1.57f));
	AddWall(Vector3f(distance, distance * 1.5f, distance * 1.5f), Matrix33f::CreateRotateAroundX(1.57f));
	AddWall(Vector3f(0.0f, distance * 1.5f, -distance * 0.5f), Matrix33f::CreateRotateAroundX(-1.57f));
	AddWall(Vector3f(distance, distance * 1.5f, -distance * 0.5f), Matrix33f::CreateRotateAroundX(-1.57f));

	const float halfWallSize = distance / 2.0f;
	AddBallInstance(Vector3f(distance, distance, halfWallSize - 3.0f));
	AddBallInstance(Vector3f(distance, distance + 1.5f, halfWallSize));
	AddBallInstance(Vector3f(distance, distance - 1.5f, halfWallSize));
	AddBallInstance(Vector3f(distance, distance, halfWallSize + 3.0f));


	//Corridoor 1
	myCurrentAmbientProbeIndex = 1;
	AddWall(Vector3f(0.0f, 0.0f, distance * 2.0f));
	AddWall(Vector3f(0.0f, 0.0f, distance * 3.0f));

	AddWall(Vector3f(0.0f, distance * 2.0f, distance * 2.0f), Matrix33f::CreateRotateAroundZ(3.14f));
	AddWall(Vector3f(0.0f, distance * 2.0f, distance * 3.0f), Matrix33f::CreateRotateAroundZ(3.14f));

	AddWall(Vector3f(-distance * 0.5f, distance * 0.5f, distance * 2.0f), Matrix33f::CreateRotateAroundZ(1.57f));
	AddWall(Vector3f(-distance * 0.5f, distance * 0.5f, distance * 3.0f), Matrix33f::CreateRotateAroundZ(1.57f));
	AddWall(Vector3f(distance * 0.5f, distance * 0.5f, distance * 2.0f), Matrix33f::CreateRotateAroundZ(-1.57f));
	AddWall(Vector3f(distance * 0.5f, distance * 0.5f, distance * 3.0f), Matrix33f::CreateRotateAroundZ(-1.57f));

	AddWall(Vector3f(-distance * 0.5f, distance * 1.5f, distance * 2.0f), Matrix33f::CreateRotateAroundZ(1.57f));
	AddWall(Vector3f(-distance * 0.5f, distance * 1.5f, distance * 3.0f), Matrix33f::CreateRotateAroundZ(1.57f));
	AddWall(Vector3f(distance * 0.5f, distance * 1.5f, distance * 2.0f), Matrix33f::CreateRotateAroundZ(-1.57f));
	AddWall(Vector3f(distance * 0.5f, distance * 1.5f, distance * 3.0f), Matrix33f::CreateRotateAroundZ(-1.57f));

	////Room 2
	myCurrentAmbientProbeIndex = 2;
	AddWall(Vector3f(0.0f, 0.0f, distance * 4.0f));
	AddWall(Vector3f(distance, 0.0f, distance * 4.0f));
	AddWall(Vector3f(0.0f, 0.0f, distance * 5.0f));
	AddWall(Vector3f(distance, 0.0f, distance * 5.0f));

	AddWall(Vector3f(0.0f, distance * 2.0f, distance * 4.0f), Matrix33f::CreateRotateAroundZ(3.14f));
	AddWall(Vector3f(distance, distance * 2.0f, distance * 4.0f), Matrix33f::CreateRotateAroundZ(3.14f));
	AddWall(Vector3f(0.0f, distance * 2.0f, distance * 5.0f), Matrix33f::CreateRotateAroundZ(3.14f));
	AddWall(Vector3f(distance, distance * 2.0f, distance * 5.0f), Matrix33f::CreateRotateAroundZ(3.14f));

	AddWall(Vector3f(-distance * 0.5f, distance * 0.5f, distance * 4.0f), Matrix33f::CreateRotateAroundZ(1.57f));
	AddWall(Vector3f(-distance * 0.5f, distance * 0.5f, distance * 5.0f), Matrix33f::CreateRotateAroundZ(1.57f));
	AddWall(Vector3f(distance * 1.5f, distance * 0.5f, distance * 4.0f), Matrix33f::CreateRotateAroundZ(-1.57f));
	AddWall(Vector3f(distance * 1.5f, distance * 0.5f, distance * 5.0f), Matrix33f::CreateRotateAroundZ(-1.57f));
	AddWall(Vector3f(distance, distance * 0.5f, distance * 5.5f), Matrix33f::CreateRotateAroundX(1.57f));
	AddWall(Vector3f(distance, distance * 0.5f, distance * 3.5f), Matrix33f::CreateRotateAroundX(-1.57f));

	AddWall(Vector3f(-distance * 0.5f, distance * 1.5f, distance * 4.0f), Matrix33f::CreateRotateAroundZ(1.57f));
	AddWall(Vector3f(-distance * 0.5f, distance * 1.5f, distance * 5.0f), Matrix33f::CreateRotateAroundZ(1.57f));
	AddWall(Vector3f(distance * 1.5f, distance * 1.5f, distance * 4.0f), Matrix33f::CreateRotateAroundZ(-1.57f));
	AddWall(Vector3f(distance * 1.5f, distance * 1.5f, distance * 5.0f), Matrix33f::CreateRotateAroundZ(-1.57f));
	AddWall(Vector3f(distance, distance * 1.5f, distance * 5.5f), Matrix33f::CreateRotateAroundX(1.57f));
	AddWall(Vector3f(distance, distance * 1.5f, distance * 3.5f), Matrix33f::CreateRotateAroundX(-1.57f));

	AddBallInstance(Vector3f(halfWallSize, halfWallSize * 0.35f, distance * 4.5f));

	//Corridoor 2
	myCurrentAmbientProbeIndex = 3;
	AddWall(Vector3f(0.0f, 0.0f, distance * 6.0f));
	AddWall(Vector3f(0.0f, 0.0f, distance * 7.0f));

	AddWall(Vector3f(0.0f, distance * 2.0f, distance * 6.0f), Matrix33f::CreateRotateAroundZ(3.14f));
	AddWall(Vector3f(0.0f, distance * 2.0f, distance * 7.0f), Matrix33f::CreateRotateAroundZ(3.14f));

	AddWall(Vector3f(-distance * 0.5f, distance * 0.5f, distance * 6.0f), Matrix33f::CreateRotateAroundZ(1.57f));
	AddWall(Vector3f(-distance * 0.5f, distance * 0.5f, distance * 7.0f), Matrix33f::CreateRotateAroundZ(1.57f));
	AddWall(Vector3f(distance * 0.5f, distance * 0.5f, distance * 6.0f), Matrix33f::CreateRotateAroundZ(-1.57f));
	AddWall(Vector3f(distance * 0.5f, distance * 0.5f, distance * 7.0f), Matrix33f::CreateRotateAroundZ(-1.57f));

	AddWall(Vector3f(-distance * 0.5f, distance * 1.5f, distance * 6.0f), Matrix33f::CreateRotateAroundZ(1.57f));
	AddWall(Vector3f(-distance * 0.5f, distance * 1.5f, distance * 7.0f), Matrix33f::CreateRotateAroundZ(1.57f));
	AddWall(Vector3f(distance * 0.5f, distance * 1.5f, distance * 6.0f), Matrix33f::CreateRotateAroundZ(-1.57f));
	AddWall(Vector3f(distance * 0.5f, distance * 1.5f, distance * 7.0f), Matrix33f::CreateRotateAroundZ(-1.57f));

	//Room 3
	myCurrentAmbientProbeIndex = 4;
	AddWall(Vector3f(0.0f, 0.0f, distance * 8.0f));
	AddWall(Vector3f(distance, 0.0f, distance * 8.0f));
	AddWall(Vector3f(0.0f, 0.0f, distance * 9.0f));
	AddWall(Vector3f(distance, 0.0f, distance * 9.0f));

	AddWall(Vector3f(0.0f, distance * 2.0f, distance * 8.0f), Matrix33f::CreateRotateAroundZ(3.14f));
	AddWall(Vector3f(distance, distance * 2.0f, distance * 8.0f), Matrix33f::CreateRotateAroundZ(3.14f));
	AddWall(Vector3f(0.0f, distance * 2.0f, distance * 9.0f), Matrix33f::CreateRotateAroundZ(3.14f));
	AddWall(Vector3f(distance, distance * 2.0f, distance * 9.0f), Matrix33f::CreateRotateAroundZ(3.14f));

	AddWall(Vector3f(-distance * 0.5f, distance * 0.5f, distance * 8.0f), Matrix33f::CreateRotateAroundZ(1.57f));
	AddWall(Vector3f(-distance * 0.5f, distance * 0.5f, distance * 9.0f), Matrix33f::CreateRotateAroundZ(1.57f));
	AddWall(Vector3f(distance * 1.5f, distance * 0.5f, distance * 8.0f), Matrix33f::CreateRotateAroundZ(-1.57f));
	AddWall(Vector3f(distance * 1.5f, distance * 0.5f, distance * 9.0f), Matrix33f::CreateRotateAroundZ(-1.57f));
	AddWall(Vector3f(distance, distance * 0.5f, distance * 9.5f), Matrix33f::CreateRotateAroundX(1.57f));
	AddWall(Vector3f(0.0f, distance * 0.5f, distance * 9.5f), Matrix33f::CreateRotateAroundX(1.57f));
	AddWall(Vector3f(distance, distance * 0.5f, distance * 7.5f), Matrix33f::CreateRotateAroundX(-1.57f));

	AddWall(Vector3f(-distance * 0.5f, distance * 1.5f, distance * 8.0f), Matrix33f::CreateRotateAroundZ(1.57f));
	AddWall(Vector3f(-distance * 0.5f, distance * 1.5f, distance * 9.0f), Matrix33f::CreateRotateAroundZ(1.57f));
	AddWall(Vector3f(distance * 1.5f, distance * 1.5f, distance * 8.0f), Matrix33f::CreateRotateAroundZ(-1.57f));
	AddWall(Vector3f(distance * 1.5f, distance * 1.5f, distance * 9.0f), Matrix33f::CreateRotateAroundZ(-1.57f));
	AddWall(Vector3f(distance, distance * 1.5f, distance * 9.5f), Matrix33f::CreateRotateAroundX(1.57f));
	AddWall(Vector3f(0.0f, distance * 1.5f, distance * 9.5f), Matrix33f::CreateRotateAroundX(1.57f));
	AddWall(Vector3f(distance, distance * 1.5f, distance * 7.5f), Matrix33f::CreateRotateAroundX(-1.57f));

	AddBallInstance(Vector3f(4.5f, halfWallSize * 0.25f, distance * 9.0f + 2.5f));
	AddBallInstance(Vector3f(0.0f, halfWallSize * 0.25f, distance * 9.0f));
	AddBallInstance(Vector3f(-2.5f, halfWallSize * 0.25f, distance * 9.0f - 4.5f));

	Light* dirLight = new Light(Light::DIRECTIONAL_LIGHT_TYPE);
	dirLight->SetLightColor(Vector4f(0.15f, 0.15f, 0.15f, 1.0f));
	dirLight->SetLightDir(Vector3f(0.5f, 1.0f, 0.1f));
	//Engine::GetInstance()->GetScene().AddLight(dirLight);

	Light* spotLight1 = new Light(Light::SPOT_LIGHT_TYPE);
	//spotLight1->SetPosition(Vector3f(halfWallSize, halfWallSize, halfWallSize));
	spotLight1->SetPosition(Vector3f(halfWallSize, halfWallSize * 2.0f, halfWallSize));
	spotLight1->SetLightColor(Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
	spotLight1->SetMaxDistance(50.0f);
	spotLight1->SetInnerFallofAngle(0.625f);
	spotLight1->SetOuterFallofAngle(0.65f);
	//spotLight1->SetLightDir(Vector3f(1.0f, 0.0f, 0.0f));
	spotLight1->SetLightDir(Vector3f(1.0f, 0.0f, 0.0f));
	mySpotLights.Add(spotLight1);
	Engine::GetInstance()->GetScene().AddLight(spotLight1);

	Light* spotLightRoom2 = new Light(Light::SPOT_LIGHT_TYPE);
	spotLightRoom2->SetPosition(Vector3f(halfWallSize, halfWallSize, distance * 4.5f));
	spotLightRoom2->SetLightColor(Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
	spotLightRoom2->SetMaxDistance(50.0f);
	spotLightRoom2->SetInnerFallofAngle(0.95f);
	spotLightRoom2->SetOuterFallofAngle(0.975f);
	spotLightRoom2->SetLightDir(Vector3f(0.0001f, -1.0f, 0.0f));
	mySpotLights.Add(spotLightRoom2);
	Engine::GetInstance()->GetScene().AddLight(spotLightRoom2);

	Light* spotLightRoom3 = new Light(Light::SPOT_LIGHT_TYPE);
	spotLightRoom3->SetPosition(Vector3f(halfWallSize, halfWallSize, distance * 8.5f));
	spotLightRoom3->SetLightColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	spotLightRoom3->SetMaxDistance(50.0f);
	spotLightRoom3->SetInnerFallofAngle(0.45f);
	spotLightRoom3->SetOuterFallofAngle(0.475f);
	spotLightRoom3->SetLightDir(Vector3f(-1.0f, -0.5f, 1.0f));
	mySpotLights.Add(spotLightRoom3);
	Engine::GetInstance()->GetScene().AddLight(spotLightRoom3);
}

void Application::AddWall(Vector3f aPosition, Matrix33f aRotation)
{
	Instance* tempWall = new Instance(*myGroundModels[0]);
	tempWall->Init();
	tempWall->SetPosition(aPosition);
	tempWall->SetRotation(aRotation);
	tempWall->myAmbientProbeIndex = myCurrentAmbientProbeIndex;
	myInstances.Add(tempWall);
	Engine::GetInstance()->GetScene().AddInstance(tempWall);
}

void Application::AddLight()
{
	const float X = static_cast<float>(rand() % 100 - 50);
	const float Y = static_cast<float>(rand() % 5 + 1);
	const float Z = static_cast<float>(rand() % 99 - 17);
	Vector4f lightColor(static_cast<float>(rand() % 500) / 660.0f + 0.25f, static_cast<float>(rand() % 500) / 660.0f + 0.25f, static_cast<float>(rand() % 500) / 660.0f + 0.25f, 1.0f);

	Light* pointLight = new Light(Light::POINT_LIGHT_TYPE);
	pointLight->SetPosition(Vector3f(X, Y, Z));
	pointLight->SetLightColor(lightColor);
	pointLight->SetMaxDistance(3.0f + static_cast<float>(rand() % 5));
	myPointLights.Add(pointLight);
	Engine::GetInstance()->GetScene().AddLight(pointLight);
}

void Application::RemoveLight()
{
	if(myPointLights.Count() > 0)
	{
		Engine::GetInstance()->GetScene().RemoveLight(myPointLights[myPointLights.Count() - 1]);
		myPointLights.DeleteCyclicAtIndex(myPointLights.Count() - 1);
	}
}

void Application::AddBallInstance(Vector3f aPosition)
{
	Instance *ballInstance = new Instance(*myBallModels[0]);
	ballInstance->Init();
	ballInstance->SetPosition(aPosition); 
	ballInstance->myAmbientProbeIndex = myCurrentAmbientProbeIndex;
	myInstances.Add(ballInstance);
	Engine::GetInstance()->GetScene().AddInstance(ballInstance);
}

void Application::RemoveBallInstance()
{
	if(myInstances.Count() > 9)
	{
		Engine::GetInstance()->GetScene().RemoveInstance(myInstances[myInstances.Count() - 1]);
		myInstances.DeleteCyclicAtIndex(myInstances.Count() - 1);
	}
}
