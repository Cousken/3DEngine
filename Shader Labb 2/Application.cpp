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

bool Application::Init()
{
	myInputHandler = new Library2D::Input(GetModuleHandle(NULL), Engine::GetInstance()->GetWindowHandler());

	InitModels();
	//InitCubeRoomMap();
	InitPlaneWithBallsMap();
	//InitThreeRooms();

	Instance *sphere = new Instance(*myBallModel);
	sphere->Init();
	sphere->SetPosition(Vector3f(0.0f, 0.0f, 0.0f)); 
	myInstances[1000] = sphere;
	//Engine::GetInstance()->GetScene().AddInstance(sphere);
	Engine::GetInstance()->GetRenderer().GetFullScreenHelper().SetDeferredRenderingSphere(sphere);
	
	Engine::GetInstance()->GetScene().SetCamera(myCamera);	

	myKeyPressCooldownTimer = 0.0f;

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
	myKeyPressCooldownTimer += delta;

	HandleInput(delta);
	myStreak.Move(Vector3f(delta, 0,0));

	myTime += delta;
	Engine::GetInstance()->GetEffectInput().SetScalar("time", myTime);
	Engine::GetInstance()->GetEffectInput().SetScalar("DeltaTime", delta);
	Engine::GetInstance()->GetScene().Update(delta);

	UpdateFPS( delta );

	if ( myBlueLight != NULL)
	{
		myBlueLight->Rotate( Matrix33f::CreateRotateAroundY(delta*2) );
	}
	if ( myRedLight != NULL)
	{
		myRedLight->Rotate( Matrix33f::CreateRotateAroundY( -delta) );
	}

	myGfxEngine.SwitchBuffers();
	myGfxEngine.RenderScene();
	return(true);
}

Application::Application( Engine& aGFXEngine )
: myGfxEngine(aGFXEngine)
{
	myCamera = Camera();
	myTime = 0;
	myGroundModel = 0;
	myLevelModel = NULL;
	myBlueLight = NULL;
	myRedLight = NULL;

	myHasRenderedCube = 0;
}

Application::~Application()
{

}

void Application::Render()
{
	Engine::GetInstance()->GetScene().SetCamera(myCamera);

	if ( myHasRenderedCube < 2 )
	{
//		RenderReflectionCubeMap();
		myHasRenderedCube++;
	}

	Engine::GetInstance()->GetRenderer().Render();
}

