#include "Renderer.h"
#include "Scene.h"
#include "Engine.h"
#include <sstream>

float locDownsampleSize = 800.0f;

Renderer::Renderer(void)
{
	mySphereMesh					= NULL;
	myConeMesh						= NULL;
	myScene							= NULL;
	myMotionBlurFlag				= true;
	myHDRFlag						= false;
	myInstancingFlag				= false;
	myTogglePointOrSpotLight		= true;
	myToggleQuadOrFullScreen		= false;
	myToggleAdvancedCulling			= true;
}

Renderer::~Renderer(void)
{
}

void Renderer::SetScene( Scene* aScene )
{
	myScene = aScene;
}

void Renderer::Render()
{

	ID3D10ShaderResourceView *const pSRV[8] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
	Engine::GetInstance()->GetDevice()->PSSetShaderResources(0, 8, pSRV);

	myFullscreenHelper.ResetPostProcessingTextures();
	RenderDepthNormalAlbedo();
	
	const Vector2f ScreenSize(Engine::GetInstance()->GetScreeenWidth(), Engine::GetInstance()->GetScreenHeight());
 	const Vector2f Offset(0.0f, 0.0f);

		//apply ambient
	Engine::GetInstance()->GetEffectInput().SetPrimaryPostProcesingTexture(myAlbedoMap.GetShaderResourceView());
	Engine::GetInstance()->GetEffectInput().SetSecondaryPostProcesingTexture(myAmbientMap.GetShaderResourceView());
	myFullscreenHelper.Process(myAlbedoMap.GetShaderResourceView(), myFinalScene.GetRenderTargetView(), "RenderAmbientPass");

	Engine::GetInstance()->GetEffectInput().SetPrimaryPostProcesingTexture(myDepthMap.GetShaderResourceView());
	Engine::GetInstance()->GetEffectInput().SetSecondaryPostProcesingTexture(myNormalMap.GetShaderResourceView());
	Engine::GetInstance()->GetEffectInput().SetThirdPostProcesingTexture(myAlbedoMap.GetShaderResourceView());
	Engine::GetInstance()->GetEffectInput().GetEffectPool()->AsEffect()->GetVariableByName("LinearDepth")->AsShaderResource()->SetResource(myLinearDepthMap.GetShaderResourceView());

	//RenderDeferredDirectionalLights(&myFinalScene, myDepthMap, ScreenSize, Offset);
	//RenderDeferredPointLights(&myFinalScene, ScreenSize, Offset);
	RenderDeferredSpotLights(&myFinalScene, myDepthMap, ScreenSize, Offset);

	Engine::GetInstance()->ResetScissorRect();
	myFullscreenHelper.DisableScissorRect();

	myFullscreenHelper.Process(myFinalScene.GetShaderResourceView(), Engine::GetInstance()->GetBackBuffer(), EffectTechniques::QUAD, ScreenSize, Vector2f(0.0f, 0.0f)); 
}

void Renderer::RenderToCube( Vector3f aPosition, int aCubeMapIndex )
{
	myToggleAdvancedCulling = false;

	float clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	for(int sideIndex = 0; sideIndex < 6; sideIndex++)
	{	
		Engine::GetInstance()->GetDevice()->ClearRenderTargetView(myAmbientProbes[aCubeMapIndex].myReflectionCubeMapSides[sideIndex]->GetRenderTargetView(), clearColor);	
	}

	Engine::GetInstance()->GetEffectInput().SetAmbientProbePosition(aPosition);
	Engine::GetInstance()->GetEffectInput().SetEnviromentMap(myAmbientProbes[aCubeMapIndex].myShaderResourceView);
	Engine::GetInstance()->GetEffectInput().SetReflectionMap(myAmbientProbes[aCubeMapIndex].myReflectionShaderResourceView);
	
	//Render each of the ambient probe's sides to a cross
	RenderCross(AmbientProbe::Right, aCubeMapIndex, aPosition);
	RenderCross(AmbientProbe::Left, aCubeMapIndex, aPosition);
	RenderCross(AmbientProbe::Up, aCubeMapIndex, aPosition);
	RenderCross(AmbientProbe::Bottom, aCubeMapIndex, aPosition);
	RenderCross(AmbientProbe::Front, aCubeMapIndex, aPosition);
	RenderCross(AmbientProbe::Back, aCubeMapIndex, aPosition);

	//Copy the finalized cubemap side textures to the actual cubemap
	myFullscreenHelper.Process(myTemporaryCubeSides[0].GetShaderResourceView(), myAmbientProbes[aCubeMapIndex].myCubeMapSides[0]->GetRenderTargetView(), "Render_Quad", Vector2f(20.0f, 20.0f), Vector2f(0.0f, 0.0f));
	myFullscreenHelper.Process(myTemporaryCubeSides[1].GetShaderResourceView(), myAmbientProbes[aCubeMapIndex].myCubeMapSides[1]->GetRenderTargetView(), "Render_Quad", Vector2f(20.0f, 20.0f), Vector2f(0.0f, 0.0f));
	myFullscreenHelper.Process(myTemporaryCubeSides[2].GetShaderResourceView(), myAmbientProbes[aCubeMapIndex].myCubeMapSides[2]->GetRenderTargetView(), "Render_Quad", Vector2f(20.0f, 20.0f), Vector2f(0.0f, 0.0f));
	myFullscreenHelper.Process(myTemporaryCubeSides[3].GetShaderResourceView(), myAmbientProbes[aCubeMapIndex].myCubeMapSides[3]->GetRenderTargetView(), "Render_Quad", Vector2f(20.0f, 20.0f), Vector2f(0.0f, 0.0f));
	myFullscreenHelper.Process(myTemporaryCubeSides[4].GetShaderResourceView(), myAmbientProbes[aCubeMapIndex].myCubeMapSides[4]->GetRenderTargetView(), "Render_Quad", Vector2f(20.0f, 20.0f), Vector2f(0.0f, 0.0f));
	myFullscreenHelper.Process(myTemporaryCubeSides[5].GetShaderResourceView(), myAmbientProbes[aCubeMapIndex].myCubeMapSides[5]->GetRenderTargetView(), "Render_Quad", Vector2f(20.0f, 20.0f), Vector2f(0.0f, 0.0f));
	
	//Copy the finalized reflection cubemap side textures to the actual reflection cubemap
	myFullscreenHelper.Process(myTemporaryReflectionCubeSides[0].GetShaderResourceView(), myAmbientProbes[aCubeMapIndex].myReflectionCubeMapSides[0]->GetRenderTargetView(), "Render_Quad", Vector2f(20.0f, 20.0f), Vector2f(0.0f, 0.0f));
	myFullscreenHelper.Process(myTemporaryReflectionCubeSides[1].GetShaderResourceView(), myAmbientProbes[aCubeMapIndex].myReflectionCubeMapSides[1]->GetRenderTargetView(), "Render_Quad", Vector2f(20.0f, 20.0f), Vector2f(0.0f, 0.0f));
	myFullscreenHelper.Process(myTemporaryReflectionCubeSides[2].GetShaderResourceView(), myAmbientProbes[aCubeMapIndex].myReflectionCubeMapSides[2]->GetRenderTargetView(), "Render_Quad", Vector2f(20.0f, 20.0f), Vector2f(0.0f, 0.0f));
	myFullscreenHelper.Process(myTemporaryReflectionCubeSides[3].GetShaderResourceView(), myAmbientProbes[aCubeMapIndex].myReflectionCubeMapSides[3]->GetRenderTargetView(), "Render_Quad", Vector2f(20.0f, 20.0f), Vector2f(0.0f, 0.0f));
	myFullscreenHelper.Process(myTemporaryReflectionCubeSides[4].GetShaderResourceView(), myAmbientProbes[aCubeMapIndex].myReflectionCubeMapSides[4]->GetRenderTargetView(), "Render_Quad", Vector2f(20.0f, 20.0f), Vector2f(0.0f, 0.0f));
	myFullscreenHelper.Process(myTemporaryReflectionCubeSides[5].GetShaderResourceView(), myAmbientProbes[aCubeMapIndex].myReflectionCubeMapSides[5]->GetRenderTargetView(), "Render_Quad", Vector2f(20.0f, 20.0f), Vector2f(0.0f, 0.0f));

	//WriteTextureToDisk(*myAmbientProbes[aCubeMapIndex].myCubeMapSides[aCubeMapIndex], "AmbientProbeAmbientCubeMap");
	//WriteTextureToDisk(*myAmbientProbes[aCubeMapIndex].myReflectionCubeMapSides[aCubeMapIndex], "AmbientProbeReflectionCubeMap");

	Engine::GetInstance()->GetEffectInput().SetEnviromentMap(myAmbientProbes[aCubeMapIndex].myShaderResourceView);
	Engine::GetInstance()->GetEffectInput().SetReflectionMap(myAmbientProbes[aCubeMapIndex].myReflectionShaderResourceView);

	myToggleAdvancedCulling = true;
}