void Application::HandleInput( float delta )
{
	myInputHandler->Update();

	Vector3f cameraMovement = Vector3f(0,0,0);
	if (myInputHandler->IsKeyDown(Keys::W) == true)
	{
		cameraMovement.z += CAMERA_SPEED * delta;
	}
	if (myInputHandler->IsKeyDown(Keys::S) == true)
	{
		cameraMovement.z -= CAMERA_SPEED  * delta;
	}
	if (myInputHandler->IsKeyDown(Keys::A) == true)
	{
		cameraMovement.x -= CAMERA_SPEED  * delta;
	}
	if (myInputHandler->IsKeyDown(Keys::D) == true)
	{
		cameraMovement.x += CAMERA_SPEED * delta;
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
			myKeyPressCooldownTimer = 0.0f;
			myGfxEngine.GetRenderer().myToggleQuadOrFullScreen = !myGfxEngine.GetRenderer().myToggleQuadOrFullScreen;
		}
	}
	if (myInputHandler->IsKeyDown(Keys::F2) == true)
	{
		if(myKeyPressCooldownTimer > LocKeyPressCooldown)
		{
			myKeyPressCooldownTimer = 0.0f;
			myGfxEngine.GetRenderer().myToggleAdvancedCulling = !myGfxEngine.GetRenderer().myToggleAdvancedCulling;
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

	if(myInputHandler->IsKeyDown(Keys::Escape) == true)
	{
		exit(0);
	}

	myCamera.Rotate(cameraRotation);
}

void Application::UpdateFPS(const float aDeltaTime)
{
	const std::string stringToDisplay = "FPS: ";
	static float timer = 0;
	static int frameCounter = 0;
	timer += aDeltaTime;
	frameCounter++;

	if ( timer >= 1.0f)
	{
		timer = 0.0f;
		std::string temp = "F1: Toggle Fullscreen || F2: Toggle Culling || F3: Toggle Point/Spotlights || ";
		temp +=	stringToDisplay + ConvertToString(frameCounter);
		Engine::GetInstance()->SetTitle( temp );
		frameCounter = 0;
	}
}

std::string Application::ConvertToString( const int aType )
{
	std::stringstream converter;
	converter << aType;
	return converter.str();
}

void Application::RenderReflectionCubeMap()
{
	CommonUtilities::StaticArray< Matrix44f, 6 > toViewMatrixes;

	D3DXMATRIX matrix;
	Matrix44f ownMatixFormat;
	Vector3f at = myBallPosition + Vector3f( 1, 0, 0 );
	Vector3f up( 0, 1, 0 );
	D3DXMatrixLookAtLH( &matrix, reinterpret_cast< D3DXVECTOR3* >( &myBallPosition.x ), reinterpret_cast< D3DXVECTOR3* >( &at.x ), reinterpret_cast< D3DXVECTOR3* >( &up.x ) );
	ownMatixFormat.ConvertD3D(matrix);
	toViewMatrixes[ 0 ] = Matrix44f( ownMatixFormat );

	at = myBallPosition + Vector3f( -1, 0, 0 );
	D3DXMatrixLookAtLH( &matrix, reinterpret_cast< D3DXVECTOR3* >( &myBallPosition.x ), reinterpret_cast< D3DXVECTOR3* >( &at.x ), reinterpret_cast< D3DXVECTOR3* >( &up.x ) );
	ownMatixFormat.ConvertD3D(matrix);
	toViewMatrixes[ 1 ] = Matrix44f( ownMatixFormat );

	up = Vector3f( 1, 0, 0 );
	at = myBallPosition + Vector3f( 0, 1, 0 );
	D3DXMatrixLookAtLH( &matrix, reinterpret_cast< D3DXVECTOR3* >( &myBallPosition.x ), reinterpret_cast< D3DXVECTOR3* >( &at.x ), reinterpret_cast< D3DXVECTOR3* >( &up.x ) );
	ownMatixFormat.ConvertD3D(matrix);
	toViewMatrixes[ 2 ] = Matrix44f( ownMatixFormat );

	up = Vector3f( -1, 0, 0 );
	at = myBallPosition + Vector3f( 0, -1, 0 );
	D3DXMatrixLookAtLH( &matrix, reinterpret_cast< D3DXVECTOR3* >( &myBallPosition.x ), reinterpret_cast< D3DXVECTOR3* >( &at.x ), reinterpret_cast< D3DXVECTOR3* >( &up.x ) );
	ownMatixFormat.ConvertD3D(matrix);
	toViewMatrixes[ 3 ] = Matrix44f( ownMatixFormat );

	up = Vector3f( 0, 1, 0 );
	at = myBallPosition + Vector3f( 0, 0, 1 );
	D3DXMatrixLookAtLH( &matrix, reinterpret_cast< D3DXVECTOR3* >( &myBallPosition.x ), reinterpret_cast< D3DXVECTOR3* >( &at.x ), reinterpret_cast< D3DXVECTOR3* >( &up.x ) );
	ownMatixFormat.ConvertD3D(matrix);
	toViewMatrixes[ 4 ] = Matrix44f( ownMatixFormat );

	at = myBallPosition + Vector3f( 0, 0, -1 );
	D3DXMatrixLookAtLH( &matrix, reinterpret_cast< D3DXVECTOR3* >( &myBallPosition.x ), reinterpret_cast< D3DXVECTOR3* >( &at.x ), reinterpret_cast< D3DXVECTOR3* >( &up.x ) );
	ownMatixFormat.ConvertD3D(matrix);
	toViewMatrixes[ 5 ] = Matrix44f( ownMatixFormat );
	//myToView = toViewMatrixes[ 5 ];

	Engine::GetInstance()->GetRenderer().RenderToCube( toViewMatrixes );
	
}

void Application::InitModels()
{
	ColladaMeshFactory factory;

	myGroundModel = factory.CreateModel("Plane_Checker.dae");
	bool success = myGroundModel->InitBuffers();
	assert( (success == true) && "Could not initiate the model!");

	myBallModel = factory.CreateModel("Sphere.dae");
	success = myGroundModel->InitBuffers();
	assert( (success == true) && "Could not initiate the model!");

	myLevelModel = factory.CreateModel("model.dae");
	success = myGroundModel->InitBuffers();
	assert( (success == true) && "Could not initiate the model!");
}


void Application::InitCubeRoomMap()
{
	const float distance = 33.3333f;

	Instance* temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f(0,0,0));
	myInstances[0] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);

	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f(0, distance ,0)); 
	temp->PerformRotation( Matrix33f::CreateRotateAroundX( PI ) );
	myInstances[1] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);

	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f( 0 , distance / 2 , (distance/2))); 
	temp->PerformRotation( Matrix33f::CreateRotateAroundX( PI / 2 ) );
	myInstances[2] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);

	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f( 0 , distance / 2 , -(distance/2))); 
	temp->PerformRotation( Matrix33f::CreateRotateAroundX( -(PI / 2 )) );
	myInstances[3] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);

	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f( -(distance/2) , distance / 2 , 0 )); 
	temp->PerformRotation( Matrix33f::CreateRotateAroundX( -(PI / 2 )) );
	temp->PerformRotation( Matrix33f::CreateRotateAroundZ( (PI / 2 )) );
	myInstances[4] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);

	myBallPosition = Vector3f( 0 , 0 , 0 );

	temp = new Instance(*myBallModel);
	temp->Init();
	temp->SetPosition( myBallPosition ); 
	myInstances[6] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);

	myBallPosition = Vector3f( 1 , 1 , 1 );

	temp = new Instance(*myBallModel);
	temp->Init();
	temp->SetPosition( myBallPosition ); 
	myInstances[6] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);

	myBallPosition = Vector3f( -1 , 1 , -1 );

	temp = new Instance(*myBallModel);
	temp->Init();
	temp->SetPosition( myBallPosition ); 
	myInstances[6] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);

	//Light* tempLight = new Light(Light::DIRECTIONAL_LIGHT_TYPE);
	//tempLight->SetLightDir( Vector3f(1.0f, 0.0f, 0.0f) );
	//tempLight->SetLightColor(Vector4f(0.0f, 0.0f, 1.0f, 1.0f) );
	//myLights.Insert(0, tempLight);
	//Engine::GetInstance()->GetScene().AddLight(tempLight);

	//Light* tempPointLight = new Light(Light::POINT_LIGHT_TYPE);
	//tempPointLight->SetPosition(Vector3f(0.0f, 5.0f, 5.0f));
	//tempPointLight->SetLightColor(Vector4f(0.0f, 1.0f, 1.0f, 1.0f));
	//tempPointLight->SetMaxDistance(25.0f);
	//myLights.Insert(1, tempPointLight);
	//Engine::GetInstance()->GetScene().AddLight(tempPointLight);

	//Light* tempPointLight2 = new Light(Light::POINT_LIGHT_TYPE);
	//tempPointLight2->SetPosition(Vector3f(5.0f, 28.0f, -5.0f));
	//tempPointLight2->SetLightColor(Vector4f(1.0f, 0.0f, 1.0f, 1.0f));
	//tempPointLight2->SetMaxDistance(25.0f);
	//myLights.Insert(2, tempPointLight2);
	//Engine::GetInstance()->GetScene().AddLight(tempPointLight2);

// 	Light* tempSpotLight = new Light(Light::SPOT_LIGHT_TYPE);
// 	tempSpotLight->SetPosition(Vector3f(25.0f, 17.0f, 0.0f));
// 	tempSpotLight->SetLightColor(Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
// 	tempSpotLight->SetMaxDistance(100.0f);
// 	tempSpotLight->SetLightDir(Vector3f(-1.0f, 0.0f, 0.0f));
// 	tempSpotLight->SetInnerFallofAngle(0.8f);
// 	tempSpotLight->SetOuterFallofAngle(0.8f);
// 	myLights.Insert(3, tempSpotLight);
// 	Engine::GetInstance()->GetScene().AddLight(tempSpotLight);

	const float X = 0.0f;
	const float Y = 10.0f;
	const float Z = 0.0f;
	Vector4f lightColor(0.0f, 0.0f , 1.0f, 1.0f);
	//lightColor = lightColor.Normalize();

	Light* spotLight = new Light(Light::SPOT_LIGHT_TYPE);
	spotLight->SetPosition(Vector3f(X, Y, Z));
	spotLight->SetLightColor(lightColor);
	spotLight->SetMaxDistance(200.0f);
	spotLight->SetLightDir(Vector3f(0.01f, -1.0f, 0.01f).Normalize());
	spotLight->SetInnerFallofAngle(0.3f);
	spotLight->SetOuterFallofAngle(0.6f);
	myLights.Insert(1, spotLight);
	Engine::GetInstance()->GetScene().AddLight(spotLight);

	myCamera.SetPosition(Vector3f(33.3f, 16.7f, 0.0f));
	myCamera.Rotate(Matrix33f::CreateRotateAroundY(1.57f));
}