void Renderer::ResetStuff()
{
	for(int probeIndex = 0; probeIndex < Application::myNumberOfAmbientProbes; probeIndex++)
	{
		float clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
		for(int sideIndex = 0; sideIndex < 6; sideIndex++)
		{
			Engine::GetInstance()->GetDevice()->ClearRenderTargetView(myAmbientProbes[probeIndex].myCubeMapSides[sideIndex]->GetRenderTargetView(), clearColor);	
			Engine::GetInstance()->GetDevice()->ClearRenderTargetView(myAmbientProbes[probeIndex].myReflectionCubeMapSides[sideIndex]->GetRenderTargetView(), clearColor);	
		}
	}
}

void Renderer::RenderCross(AmbientProbe::CubeMapSideTypes aSideToRenderCrossAround, int anAmbientProbeIndex, Vector3f aPosition)
{
	//right, left, up, down, forward, backward
	const float piValue = 3.1415926f;
	const float halfPi = piValue / 2.0f;

	Matrix44f cameraOrientation[6];

	if(aSideToRenderCrossAround == AmbientProbe::Right)
	{
		cameraOrientation[AmbientProbe::Right]	= Matrix44f::CreateRotateAroundY(piValue);
		cameraOrientation[AmbientProbe::Left]	= Matrix44f::CreateRotateAroundY(0.0f);
		cameraOrientation[AmbientProbe::Up]		= Matrix44f::CreateRotateAroundX(halfPi) * Matrix44f::CreateRotateAroundY(-halfPi);
		cameraOrientation[AmbientProbe::Bottom] = Matrix44f::CreateRotateAroundX(-halfPi) * Matrix44f::CreateRotateAroundY(-halfPi);
		cameraOrientation[AmbientProbe::Front]	= Matrix44f::CreateRotateAroundY(-halfPi);
		cameraOrientation[AmbientProbe::Back]	= Matrix44f::CreateRotateAroundY(halfPi);
	}
	else if(aSideToRenderCrossAround == AmbientProbe::Left)
	{
		cameraOrientation[AmbientProbe::Right]	= Matrix44f::CreateRotateAroundY(0.0f);
		cameraOrientation[AmbientProbe::Left]	= Matrix44f::CreateRotateAroundY(piValue);
		cameraOrientation[AmbientProbe::Up]		= Matrix44f::CreateRotateAroundX(halfPi) * Matrix44f::CreateRotateAroundY(halfPi);
		cameraOrientation[AmbientProbe::Bottom] = Matrix44f::CreateRotateAroundX(-halfPi) * Matrix44f::CreateRotateAroundY(halfPi);
		cameraOrientation[AmbientProbe::Front]	= Matrix44f::CreateRotateAroundY(halfPi);
		cameraOrientation[AmbientProbe::Back]	= Matrix44f::CreateRotateAroundY(-halfPi);
	}
	else if(aSideToRenderCrossAround == AmbientProbe::Up)
	{
		cameraOrientation[AmbientProbe::Right]	= Matrix44f::CreateRotateAroundY(-halfPi) * Matrix44f::CreateRotateAroundX(halfPi);
		cameraOrientation[AmbientProbe::Left]	= Matrix44f::CreateRotateAroundY(halfPi) * Matrix44f::CreateRotateAroundX(halfPi);
		cameraOrientation[AmbientProbe::Up]		= Matrix44f::CreateRotateAroundY(piValue) * Matrix44f::CreateRotateAroundZ(piValue);
		cameraOrientation[AmbientProbe::Bottom] = Matrix44f::CreateRotateAroundX(0.0f);
		cameraOrientation[AmbientProbe::Front]	= Matrix44f::CreateRotateAroundX(halfPi);
		cameraOrientation[AmbientProbe::Back]	= Matrix44f::CreateRotateAroundX(-halfPi);
	}
	else if(aSideToRenderCrossAround == AmbientProbe::Bottom)
	{
		cameraOrientation[AmbientProbe::Right]	= Matrix44f::CreateRotateAroundY(-halfPi) * Matrix44f::CreateRotateAroundX(-halfPi);
		cameraOrientation[AmbientProbe::Left]	= Matrix44f::CreateRotateAroundY(halfPi) * Matrix44f::CreateRotateAroundX(-halfPi);
		cameraOrientation[AmbientProbe::Up]		= Matrix44f::CreateRotateAroundY(0.0f);
		cameraOrientation[AmbientProbe::Bottom] = Matrix44f::CreateRotateAroundY(piValue) * Matrix44f::CreateRotateAroundZ(-piValue);
		cameraOrientation[AmbientProbe::Front]	= Matrix44f::CreateRotateAroundX(-halfPi);
		cameraOrientation[AmbientProbe::Back]	= Matrix44f::CreateRotateAroundX(halfPi);
	}
	else if(aSideToRenderCrossAround == AmbientProbe::Front)
	{
		cameraOrientation[AmbientProbe::Right]	= Matrix44f::CreateRotateAroundY(-halfPi);
		cameraOrientation[AmbientProbe::Left]	= Matrix44f::CreateRotateAroundY(halfPi);
		cameraOrientation[AmbientProbe::Up]		= Matrix44f::CreateRotateAroundX(halfPi);
		cameraOrientation[AmbientProbe::Bottom] = Matrix44f::CreateRotateAroundX(-halfPi);
		cameraOrientation[AmbientProbe::Front]	= Matrix44f::CreateRotateAroundY(0.0f);
		cameraOrientation[AmbientProbe::Back]	= Matrix44f::CreateRotateAroundY(piValue);
	}
	else if(aSideToRenderCrossAround == AmbientProbe::Back)
	{
		cameraOrientation[AmbientProbe::Right]	= Matrix44f::CreateRotateAroundY(halfPi);
		cameraOrientation[AmbientProbe::Left]	= Matrix44f::CreateRotateAroundY(-halfPi);
		cameraOrientation[AmbientProbe::Up]		= Matrix44f::CreateRotateAroundX(halfPi) * Matrix44f::CreateRotateAroundY(-piValue);
		cameraOrientation[AmbientProbe::Bottom] = Matrix44f::CreateRotateAroundX(-halfPi) * Matrix44f::CreateRotateAroundY(-piValue);
		cameraOrientation[AmbientProbe::Front]	= Matrix44f::CreateRotateAroundY(piValue);
		cameraOrientation[AmbientProbe::Back]	= Matrix44f::CreateRotateAroundY(0.0f);
	}

	//Render the cubemaps sides
	Matrix44f oldCameraOrientation = myScene->GetCamera().GetOrientation();
	for(int index = 0; index < 6; index++)
	{
		cameraOrientation[index].SetPosition(aPosition);
		myScene->GetCamera().SetOrientationNotRetarded(cameraOrientation[index]);

		float clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
		Engine::GetInstance()->GetDevice()->ClearRenderTargetView(myAmbientProbes[anAmbientProbeIndex].myCubeMapSidesDepth[index]->GetRenderTargetView(), clearColor);
		Engine::GetInstance()->GetDevice()->ClearRenderTargetView(myAmbientProbes[anAmbientProbeIndex].myTemporaryNormalMap->GetRenderTargetView(), clearColor);
		Engine::GetInstance()->GetDevice()->ClearRenderTargetView(myAmbientProbes[anAmbientProbeIndex].myCubeMapSidesColor[index]->GetRenderTargetView(), clearColor);
		Engine::GetInstance()->GetDevice()->ClearRenderTargetView(myAmbientMap.GetRenderTargetView(), clearColor);

		ID3D10RenderTargetView* renderTargetViews[5] = {myAmbientProbes[anAmbientProbeIndex].myCubeMapSidesDepth[index]->GetRenderTargetView(), myAmbientProbes[anAmbientProbeIndex].myTemporaryNormalMap->GetRenderTargetView(), myAmbientProbes[anAmbientProbeIndex].myCubeMapSidesColor[index]->GetRenderTargetView(), myAmbientMap.GetRenderTargetView(), myLinearDepthMap.GetRenderTargetView()};

		Engine::GetInstance()->GetDevice()->OMSetRenderTargets(5, renderTargetViews, NULL);
		myScene->Render(EffectTechniques::DEPTH_NORMAL_ALBEDO);

		//apply ambient
		Engine::GetInstance()->GetEffectInput().SetPrimaryPostProcesingTexture(myAmbientProbes[anAmbientProbeIndex].myCubeMapSidesColor[index]->GetShaderResourceView());
		Engine::GetInstance()->GetEffectInput().SetSecondaryPostProcesingTexture(myAmbientMap.GetShaderResourceView());
		myFullscreenHelper.Process(myAmbientProbes[anAmbientProbeIndex].myCubeMapSidesColor[index]->GetShaderResourceView(), myAmbientProbes[anAmbientProbeIndex].myTemporaryColorMap->GetRenderTargetView(), "RenderAmbientPass");
		myFullscreenHelper.ResetPostProcessingTextures();
	
		//copy
		myFullscreenHelper.Process(myAmbientProbes[anAmbientProbeIndex].myTemporaryColorMap->GetShaderResourceView(), myAmbientProbes[anAmbientProbeIndex].myCubeMapSidesColor[index]->GetRenderTargetView(), "Render_Quad", Vector2f(800.0f, 800.0f), Vector2f(0.0f, 0.0f));

		//apply lighting
		Engine::GetInstance()->GetEffectInput().SetPrimaryPostProcesingTexture(myAmbientProbes[anAmbientProbeIndex].myCubeMapSidesDepth[index]->GetShaderResourceView());
		Engine::GetInstance()->GetEffectInput().SetSecondaryPostProcesingTexture(myAmbientProbes[anAmbientProbeIndex].myTemporaryNormalMap->GetShaderResourceView());
		RenderDeferredSpotLights(myAmbientProbes[anAmbientProbeIndex].myCubeMapSidesColor[index], *myAmbientProbes[anAmbientProbeIndex].myCubeMapSidesDepth[index], Vector2f(800.0f, 800.0f), Vector2f(0.0f, 0.0f), false);
		myFullscreenHelper.ResetPostProcessingTextures();
	}
	myScene->GetCamera().SetOrientation(oldCameraOrientation);

	float clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView(myAmbientProbes[anAmbientProbeIndex].myCrossMapColor[aSideToRenderCrossAround]->GetRenderTargetView(), clearColor);
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView(myAmbientProbes[anAmbientProbeIndex].myCrossMapDepth[aSideToRenderCrossAround]->GetRenderTargetView(), clearColor);

	//Render the 5 color textures to a cross
	myFullscreenHelper.Process(myAmbientProbes[anAmbientProbeIndex].myCubeMapSidesColor[0]->GetShaderResourceView(), myAmbientProbes[anAmbientProbeIndex].myCrossMapColor[aSideToRenderCrossAround]->GetRenderTargetView(), EffectTechniques::QUAD, Vector2f(20.0f, 20.0f), Vector2f(40.0f, 20.0f));
	myFullscreenHelper.Process(myAmbientProbes[anAmbientProbeIndex].myCubeMapSidesColor[1]->GetShaderResourceView(), myAmbientProbes[anAmbientProbeIndex].myCrossMapColor[aSideToRenderCrossAround]->GetRenderTargetView(), EffectTechniques::QUAD, Vector2f(20.0f, 20.0f), Vector2f(0.0f, 20.0f));
	myFullscreenHelper.Process(myAmbientProbes[anAmbientProbeIndex].myCubeMapSidesColor[2]->GetShaderResourceView(), myAmbientProbes[anAmbientProbeIndex].myCrossMapColor[aSideToRenderCrossAround]->GetRenderTargetView(), EffectTechniques::QUAD, Vector2f(20.0f, 20.0f), Vector2f(20.0f, 0.0f));
	myFullscreenHelper.Process(myAmbientProbes[anAmbientProbeIndex].myCubeMapSidesColor[3]->GetShaderResourceView(), myAmbientProbes[anAmbientProbeIndex].myCrossMapColor[aSideToRenderCrossAround]->GetRenderTargetView(), EffectTechniques::QUAD, Vector2f(20.0f, 20.0f), Vector2f(20.0f, 40.0f));
	myFullscreenHelper.Process(myAmbientProbes[anAmbientProbeIndex].myCubeMapSidesColor[4]->GetShaderResourceView(), myAmbientProbes[anAmbientProbeIndex].myCrossMapColor[aSideToRenderCrossAround]->GetRenderTargetView(), EffectTechniques::QUAD, Vector2f(20.0f, 20.0f), Vector2f(20.0f, 20.0f));

	//Render the 5 depth textures to a cross
	myFullscreenHelper.Process(myAmbientProbes[anAmbientProbeIndex].myCubeMapSidesDepth[0]->GetShaderResourceView(), myAmbientProbes[anAmbientProbeIndex].myCrossMapDepth[aSideToRenderCrossAround]->GetRenderTargetView(), EffectTechniques::QUAD, Vector2f(20.0f, 20.0f), Vector2f(40.0f, 20.0f));
	myFullscreenHelper.Process(myAmbientProbes[anAmbientProbeIndex].myCubeMapSidesDepth[1]->GetShaderResourceView(), myAmbientProbes[anAmbientProbeIndex].myCrossMapDepth[aSideToRenderCrossAround]->GetRenderTargetView(), EffectTechniques::QUAD, Vector2f(20.0f, 20.0f), Vector2f(0.0f, 20.0f));
	myFullscreenHelper.Process(myAmbientProbes[anAmbientProbeIndex].myCubeMapSidesDepth[2]->GetShaderResourceView(), myAmbientProbes[anAmbientProbeIndex].myCrossMapDepth[aSideToRenderCrossAround]->GetRenderTargetView(), EffectTechniques::QUAD, Vector2f(20.0f, 20.0f), Vector2f(20.0f, 0.0f));
	myFullscreenHelper.Process(myAmbientProbes[anAmbientProbeIndex].myCubeMapSidesDepth[3]->GetShaderResourceView(), myAmbientProbes[anAmbientProbeIndex].myCrossMapDepth[aSideToRenderCrossAround]->GetRenderTargetView(), EffectTechniques::QUAD, Vector2f(20.0f, 20.0f), Vector2f(20.0f, 40.0f));
	myFullscreenHelper.Process(myAmbientProbes[anAmbientProbeIndex].myCubeMapSidesDepth[4]->GetShaderResourceView(), myAmbientProbes[anAmbientProbeIndex].myCrossMapDepth[aSideToRenderCrossAround]->GetRenderTargetView(), EffectTechniques::QUAD, Vector2f(20.0f, 20.0f), Vector2f(20.0f, 20.0f));
	
	//WriteTextureToDisk(*myAmbientProbes[anAmbientProbeIndex].myCrossMapColor[aSideToRenderCrossAround], "CROSS");
	//Blur the color cross
	Engine::GetInstance()->GetEffectInput().SetPrimaryPostProcesingTexture(myAmbientProbes[anAmbientProbeIndex].myCrossMapColor[aSideToRenderCrossAround]->GetShaderResourceView());
	Engine::GetInstance()->GetEffectInput().SetSecondaryPostProcesingTexture(myAmbientProbes[anAmbientProbeIndex].myCrossMapDepth[aSideToRenderCrossAround]->GetShaderResourceView());
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView(myAmbientProbes[anAmbientProbeIndex].myCrossMapCopy->GetRenderTargetView(), clearColor);
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView(myAmbientProbes[anAmbientProbeIndex].myCrossMapCopy2->GetRenderTargetView(), clearColor);
	myFullscreenHelper.Process(myAmbientProbes[anAmbientProbeIndex].myCrossMapColor[aSideToRenderCrossAround]->GetShaderResourceView(), myAmbientProbes[anAmbientProbeIndex].myCrossMapCopy->GetRenderTargetView(), "JaffeBlurHorizontal", Vector2f(60.0f, 60.0f));
	myFullscreenHelper.Process(myAmbientProbes[anAmbientProbeIndex].myCrossMapCopy->GetShaderResourceView(), myAmbientProbes[anAmbientProbeIndex].myCrossMapCopy2->GetRenderTargetView(), "Render_Quad", Vector2f(60.0f, 60.0f));
	myFullscreenHelper.ResetPostProcessingTextures();
	Engine::GetInstance()->GetEffectInput().SetPrimaryPostProcesingTexture(myAmbientProbes[anAmbientProbeIndex].myCrossMapCopy2->GetShaderResourceView());
	Engine::GetInstance()->GetEffectInput().SetSecondaryPostProcesingTexture(myAmbientProbes[anAmbientProbeIndex].myCrossMapDepth[aSideToRenderCrossAround]->GetShaderResourceView());
	myFullscreenHelper.Process(myAmbientProbes[anAmbientProbeIndex].myCrossMapCopy2->GetShaderResourceView(), myAmbientProbes[anAmbientProbeIndex].myCrossMapCopy->GetRenderTargetView(), "JaffeBlurVertical", Vector2f(60.0f, 60.0f));
	myFullscreenHelper.ResetPostProcessingTextures();

	//Extract finalized blurred side texture from cross to temporary cubemap side texture
	myFullscreenHelper.Process(myAmbientProbes[anAmbientProbeIndex].myCrossMapCopy->GetShaderResourceView(), myTemporaryCubeSides[aSideToRenderCrossAround].GetRenderTargetView(), "JaffeCopyFromCrossCenter", Vector2f(20.0f, 20.0f), Vector2f(0.0f, 0.0f));
	myFullscreenHelper.ResetPostProcessingTextures();

	//Blur the color cross with special reflection blur
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView(myAmbientProbes[anAmbientProbeIndex].myCrossMapCopy->GetRenderTargetView(), clearColor);
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView(myAmbientProbes[anAmbientProbeIndex].myCrossMapCopy2->GetRenderTargetView(), clearColor);
	//myFullscreenHelper.Process(myAmbientProbes[anAmbientProbeIndex].myCrossMapColor[aSideToRenderCrossAround]->GetShaderResourceView(), myAmbientProbes[anAmbientProbeIndex].myCrossMapCopy2->GetRenderTargetView(), "JaffeScale", Vector2f(60.0f, 60.0f));
	Engine::GetInstance()->GetEffectInput().SetPrimaryPostProcesingTexture(myAmbientProbes[anAmbientProbeIndex].myCrossMapColor[aSideToRenderCrossAround]->GetShaderResourceView());
	Engine::GetInstance()->GetEffectInput().SetSecondaryPostProcesingTexture(myAmbientProbes[anAmbientProbeIndex].myCrossMapDepth[aSideToRenderCrossAround]->GetShaderResourceView());
	myFullscreenHelper.Process(myAmbientProbes[anAmbientProbeIndex].myCrossMapColor[aSideToRenderCrossAround]->GetShaderResourceView(), myAmbientProbes[anAmbientProbeIndex].myCrossMapCopy->GetRenderTargetView(), "JaffeSpecialBlurHorizontal", Vector2f(60.0f, 60.0f));
	myFullscreenHelper.ResetPostProcessingTextures();
	myFullscreenHelper.Process(myAmbientProbes[anAmbientProbeIndex].myCrossMapCopy->GetShaderResourceView(), myAmbientProbes[anAmbientProbeIndex].myCrossMapCopy2->GetRenderTargetView(), "Render_Quad", Vector2f(60.0f, 60.0f));
	myFullscreenHelper.ResetPostProcessingTextures();
	Engine::GetInstance()->GetEffectInput().SetPrimaryPostProcesingTexture(myAmbientProbes[anAmbientProbeIndex].myCrossMapCopy2->GetShaderResourceView());
	Engine::GetInstance()->GetEffectInput().SetSecondaryPostProcesingTexture(myAmbientProbes[anAmbientProbeIndex].myCrossMapDepth[aSideToRenderCrossAround]->GetShaderResourceView());
	myFullscreenHelper.Process(myAmbientProbes[anAmbientProbeIndex].myCrossMapCopy2->GetShaderResourceView(), myAmbientProbes[anAmbientProbeIndex].myCrossMapCopy->GetRenderTargetView(), "JaffeSpecialBlurVertical", Vector2f(60.0f, 60.0f));
	myFullscreenHelper.ResetPostProcessingTextures();

	//Extract texture from blurred cross to temporary cubemap reflection side texture
	myFullscreenHelper.Process(myAmbientProbes[anAmbientProbeIndex].myCrossMapCopy->GetShaderResourceView(), myTemporaryReflectionCubeSides[aSideToRenderCrossAround].GetRenderTargetView(), "JaffeCopyFromCrossCenter", Vector2f(20.0f, 20.0f), Vector2f(0.0f, 0.0f));
	myFullscreenHelper.ResetPostProcessingTextures();
	
	//const std::string sides[6] = {"Right", "Left", "Up", "Bottom", "Front", "Back"};
	//WriteTextureToDisk(*myAmbientProbes[anAmbientProbeIndex].myCrossMapColor[aSideToRenderCrossAround], "CrossColor" + sides[aSideToRenderCrossAround]);
	//WriteTextureToDisk(*myAmbientProbes[anAmbientProbeIndex].myCrossMapDepth[aSideToRenderCrossAround], "CrossDepth" + sides[aSideToRenderCrossAround]);
	//WriteTextureToDisk(*myAmbientProbes[anAmbientProbeIndex].myCrossMapCopy, "BlurredCross" + sides[aSideToRenderCrossAround]);
	//WriteTextureToDisk(myTemporaryCubeSides[aSideToRenderCrossAround], "CubeMapSideFinal" + sides[aSideToRenderCrossAround]);
	//WriteTextureToDisk(myTemporaryReflectionCubeSides[aSideToRenderCrossAround], "CubeMapReflectionSideFinal" + sides[aSideToRenderCrossAround]);
}