void Application::InitPlaneWithBallsMap()
{
	//FLOOR GENERATION
	const int NumberOfFloorsSquared = 3;
	int indexCount = 0;
	for(int index = 0; index < NumberOfFloorsSquared; index++)
	{
		for(int index2 = 0; index2 < NumberOfFloorsSquared; index2++)
		{
			const float X = 33.3333f * (-1.0f + static_cast<float>(index));
			const float Z = 33.3333f * static_cast<float>(index2);
			
			Instance* floorInstance = new Instance(*myGroundModel);
			floorInstance->Init();
			floorInstance->SetPosition(Vector3f(X, 0, Z));
			myInstances[indexCount] = floorInstance;
			Engine::GetInstance()->GetScene().AddInstance(floorInstance);

			indexCount++;
		}
	}

	//BALL GENERATION
	const int NumberOfBalls = 100;
	for(int index = 0; index < NumberOfBalls; index++)
	{
		const float X = static_cast<float>(rand() % 100 - 50);
		const float Y = static_cast<float>(rand() % 10 + 1);
		const float Z = static_cast<float>(rand() % 99 - 17);
			
		Instance *ballInstance = new Instance(*myBallModel);
		ballInstance->Init();
		ballInstance->SetPosition(Vector3f(X, Y, Z)); 
		myInstances[(NumberOfFloorsSquared * NumberOfFloorsSquared) + index] = ballInstance;
		Engine::GetInstance()->GetScene().AddInstance(ballInstance);
	}

	//POINT LIGHT GENERATION
	const int LightsOfEachType = 10;
	for(int index = 0; index < LightsOfEachType; index++)
	{
		const float X = static_cast<float>(rand() % 100 - 50);
		const float Y = static_cast<float>(rand() % 20 + 1);
		const float Z = static_cast<float>(rand() % 99 - 17);
		Vector4f lightColor(static_cast<float>(rand() % 500) / 660.0f + 0.25f, static_cast<float>(rand() % 500) / 660.0f + 0.25f, static_cast<float>(rand() % 500) / 660.0f + 0.25f, 1.0f);

		Light* pointLight = new Light(Light::POINT_LIGHT_TYPE);
		pointLight->SetPosition(Vector3f(X, Y, Z));
		pointLight->SetLightColor(lightColor);
		pointLight->SetMaxDistance(10.0f);
		myLights.Insert(index, pointLight);
		Engine::GetInstance()->GetScene().AddLight(pointLight);
	}

	//SPOT LIGHT GENERATION
	for(int index = 0; index < LightsOfEachType; index++)
	{
		const float X = static_cast<float>(rand() % 100 - 50);
		const float Y = static_cast<float>(rand() % 40 + 5);
		const float Z = static_cast<float>(rand() % 99 - 17);
		Vector4f lightColor(static_cast<float>(rand() % 500) / 660.0f + 0.25f, static_cast<float>(rand() % 500) / 660.0f + 0.25f, static_cast<float>(rand() % 500) / 660.0f + 0.25f, 1.0f);
		lightColor = lightColor.Normalize();

		Light* spotLight = new Light(Light::SPOT_LIGHT_TYPE);
		spotLight->SetPosition(Vector3f(X, Y, Z));
		spotLight->SetLightColor(lightColor);
		spotLight->SetMaxDistance(200.0f);
		spotLight->SetLightDir(Vector3f(0.0f, -1.0f, 0.1f).Normalize());
		spotLight->SetInnerFallofAngle(0.3f);
		spotLight->SetOuterFallofAngle(0.3f);
		myLights.Insert(index + LightsOfEachType, spotLight);
		Engine::GetInstance()->GetScene().AddLight(spotLight);
	}

	myCamera.SetPosition(Vector3f(0.0f, 15.0f, -30.0f));
}