void Renderer::DownsampleCross(ID3D10ShaderResourceView* aCrossToDownsample)
{
	myFullscreenHelper.Process(aCrossToDownsample, myDownSampledCross400.GetRenderTargetView(), "JaffeDownSample", Vector2f(400.0f, 400.0f));
	myFullscreenHelper.Process(myDownSampledCross400.GetShaderResourceView(), myDownSampledCross200.GetRenderTargetView(), "JaffeDownSample", Vector2f(200.0f, 200.0f));
	myFullscreenHelper.Process(myDownSampledCross200.GetShaderResourceView(), myDownSampledCross100.GetRenderTargetView(), "JaffeDownSample", Vector2f(100.0f, 100.0f));
	myFullscreenHelper.Process(myDownSampledCross100.GetShaderResourceView(), myDownSampledCross60.GetRenderTargetView(), "JaffeDownSample", Vector2f(60.0f, 60.0f));
}

void Renderer::RenderDepthOfField(RenderTarget& aRenderTarget)
{
	float clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView(myDownsampledAlbedo.GetRenderTargetView(), clearColor);
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView(myDownsampledAlbedo2.GetRenderTargetView(), clearColor);
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView(myAlbedoMap2.GetRenderTargetView(), clearColor);

	//Compute depth data myAlbedoMap myFinalScene
	Engine::GetInstance()->GetEffectInput().SetPrimaryPostProcesingTexture(myFinalScene.GetShaderResourceView());
	Engine::GetInstance()->GetEffectInput().SetSecondaryPostProcesingTexture(myLinearDepthMap.GetShaderResourceView());
	myFullscreenHelper.Process(myFinalScene.GetShaderResourceView(), myAlbedoMap2.GetRenderTargetView(), "ComputeBlurDepth", Vector2f(800.0f, 800.0f));
	//WriteTextureToDisk(myAlbedoMap2, "TEMP0");
	//WriteTextureToDisk(myLinearDepthMap, "LinearDepth");

	//Downsample
	myFullscreenHelper.Process(myFinalScene.GetShaderResourceView(), myDownsampledAlbedo.GetRenderTargetView(), "Render_Quad_Linear", Vector2f(locDownsampleSize, locDownsampleSize));
	myFullscreenHelper.ResetPostProcessingTextures();
	//WriteTextureToDisk(myDownsampledAlbedo, "TEMP1");
	
	//Blur horizontally
	myFullscreenHelper.Process(myDownsampledAlbedo.GetShaderResourceView(), myDownsampledAlbedo2.GetRenderTargetView(), "JaffeSpecialBlurHorizontal2", Vector2f(locDownsampleSize, locDownsampleSize));
	//WriteTextureToDisk(myDownsampledAlbedo2, "TEMP2");
	myFullscreenHelper.ResetPostProcessingTextures();

	//Blur vertically
	myFullscreenHelper.Process(myDownsampledAlbedo2.GetShaderResourceView(), myDownsampledAlbedo.GetRenderTargetView(), "JaffeSpecialBlurVertical2", Vector2f(locDownsampleSize, locDownsampleSize));
	myFullscreenHelper.ResetPostProcessingTextures();
	//WriteTextureToDisk(myDownsampledAlbedo, "TEMP3");

	//Perform depth of field
	Engine::GetInstance()->GetEffectInput().SetPrimaryPostProcesingTexture(myAlbedoMap2.GetShaderResourceView());
	Engine::GetInstance()->GetEffectInput().SetSecondaryPostProcesingTexture(myDownsampledAlbedo.GetShaderResourceView());
	myFullscreenHelper.Process(myAlbedoMap2.GetShaderResourceView(), myFinalScene.GetRenderTargetView(), "RenderDepthOfField", Vector2f(800.0f, 800.0f));
	myFullscreenHelper.ResetPostProcessingTextures();
	//WriteTextureToDisk(myFinalScene, "TEMP4");
}