void Application::InitThreeRooms()
{
	const float distance = 33.3333f;

	InitRoomOne(distance);


	const float X = 0.0f;
	const float Y = 10.0f;
	const float Z = 0.0f;
	Vector4f lightColor(1.0f, 0.0f , 0.0f, 1.0f);
	//lightColor = lightColor.Normalize();

	Light* spotLight = new Light(Light::SPOT_LIGHT_TYPE);
	spotLight->SetPosition(Vector3f(X, Y, Z));
	spotLight->SetLightColor(lightColor);
	spotLight->SetMaxDistance(200.0f);
	spotLight->SetLightDir(Vector3f(0.3f, 0.01f, 1.0f).Normalize());
	spotLight->SetInnerFallofAngle(0.3f);
	spotLight->SetOuterFallofAngle(0.6f);
	spotLight->LoadProjectiveTexture("blinds.dds");
	myLights.Insert(0, spotLight);
	//myRedLight = spotLight;
	Engine::GetInstance()->GetScene().AddLight(spotLight);

	Light* point = new Light(Light::POINT_LIGHT_TYPE);
	point->SetPosition(Vector3f(X, Y, Z));
	point->SetLightColor(Vector4f( 0.0f, 1.0f, 0.0f, 1.0f));
	point->SetMaxDistance(2000.0f);
	point->LoadProjectiveTexture("cubemap.dds");
	myLights.Insert(2, point);
	//myRedLight = spotLight;
	Engine::GetInstance()->GetScene().AddLight(point);

	myCamera.SetPosition(Vector3f(33.3f, 16.7f, 0.0f));
	myCamera.Rotate(Matrix33f::CreateRotateAroundY(1.57f));
}