bool Renderer::Init()
{
	if ( myRenderTargetTexture.Init( DXGI_FORMAT_R16G16B16A16_UNORM ) == false)
	{
		return false;
	}

	if ( myFullscreenHelper.Init() == false )
	{
		return false;
	}
		
	if ( myShadowBuffer.Init( DXGI_FORMAT_R32G32B32A32_FLOAT ) == false)
	{
		return false;
	}
	if ( InitShadowDepthStencil() == false )
	{
		return false;
	}
	for(int index = 0; index < Application::myNumberOfAmbientProbes; index++)
	{
		myAmbientProbes[index].Init();
	}
	for(int index = 0; index < 6; index++)
	{
		if(myTemporaryCubeSides[index].Init(DXGI_FORMAT_R32G32B32A32_FLOAT, 20) == false)
		{
			return false;
		}
		if(myTemporaryReflectionCubeSides[index].Init(DXGI_FORMAT_R32G32B32A32_FLOAT, 20) == false)
		{
			return false;
		}
	}
	if(myTemporaryReflectionTexture.Init(DXGI_FORMAT_R32G32B32A32_FLOAT, 20) == false)
	{
		return false;
	}
	if(myDownsampledAlbedo.Init(DXGI_FORMAT_R32G32B32A32_FLOAT, (int)locDownsampleSize) == false)
	{
		return false;
	}
	if(myDownsampledAlbedo2.Init(DXGI_FORMAT_R32G32B32A32_FLOAT, (int)locDownsampleSize) == false)
	{
		return false;
	}
	if ( myDepthMap.Init( DXGI_FORMAT_R32G32B32A32_FLOAT ) == false )
	{
		return false;
	}
	if ( myLinearDepthMap.Init( DXGI_FORMAT_R32G32B32A32_FLOAT ) == false )
	{
		return false;
	}
	if ( myFinalScene.Init( DXGI_FORMAT_R32G32B32A32_FLOAT ) == false )
	{
		return false;
	}
	if ( myNormalMap.Init( DXGI_FORMAT_R32G32B32A32_FLOAT ) == false )
	{
		return false;
	}
	if ( myAlbedoMap.Init( DXGI_FORMAT_R32G32B32A32_FLOAT ) == false )
	{
		return false;
	}
	if ( myAlbedoMap2.Init( DXGI_FORMAT_R32G32B32A32_FLOAT ) == false )
	{
		return false;
	}
	if(myAmbientMap.Init(DXGI_FORMAT_R32G32B32A32_FLOAT) == false)
	{
		return false;
	}
	if(myShadowMapCopy.Init(DXGI_FORMAT_R32G32B32A32_FLOAT) == false)
	{
		return false;
	}
	if(myShadowMapCopy2.Init(DXGI_FORMAT_R32G32B32A32_FLOAT) == false)
	{
		return false;
	}

	myRenderProcessTarget.Init();

	return true;
}

void Renderer::ClearRenderTargets( float aClearColor[4] )
{	
	float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; //red,green,blue,alpha
	float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	Engine::GetInstance()->GetDevice()->ClearRenderTargetView( myRenderTargetTexture.GetRenderTargetView(), aClearColor );
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView( myShadowBuffer.GetRenderTargetView(), black );
	Engine::GetInstance()->GetDevice()->ClearDepthStencilView( myShadowDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0 );
	
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView( myDepthMap.GetRenderTargetView(), black );
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView( myLinearDepthMap.GetRenderTargetView(), black );
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView( myFinalScene.GetRenderTargetView(), black );
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView( myNormalMap.GetRenderTargetView(), black );
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView( myAlbedoMap.GetRenderTargetView(), black );
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView(myAmbientMap.GetRenderTargetView(), black);
}

void Renderer::Cleanup()
{
	myRenderTargetTexture.Release();
	myDownsampledTexture2.Release();
	myDownsampledTexture4.Release();
	myDownsampledTexture8.Release();
	myDownsampledTexture16.Release();
	myDownsampledTexture32.Release();
	myDownsampledTexture64.Release();
	myDownsampledTexture128.Release();
	myDownsampledTexture256.Release();
	myDownsampledOnePixel.Release();
	//for(int index = 0; index < Application::myNumberOfAmbientProbes; index++)
	//{
	//	myAmbientProbes[index].Release();
	//}
	myShadowBuffer.Release();

	myDepthMap.Release();
	myFinalScene.Release();
	myNormalMap.Release();
	myAlbedoMap.Release();
	myAmbientOcclusionMap.Release();
}