void Application::InitRoomOne( const float aDistance )
{
	//Floor
	Instance* temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f(0,0,0));
	myInstances[0] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f(0,0,-aDistance));
	myInstances[0] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f(0,0, aDistance));
	myInstances[0] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f(-aDistance,0,0));
	myInstances[0] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f(-aDistance,0,-aDistance));
	myInstances[0] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f(-aDistance,0, aDistance));
	myInstances[0] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);

	// ceiling
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f(0, 2*aDistance ,0)); 
	temp->PerformRotation( Matrix33f::CreateRotateAroundX( PI ) );
	myInstances[1] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f(0, 2*aDistance ,aDistance)); 
	temp->PerformRotation( Matrix33f::CreateRotateAroundX( PI ) );
	myInstances[1] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f(0, 2*aDistance ,-aDistance)); 
	temp->PerformRotation( Matrix33f::CreateRotateAroundX( PI ) );
	myInstances[1] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f(-aDistance, 2*aDistance ,0)); 
	temp->PerformRotation( Matrix33f::CreateRotateAroundX( PI ) );
	myInstances[1] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f(-aDistance, 2*aDistance ,aDistance)); 
	temp->PerformRotation( Matrix33f::CreateRotateAroundX( PI ) );
	myInstances[1] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f(-aDistance, 2*aDistance ,-aDistance)); 
	temp->PerformRotation( Matrix33f::CreateRotateAroundX( PI ) );
	myInstances[1] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);


	// right wall
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f( 0 , aDistance / 2 , (aDistance*1.5f))); 
	temp->PerformRotation( Matrix33f::CreateRotateAroundX( PI / 2 ) );
	myInstances[2] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f( -aDistance , aDistance / 2 , (aDistance*1.5f))); 
	temp->PerformRotation( Matrix33f::CreateRotateAroundX( PI / 2 ) );
	myInstances[2] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f( 0 , aDistance*1.5f , (aDistance*1.5f))); 
	temp->PerformRotation( Matrix33f::CreateRotateAroundX( PI / 2 ) );
	myInstances[2] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f( -aDistance , aDistance*1.5f , (aDistance*1.5f))); 
	temp->PerformRotation( Matrix33f::CreateRotateAroundX( PI / 2 ) );
	myInstances[2] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);
	
	// left wall
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f( 0 , aDistance / 2 , -(aDistance*1.5f))); 
	temp->PerformRotation( Matrix33f::CreateRotateAroundX( -(PI / 2 )) );
	myInstances[3] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f( -aDistance , aDistance / 2 , -(aDistance*1.5f))); 
	temp->PerformRotation( Matrix33f::CreateRotateAroundX( -(PI / 2 )) );
	myInstances[3] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f( 0 , aDistance*1.5f , -(aDistance*1.5f))); 
	temp->PerformRotation( Matrix33f::CreateRotateAroundX( -(PI / 2 )) );
	myInstances[3] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f( -aDistance , aDistance*1.5f , -(aDistance*1.5f))); 
	temp->PerformRotation( Matrix33f::CreateRotateAroundX( -(PI / 2 )) );
	myInstances[3] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);

	// back wall
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f( -(aDistance*1.5f), aDistance / 2 , 0 )); 
	temp->PerformRotation( Matrix33f::CreateRotateAroundX( -(PI / 2 )) );
	temp->PerformRotation( Matrix33f::CreateRotateAroundZ( (PI / 2 )) );
	myInstances[4] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f( -(aDistance*1.5f), aDistance / 2 , -aDistance )); 
	temp->PerformRotation( Matrix33f::CreateRotateAroundX( -(PI / 2 )) );
	temp->PerformRotation( Matrix33f::CreateRotateAroundZ( (PI / 2 )) );
	myInstances[4] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f( -(aDistance*1.5f), aDistance / 2 , aDistance )); 
	temp->PerformRotation( Matrix33f::CreateRotateAroundX( -(PI / 2 )) );
	temp->PerformRotation( Matrix33f::CreateRotateAroundZ( (PI / 2 )) );
	myInstances[4] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f( -(aDistance*1.5f), aDistance * 1.5f , 0 )); 
	temp->PerformRotation( Matrix33f::CreateRotateAroundX( -(PI / 2 )) );
	temp->PerformRotation( Matrix33f::CreateRotateAroundZ( (PI / 2 )) );
	myInstances[4] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f( -(aDistance*1.5f), aDistance * 1.5f , -aDistance )); 
	temp->PerformRotation( Matrix33f::CreateRotateAroundX( -(PI / 2 )) );
	temp->PerformRotation( Matrix33f::CreateRotateAroundZ( (PI / 2 )) );
	myInstances[4] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f( -(aDistance*1.5f), aDistance * 1.5f , aDistance )); 
	temp->PerformRotation( Matrix33f::CreateRotateAroundX( -(PI / 2 )) );
	temp->PerformRotation( Matrix33f::CreateRotateAroundZ( (PI / 2 )) );
	myInstances[4] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);

	// front wall
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f( (aDistance*0.5f), aDistance / 2 , 0 )); 
	temp->PerformRotation( Matrix33f::CreateRotateAroundX( -(PI / 2 )) );
	temp->PerformRotation( Matrix33f::CreateRotateAroundZ( -(PI / 2 )) );
	myInstances[4] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f( (aDistance*0.5f), aDistance / 2 , -aDistance )); 
	temp->PerformRotation( Matrix33f::CreateRotateAroundX( -(PI / 2 )) );
	temp->PerformRotation( Matrix33f::CreateRotateAroundZ( -(PI / 2 )) );
	myInstances[4] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f( (aDistance*0.5f), aDistance / 2 , aDistance )); 
	temp->PerformRotation( Matrix33f::CreateRotateAroundX( -(PI / 2 )) );
	temp->PerformRotation( Matrix33f::CreateRotateAroundZ( -(PI / 2 )) );
	myInstances[4] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f( (aDistance*0.5f), aDistance * 1.5f , 0 )); 
	temp->PerformRotation( Matrix33f::CreateRotateAroundX( -(PI / 2 )) );
	temp->PerformRotation( Matrix33f::CreateRotateAroundZ( -(PI / 2 )) );
	myInstances[4] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f( (aDistance*0.5f), aDistance * 1.5f , -aDistance )); 
	temp->PerformRotation( Matrix33f::CreateRotateAroundX( -(PI / 2 )) );
	temp->PerformRotation( Matrix33f::CreateRotateAroundZ( -(PI / 2 )) );
	myInstances[4] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);
	temp = new Instance(*myGroundModel);
	temp->Init();
	temp->SetPosition(Vector3f( (aDistance*0.5f), aDistance * 1.5f , aDistance )); 
	temp->PerformRotation( Matrix33f::CreateRotateAroundX( -(PI / 2 )) );
	temp->PerformRotation( Matrix33f::CreateRotateAroundZ( -(PI / 2 )) );
	myInstances[4] = temp;
	Engine::GetInstance()->GetScene().AddInstance(temp);


	//SPOTLIGHT
	bool success = false;
	Light* spotLight = new Light(Light::SPOT_LIGHT_TYPE);
	spotLight->SetPosition(Vector3f(0.0f, 60.0f, 0.0f));
	spotLight->SetLightColor(Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
	spotLight->SetMaxDistance(100.0f);
	spotLight->SetLightDir(Vector3f(0.01f, -1.0f, 0.01f).Normalize());
	spotLight->SetInnerFallofAngle(0.01f);
	spotLight->SetOuterFallofAngle(0.6f);
	success = spotLight->LoadProjectiveTexture("Fan.dds");
	assert(success == true);
	myLights.Insert(1, spotLight);
	myBlueLight = spotLight;
	Engine::GetInstance()->GetScene().AddLight(spotLight);


// 	// Omni
// 	Light* point = new Light(Light::POINT_LIGHT_TYPE);
// 	point->SetPosition(Vector3f(0.0f, 60.0f, 0.0f));
// 	point->SetLightColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
// 	point->SetMaxDistance(1000.0f);
// 	myLights.Insert(2, point);
// 	Engine::GetInstance()->GetScene().AddLight(spotLight);

	
}