bool Renderer::SetDownsampleVariables( Vector2f aTargetDimensions )
{
// 	Vector2f PixelCoordsDownFilter[16];
// 
// 	PixelCoordsDownFilter[0].Set( 1.5,  -1.5 );
// 	PixelCoordsDownFilter[1].Set(  1.5,  -0.5 );
// 	PixelCoordsDownFilter[2].Set(  1.5,   0.5 );
// 	PixelCoordsDownFilter[3].Set(  1.5,   1.5 );
// 
// 	PixelCoordsDownFilter[4].Set(  0.5,  -1.5 );
// 	PixelCoordsDownFilter[5].Set(  0.5,  -0.5 );
// 	PixelCoordsDownFilter[6].Set(  0.5,   0.5 );
// 	PixelCoordsDownFilter[7].Set(  0.5,   1.5 );
// 
// 	PixelCoordsDownFilter[8].Set( -0.5,  -1.5 );
// 	PixelCoordsDownFilter[9].Set( -0.5,  -0.5 );
// 	PixelCoordsDownFilter[10].Set( -0.5,   0.5 );
// 	PixelCoordsDownFilter[11].Set( -0.5,   1.5 );
// 
// 	PixelCoordsDownFilter[12].Set( -1.5,  -1.5 );
// 	PixelCoordsDownFilter[13].Set( -1.5,  -0.5 );
// 	PixelCoordsDownFilter[14].Set( -1.5,   0.5 );
// 	PixelCoordsDownFilter[15].Set( -1.5,   1.5 );
// 
// 
// 	for(int i=0;i<16;i++)
// 	{
// 		PixelCoordsDownFilter[i].x*=1.0f/aTargetDimensions.x;
// 		PixelCoordsDownFilter[i].y*=1.0f/aTargetDimensions.y;
// 	}
// 
// 	Engine::GetInstance()->GetEffectInput().SetDownsampleArray(PixelCoordsDownFilter);
	Vector2f textelSize(1,1);
	textelSize.x /= aTargetDimensions.x;
	textelSize.y /= aTargetDimensions.y;
	Engine::GetInstance()->GetEffectInput().SetVector( "texelSize", textelSize );
	return true;
}

void Renderer::Downsample()
{
	ID3D10RenderTargetView* tempRenderTargetView;

	// RENDER DOWNSAMPLE2 TEXTURE
	Vector2f destScreenSize(Engine::GetInstance()->GetScreeenWidth()/2, Engine::GetInstance()->GetScreenHeight()/2);
	SetDownsampleVariables( destScreenSize );
	tempRenderTargetView = myDownsampledTexture2.GetRenderTargetView();
	myFullscreenHelper.Process(myRenderTargetTexture.GetShaderResourceView(),
		tempRenderTargetView, EffectTechniques::QUAD_DOWNSAMPLE,
		destScreenSize, Vector2f(0,0));

	// RENDER DOWNSAMPLE4 TEXTURE
	destScreenSize /= 2;
	SetDownsampleVariables( destScreenSize );
	tempRenderTargetView = myDownsampledTexture4.GetRenderTargetView();
	myFullscreenHelper.Process(myDownsampledTexture2.GetShaderResourceView(),
		tempRenderTargetView, EffectTechniques::QUAD_DOWNSAMPLE,
		destScreenSize, Vector2f(0,0));

	// RENDER DOWNSAMPLE8 TEXTURE
	destScreenSize /= 2;
	SetDownsampleVariables( destScreenSize );
	tempRenderTargetView = myDownsampledTexture8.GetRenderTargetView();
	myFullscreenHelper.Process(myDownsampledTexture4.GetShaderResourceView(),
		tempRenderTargetView, EffectTechniques::QUAD_DOWNSAMPLE,
		destScreenSize, Vector2f(0,0));

	// RENDER DOWNSAMPLE16 TEXTURE
	destScreenSize /= 2;
	SetDownsampleVariables( destScreenSize );
	tempRenderTargetView = myDownsampledTexture16.GetRenderTargetView();
	myFullscreenHelper.Process(myDownsampledTexture8.GetShaderResourceView(),
		tempRenderTargetView, EffectTechniques::QUAD_DOWNSAMPLE,
		destScreenSize, Vector2f(0,0));

	// RENDER DOWNSAMPLE32 TEXTURE
	destScreenSize /= 2;
	SetDownsampleVariables( destScreenSize );
	tempRenderTargetView = myDownsampledTexture32.GetRenderTargetView();
	myFullscreenHelper.Process(myDownsampledTexture16.GetShaderResourceView(),
		tempRenderTargetView, EffectTechniques::QUAD_DOWNSAMPLE,
		destScreenSize, Vector2f(0,0));

	// RENDER DOWNSAMPLE64 TEXTURE
	destScreenSize /= 2;
	SetDownsampleVariables( destScreenSize );
	tempRenderTargetView = myDownsampledTexture64.GetRenderTargetView();
	myFullscreenHelper.Process(myDownsampledTexture32.GetShaderResourceView(),
		tempRenderTargetView, EffectTechniques::QUAD_DOWNSAMPLE,
		destScreenSize, Vector2f(0,0));

	// RENDER DOWNSAMPLE128 TEXTURE
	destScreenSize /= 2;
	SetDownsampleVariables( destScreenSize );
	tempRenderTargetView = myDownsampledTexture128.GetRenderTargetView();
	myFullscreenHelper.Process(myDownsampledTexture64.GetShaderResourceView(),
		tempRenderTargetView, EffectTechniques::QUAD_DOWNSAMPLE,
		destScreenSize, Vector2f(0,0));

	// RENDER DOWNSAMPLE256 TEXTURE
	destScreenSize /= 2;
	SetDownsampleVariables( destScreenSize );
	tempRenderTargetView = myDownsampledTexture256.GetRenderTargetView();
	myFullscreenHelper.Process(myDownsampledTexture128.GetShaderResourceView(),
		tempRenderTargetView, EffectTechniques::QUAD_DOWNSAMPLE,
		destScreenSize, Vector2f(0,0));

	// RENDER DOWNSAMPLE512 TEXTURE
	destScreenSize /= 2;
	SetDownsampleVariables( destScreenSize );
	tempRenderTargetView = myDownsampledTexture512.GetRenderTargetView();
	myFullscreenHelper.Process(myDownsampledTexture256.GetShaderResourceView(),
		tempRenderTargetView, EffectTechniques::QUAD_DOWNSAMPLE,
		destScreenSize, Vector2f(0,0));

	// RENDER DOWNSAMPLE ONE PIXEL TEXTURE
	SetDownsampleVariables( Vector2f(1, 1) );
	tempRenderTargetView = myDownsampledOnePixel.GetRenderTargetView();
	myFullscreenHelper.Process(myDownsampledTexture512.GetShaderResourceView(),
		tempRenderTargetView, EffectTechniques::QUAD_DOWNSAMPLE,
		Vector2f(1, 
		1), Vector2f(0,0));
}

void Renderer::SetHDRFlag( const bool aFlag )
{
	myHDRFlag = aFlag;
}

void Renderer::SetInstancingFlag( const bool aFlag )
{
	myInstancingFlag = aFlag;
}

bool Renderer::InitShadowDepthStencil()
{
	HRESULT hr = S_OK;

	// Create depth stencil texture
	D3D10_TEXTURE2D_DESC descDepth;
	descDepth.Width = Engine::GetInstance()->GetScreeenWidth();
	descDepth.Height = Engine::GetInstance()->GetScreenHeight();
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D10_USAGE_DEFAULT;
	descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = Engine::GetInstance()->GetDevice()->CreateTexture2D( &descDepth, NULL, &myShadowZBuffer );
	if( FAILED(hr) )
		return false;

	// Create the depth stencil view
	ID3D10DepthStencilView* tempDepthStencilView;
	D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = Engine::GetInstance()->GetDevice()->CreateDepthStencilView( myShadowZBuffer, &descDSV, &tempDepthStencilView );
	if( FAILED(hr) )
		return false;

	myShadowDepthStencilView = tempDepthStencilView;

	return true;
}

void Renderer::RenderDepthNormalAlbedo()
{	
	ID3D10RenderTargetView *renderTargets[5] = {myDepthMap.GetRenderTargetView(), myNormalMap.GetRenderTargetView(), myAlbedoMap.GetRenderTargetView(), myAmbientMap.GetRenderTargetView(), myLinearDepthMap.GetRenderTargetView()};
	
	Engine::GetInstance()->GetDevice()->OMSetRenderTargets(5, renderTargets, Engine::GetInstance()->GetDepthStencil() );
	
	myScene->Render( EffectTechniques::DEPTH_NORMAL_ALBEDO );
}

void Renderer::RenderDeferredDirectionalLights(RenderTarget *aRenderTarget, RenderTarget& aDepthMap, const Vector2f &aSize, const Vector2f anOffset)
{
	for(int index = 0; index < myScene->myLights.Count(); index++)
	{
		Light *currentLight = myScene->myLights[index];
		if(currentLight->GetType() == Light::DIRECTIONAL_LIGHT_TYPE)
		{
			//Engine::GetInstance()->GetEffectInput().SetSecondaryPostProcesingTexture(myNormalMap.GetShaderResourceView());
			//Engine::GetInstance()->GetEffectInput().SetThirdPostProcesingTexture(myAlbedoMap.GetShaderResourceView());

			Engine::GetInstance()->GetEffectInput().UpdateDirectionalLight(currentLight->GetLightColor(), currentLight->GetCurrentLightDir());
			myFullscreenHelper.Process(aDepthMap.GetShaderResourceView(), aRenderTarget->GetRenderTargetView(), EffectTechniques::DEFERRED_DIRECTIONAL, aSize, anOffset);
		}
	}
}

void Renderer::RenderDeferredPointLights(RenderTarget *aRenderTarget, const Vector2f &aSize, const Vector2f anOffset)
{
	for(int index = 0; index < myScene->myLights.Count(); index++)
	{
		Light *currentLight = myScene->myLights[index];
		if(currentLight->myIsInsideFrustrum == false)
		{
			continue;
		}
		if(currentLight->GetType() == Light::POINT_LIGHT_TYPE)
		{
			Engine::GetInstance()->GetEffectInput().UpdatePointLight(currentLight->GetLightColor(), currentLight->GetPosition(), currentLight->GetMaxDistance());
			//currentLight->SetProjectionTexture();

			ID3D10RenderTargetView *tempView = aRenderTarget->GetRenderTargetView();
			//Engine::GetInstance()->GetDevice()->OMSetRenderTargets( 1, &tempView, Engine::GetInstance()->GetDepthStencil());

			Vector4f sphereScreen = Vector4f(currentLight->GetPosition().myX, currentLight->GetPosition().myY, currentLight->GetPosition().myZ, 0.0f);
			sphereScreen.w = 1.0f;
			sphereScreen = sphereScreen * myScene->GetCamera().GetInverseOrientation();
			float z = sphereScreen.z;
			sphereScreen = sphereScreen * myScene->GetCamera().GetProjection();
			sphereScreen /= sphereScreen.w;
			Vector2f screenHalfRes;
			screenHalfRes.x = static_cast<float>(Engine::GetInstance()->GetScreeenWidth()) * 0.5f;
			screenHalfRes.y = static_cast<float>(Engine::GetInstance()->GetScreenHeight()) * 0.5f;
			sphereScreen.x = sphereScreen.x * screenHalfRes.x + screenHalfRes.x;
			sphereScreen.y = -sphereScreen.y * screenHalfRes.y + screenHalfRes.y;
			float distance = z;
			float specialWidth;
			if( distance > 0.0f )
			{
				specialWidth = screenHalfRes.x * 2.0f * ( currentLight->GetMaxDistance() / distance );
			}
			else if( distance == 0.0f )
			{
				specialWidth = screenHalfRes.x * 2.0f * ( currentLight->GetMaxDistance() / 0.0000001f );
			}
			else
			{
				specialWidth = screenHalfRes.x * 2.0f * ( currentLight->GetMaxDistance() / -distance );
			}
			Vector2i sphereScreen2D = Vector2i( static_cast<int>(sphereScreen.x), static_cast<int>(sphereScreen.y) );
			Vector2i maxDistance2D = Vector2i( static_cast<int>(specialWidth), static_cast<int>(specialWidth) );
			Vector2i minCorner = sphereScreen2D - maxDistance2D;
			Vector2i maxCorner = sphereScreen2D + maxDistance2D;
			Engine::GetInstance()->SetScissorRect( minCorner, maxCorner );

			mySphereMesh->Scale(currentLight->GetMaxDistance() * 0.5913985f);
			mySphereMesh->SetPosition(currentLight->GetPosition());

			const float distance2 = (myScene->GetCamera().GetPositionNotRetarded() - currentLight->GetPosition()).Length();
			const bool isCameraInsideMeshFlag = (distance2 <= currentLight->GetMaxDistance());

			//myFullscreenHelper.Process(myDepthMap.GetShaderResourceView(), tempView, EffectTechniques::DEFERRED_POINT_SCISSORED, aSize, anOffset);
			myFullscreenHelper.ProcessMeshCulling(myScene->GetCamera(), mySphereMesh, EffectTechniques::DEFERRED_POINT_MESH, myDepthMap.GetShaderResourceView(), tempView, isCameraInsideMeshFlag);	
		}
	}
}

void Renderer::RenderDeferredSpotLights(RenderTarget *aRenderTarget, RenderTarget& aDepthMap, const Vector2f &aSize, const Vector2f anOffset, const bool anEnableSpecFlag)
{
	const int nbrOfLights = myScene->myLights.Count();
	for(int index = 0; index < nbrOfLights; index++)
	{
		Light *currentLight = myScene->myLights[index];
		if(currentLight->GetType() == Light::SPOT_LIGHT_TYPE)
		{
			Engine::GetInstance()->GetEffectInput().UpdateSpotLight(currentLight->GetLightColor(), currentLight->GetCurrentLightDir(), currentLight->GetPosition(), currentLight->GetMaxDistance(), currentLight->GetInnerFallofAngle(), currentLight->GetOuterFallofAngle());
			
			//currentLight->SetProjectionTexture();

			Matrix44f temp = currentLight->myLightViewMatrix;
			//temp.SetPosition(Vector3f(0.0f, 0.0f, 0.0f));
			D3DXMATRIX matrix(&temp.myData[0]);
			D3DXMATRIX inverse;
			D3DXMATRIX* result = D3DXMatrixInverse(&inverse, NULL, &matrix);
			assert(result != NULL && "D3DXMatrixInverse failed");
			Matrix44f newInverse;
			newInverse.Init(&inverse[0]);
			//newInverse.SetPosition(currentLight->myLightViewMatrix.GetPosition());
			//Vector4f hej(10.0f, 0.0f, 10.0f, 0.0f);
			//hej = hej * view;
			//hej = hej * currentLight->GetProjectionMatrix();
			Matrix44f view = newInverse;//newInverse;
			Matrix44f projection = currentLight->GetProjectionMatrix();
 			Engine::GetInstance()->GetEffectInput().SetMatrixArray( "LightView", &view, 1 );
 			Engine::GetInstance()->GetEffectInput().SetMatrixArray( "LightProjection", &projection, 1 );

			const float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
			Engine::GetInstance()->GetDevice()->ClearRenderTargetView(currentLight->myShadowMap->myRenderTargetView, clearColor);
			Engine::GetInstance()->GetDevice()->ClearRenderTargetView(myShadowMapCopy.myRenderTargetView, clearColor);
			Engine::GetInstance()->GetDevice()->ClearRenderTargetView(myShadowMapCopy2.myRenderTargetView, clearColor);
			Engine::GetInstance()->GetDevice()->ClearDepthStencilView(Engine::GetInstance()->GetDepthStencil(), D3D10_CLEAR_DEPTH, 1.0f, 0);
			Engine::GetInstance()->GetDevice()->OMSetRenderTargets(1, &currentLight->myShadowMap->myRenderTargetView, Engine::GetInstance()->GetDepthStencil());
			myScene->Render(EffectTechniques::DEPTH_FROM_LIGHT);
			
			//WriteTextureToDisk(*currentLight->myShadowMap, "shadow1");
			myFullscreenHelper.Process(currentLight->myShadowMap->GetShaderResourceView(), myShadowMapCopy.GetRenderTargetView(), "BlurShadowX", Vector2f(800.0f, 800.0f));
			myFullscreenHelper.Process(myShadowMapCopy.GetShaderResourceView(), myShadowMapCopy2.GetRenderTargetView(), "BlurShadowY", Vector2f(800.0f, 800.0f));
			myFullscreenHelper.Process(myShadowMapCopy2.GetShaderResourceView(), currentLight->myShadowMap->GetRenderTargetView(), "Render_Quad", Vector2f(800.0f, 800.0f));
			//myFullscreenHelper.Process(currentLight->myShadowMap->GetShaderResourceView(), myShadowMapCopy.GetRenderTargetView(), "JaffeSpecialBlurHorizontal2", Vector2f(800.0f, 800.0f));
			//myFullscreenHelper.Process(myShadowMapCopy.GetShaderResourceView(), myShadowMapCopy2.GetRenderTargetView(), "JaffeSpecialBlurVertical2", Vector2f(800.0f, 800.0f));
			//myFullscreenHelper.Process(myShadowMapCopy2.GetShaderResourceView(), currentLight->myShadowMap->GetRenderTargetView(), "Render_Quad", Vector2f(800.0f, 800.0f));
			//WriteTextureToDisk(*currentLight->myShadowMap, "shadow2");
			//WriteTextureToDisk(myShadowMapCopy, "shadow3");
			//std::stringstream tempSS;
			//tempSS << "ShadowDepthMap" << index + 1;
			//WriteTextureToDisk(myShadowMapCopy2, tempSS.str());
			Engine::GetInstance()->GetEffectInput().SetShadowTexture(currentLight->myShadowMap->GetShaderResourceView());

			if(myToggleAdvancedCulling == false)
			{
				if(anEnableSpecFlag == true)
				{
					myFullscreenHelper.Process(aDepthMap.GetShaderResourceView(), aRenderTarget->GetRenderTargetView(), EffectTechniques::DEFERRED_SPOT, aSize, anOffset);
				}
				else
				{
					myFullscreenHelper.Process(aDepthMap.GetShaderResourceView(), aRenderTarget->GetRenderTargetView(), "Render_DeferredSpotNoSpec", aSize, anOffset);
				}
			}
			else
			{
				Vector4f sphereScreen = Vector4f(currentLight->GetPosition().myX, currentLight->GetPosition().myY, currentLight->GetPosition().myZ, 0.0f);
				sphereScreen.w = 1.0f;
				sphereScreen = sphereScreen * myScene->GetCamera().GetInverseOrientation();
				float z = sphereScreen.z;
				sphereScreen = sphereScreen * myScene->GetCamera().GetProjection();
				sphereScreen /= sphereScreen.w;
				Vector2f screenHalfRes;
				screenHalfRes.x = static_cast<float>(Engine::GetInstance()->GetScreeenWidth()) * 0.5f;
				screenHalfRes.y = static_cast<float>(Engine::GetInstance()->GetScreenHeight()) * 0.5f;
				sphereScreen.x = sphereScreen.x * screenHalfRes.x + screenHalfRes.x;
				sphereScreen.y = -sphereScreen.y * screenHalfRes.y + screenHalfRes.y;
				float distance = z;
				float specialWidth;
				if( distance > 0.0f )
				{
					specialWidth = screenHalfRes.x * 2.0f * ( currentLight->GetMaxDistance() / distance );
				}
				else
				{
					specialWidth = screenHalfRes.x * 2.0f * ( currentLight->GetMaxDistance() / 0.01f );
				}
				Vector2i sphereScreen2D = Vector2i( static_cast<int>(sphereScreen.x), static_cast<int>(sphereScreen.y) );
				Vector2i maxDistance2D = Vector2i( static_cast<int>(specialWidth), static_cast<int>(specialWidth) );
				Vector2i minCorner = sphereScreen2D - maxDistance2D;
				Vector2i maxCorner = sphereScreen2D + maxDistance2D;
				Engine::GetInstance()->SetScissorRect( minCorner, maxCorner );
				myFullscreenHelper.Process(aDepthMap.GetShaderResourceView(), aRenderTarget->GetRenderTargetView(), EffectTechniques::DEFERRED_SPOT_SCISSORED, aSize, anOffset);
			}
		}
		Engine::GetInstance()->GetEffectInput().SetShadowTexture(NULL);
	}
}

void Renderer::RenderSSAO(RenderTarget &aTexture, const Vector2f &aSize, const Vector2f anOffset)
{
	Engine::GetInstance()->GetEffectInput().SetNoiseTexture(myNoiseTexture->GetTexture());
	Engine::GetInstance()->GetEffectInput().SetThirdPostProcesingTexture(myNormalMap.GetShaderResourceView());
	
	ID3D10RenderTargetView *tempView = aTexture.GetRenderTargetView();
	myFullscreenHelper.Process(myDepthMap.GetShaderResourceView(),
		tempView, 
		EffectTechniques::SSAO,
		aSize,
		anOffset);
}

void Renderer::WriteTextureToDisk(RenderTarget& aTexture, std::string aFilename)
{
	const std::string fileName = "TextureOutput/" + aFilename + ".dds";
	HRESULT hr = D3DX10SaveTextureToFile(aTexture.myTexture, D3DX10_IFF_DDS, fileName.c_str());
	assert(hr == S_OK && "SAVE TEXTURE TO FILE FAILED");
}

void Renderer::WriteAllToDisk()
{
	
}

ID3D10ShaderResourceView* Renderer::GetGeneratedCubeMap(const int anAmbientProbeIndex)
{
	return myAmbientProbes[anAmbientProbeIndex].myShaderResourceView;
}

ID3D10ShaderResourceView* Renderer::GetGeneratedReflectionMap(const int anAmbientProbeIndex)
{
	return myAmbientProbes[anAmbientProbeIndex].myReflectionShaderResourceView;
}

FullscreenHelper& Renderer::GetFullScreenHelper()
{
	return myFullscreenHelper;
}

void Renderer::CopyAlbedo()
{
	myFullscreenHelper.Process(myAlbedoMap.GetShaderResourceView(),
		myFinalScene.GetRenderTargetView(), EffectTechniques::QUAD,
		Vector2f(Engine::GetInstance()->GetScreeenWidth(), Engine::GetInstance()->GetScreenHeight()),
		Vector2f(0,0) );
}


