#include "Renderer.h"
#include "Scene.h"
#include "Engine.h"

Renderer::Renderer(void)
{
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
	Engine::GetInstance()->GetEffectInput().UpdateCamera(myScene->myCamera);

 	ID3D10RenderTargetView* tempRenderTargetView;	

	//tempRenderTargetView = mySceneTexture.GetRenderTargetView();
	//RenderScene();
	RenderDepthNormalAlbedo();
	//CopyAlbedo();

	const Vector2f ScreenSize(Engine::GetInstance()->GetScreeenWidth(), Engine::GetInstance()->GetScreenHeight());
 	const Vector2f Offset(0.0f, 0.0f);
	RenderSSAO(myAmbientOcclusionMap, ScreenSize, Offset);

	//RENDER DEFERRED
	RenderDeferredDirectionalLights(&myFinalScene, ScreenSize, Offset);
	//if(myTogglePointOrSpotLight == true)
	{
		RenderDeferredPointLights(&myFinalScene, ScreenSize, Offset);
	}
	//else
	{
		RenderDeferredSpotLights(&myFinalScene, ScreenSize, Offset);
	}

	if(myToggleQuadOrFullScreen == true)
	{
		//RENDER THE LITTLE VIEWS ON THE LEFT
		// DEPTH
		tempRenderTargetView = myRenderTargetTexture.GetRenderTargetView();
		float targetWidth = Engine::GetInstance()->GetScreeenWidth()/2;
		float targetHeight = Engine::GetInstance()->GetScreenHeight()/2;	
		myFullscreenHelper.Process(myDepthMap.GetShaderResourceView(),
			tempRenderTargetView, EffectTechniques::QUAD,
			Vector2f( targetWidth, targetHeight),
			Vector2f( 0, targetHeight * 0));

		//NORMAL
		myFullscreenHelper.Process(myNormalMap.GetShaderResourceView(),
			tempRenderTargetView, EffectTechniques::QUAD,
			Vector2f( targetWidth, targetHeight),
			Vector2f( targetWidth, 0));

		//ALBEDO
		myFullscreenHelper.Process(myAlbedoMap.GetShaderResourceView(),
			tempRenderTargetView, EffectTechniques::QUAD,
			Vector2f( targetWidth, targetHeight),
			Vector2f( 0, targetHeight));

		//FULL SCENE
		myFullscreenHelper.Process(myFinalScene.GetShaderResourceView(),
			tempRenderTargetView, EffectTechniques::QUAD,
			Vector2f( targetWidth, targetHeight),
			Vector2f( targetWidth, targetHeight));


		//RENDERS FINAL FULLSCREEN FRAME TEXTURE
		//RenderFinalFrameTexture(myRenderTargetTexture, Vector2f(Engine::GetInstance()->GetScreeenWidth(), Engine::GetInstance()->GetScreenHeight()), Vector2f(0.0f, 0.0f));
		RenderFinalFrameTexture(myRenderTargetTexture, Vector2f(Engine::GetInstance()->GetScreeenWidth(), Engine::GetInstance()->GetScreenHeight()), Vector2f(0.0f, 0.0f));
	}
	else
	{
		//FULL SCENE
		myFullscreenHelper.Process(myFinalScene.GetShaderResourceView(),
			myRenderTargetTexture.GetRenderTargetView(), EffectTechniques::QUAD,
			Vector2f( Engine::GetInstance()->GetScreeenWidth(), Engine::GetInstance()->GetScreenHeight()),
			Vector2f( 0.0f, 0.0f));

		//RENDERS FINAL FULLSCREEN FRAME TEXTURE
		RenderFinalFrameTexture(myFinalScene, Vector2f(Engine::GetInstance()->GetScreeenWidth(), Engine::GetInstance()->GetScreenHeight()), Vector2f(0.0f, 0.0f));
	}
}

void Renderer::RenderToCube( const CommonUtilities::StaticArray< Matrix44f, 6 >& someToView )
{
	ID3D10RenderTargetView* tempRenderTargetView;

	float colour[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; //red,green,blue,alpha
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView( myCubeMap.GetRenderTargetView(), colour );

	tempRenderTargetView = myCubeMap.GetRenderTargetView();
	Engine::GetInstance()->GetDevice()->OMSetRenderTargets(1,
		&tempRenderTargetView,
		NULL);

	myScene->RenderToCube( someToView );

	Engine::GetInstance()->GetEffectInput().SetEnviromentMap( myCubeMap.GetShaderResourceView() );
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
		
	if ( myDownsampledTexture2.Init( DXGI_FORMAT_R16G16B16A16_UNORM, 2.0f ) == false)
	{
		return false;
	}
	if ( myDownsampledTexture4.Init( DXGI_FORMAT_R16G16B16A16_UNORM, 4.0f ) == false)
	{
		return false;
	}
	if ( myDownsampledTexture8.Init( DXGI_FORMAT_R16G16B16A16_UNORM, 8.0f ) == false)
	{
		return false;
	}
	if ( myDownsampledTexture16.Init( DXGI_FORMAT_R16G16B16A16_UNORM, 16.0f ) == false)
	{
		return false;
	}
	if ( myDownsampledTexture32.Init( DXGI_FORMAT_R16G16B16A16_UNORM, 32.0f ) == false)
	{
		return false;
	}
	if ( myDownsampledTexture64.Init( DXGI_FORMAT_R16G16B16A16_UNORM, 64.0f ) == false)
	{
		return false;
	}
	if ( myDownsampledTexture128.Init( DXGI_FORMAT_R16G16B16A16_UNORM, 128.0f ) == false)
	{
		return false;
	}
	if ( myDownsampledTexture256.Init( DXGI_FORMAT_R16G16B16A16_UNORM, 256.0f ) == false)
	{
		return false;
	}
	if ( myDownsampledTexture512.Init( DXGI_FORMAT_R16G16B16A16_UNORM, 512.0f ) == false)
	{
		return false;
	}
	if ( myDownsampledOnePixel.InitForOnePixel() == false )
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
	if ( myCubeMap.InitCube() == false )
	{
		return false;
	}
	if ( myDepthMap.Init( DXGI_FORMAT_R32G32B32A32_FLOAT ) == false )
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
	if(myAmbientOcclusionMap.Init(DXGI_FORMAT_R32G32B32A32_FLOAT) == false)
	{
		return false;
	}

	myNoiseTexture = Engine::GetInstance()->GetTextureContainer().GetTexture("Noise.png", false);
	if(myNoiseTexture == NULL)
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
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView( myDownsampledTexture2.GetRenderTargetView(), black );
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView( myDownsampledTexture4.GetRenderTargetView(), black );
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView( myDownsampledTexture8.GetRenderTargetView(), black );
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView( myDownsampledTexture16.GetRenderTargetView(), black );
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView( myDownsampledTexture32.GetRenderTargetView(), black );
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView( myDownsampledTexture64.GetRenderTargetView(), black );
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView( myDownsampledTexture128.GetRenderTargetView(), black );
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView( myDownsampledTexture512.GetRenderTargetView(), black );
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView( myDownsampledOnePixel.GetRenderTargetView(), black );
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView( myShadowBuffer.GetRenderTargetView(), black );
	Engine::GetInstance()->GetDevice()->ClearDepthStencilView( myShadowDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0 );
	
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView( myDepthMap.GetRenderTargetView(), black );
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView( myFinalScene.GetRenderTargetView(), black );
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView( myNormalMap.GetRenderTargetView(), black );
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView( myAlbedoMap.GetRenderTargetView(), black );
	Engine::GetInstance()->GetDevice()->ClearRenderTargetView( myAmbientOcclusionMap.GetRenderTargetView(), black );
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
	myCubeMap.Release();
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
	//RENDER DEPTH, NORMAL AND ALBEDO AT SAME TIME
	ID3D10RenderTargetView *renderTargets[3] = {myDepthMap.GetRenderTargetView(), 
		myNormalMap.GetRenderTargetView(),
		myAlbedoMap.GetRenderTargetView()};
	Engine::GetInstance()->GetDevice()->OMSetRenderTargets(3, renderTargets, Engine::GetInstance()->GetDepthStencil() );

	myScene->Render( EffectTechniques::DEPTH_NORMAL_ALBEDO );
}	
void Renderer::RenderDeferredDirectionalLights(RenderTarget *aRenderTarget, const Vector2f &aSize, const Vector2f anOffset)
{
	for(int index = 0; index < myScene->myLights.Count(); index++)
	{
		Light *currentLight = myScene->myLights[index];
		if(currentLight->GetType() == Light::DIRECTIONAL_LIGHT_TYPE)
		{
			Engine::GetInstance()->GetEffectInput().UpdateDirectionalLight(currentLight->GetLightColor(),
				currentLight->GetCurrentLightDir());
			
			Engine::GetInstance()->GetEffectInput().SetSecondaryPostProcesingTexture(myNormalMap.GetShaderResourceView());
			Engine::GetInstance()->GetEffectInput().SetThirdPostProcesingTexture(myAlbedoMap.GetShaderResourceView());

			myFullscreenHelper.Process(myDepthMap.GetShaderResourceView(),
				aRenderTarget->GetRenderTargetView(), 
				EffectTechniques::DEFERRED_DIRECTIONAL,
				aSize,
				anOffset);
		}
	}
}

void Renderer::RenderDeferredPointLights(RenderTarget *aRenderTarget, const Vector2f &aSize, const Vector2f anOffset)
{
	for(int index = 0; index < myScene->myLights.Count(); index++)
	{
		Light *currentLight = myScene->myLights[index];
		if(currentLight->GetType() == Light::POINT_LIGHT_TYPE)
		{
			Engine::GetInstance()->GetEffectInput().UpdatePointLight(currentLight->GetLightColor(),
				currentLight->GetPosition(),
				currentLight->GetMaxDistance());

			currentLight->SetProjectionTexture();
			Engine::GetInstance()->GetEffectInput().SetSecondaryPostProcesingTexture(myNormalMap.GetShaderResourceView());
			Engine::GetInstance()->GetEffectInput().SetThirdPostProcesingTexture(myAlbedoMap.GetShaderResourceView());

			ID3D10RenderTargetView *tempView = aRenderTarget->GetRenderTargetView();
			//myFullscreenHelper.ProcessSphere(myDepthMap.GetShaderResourceView(),
			//	tempView,
			//	EffectTechniques::DEFERRED_POINT_MESH,
			//	aSize,
			//	anOffset,
			//	Vector3f(0.0f, 0.0f, 0.0f),
			//	myScene->GetCamera());

			myFullscreenHelper.Process(myDepthMap.GetShaderResourceView(),
				tempView, 
				EffectTechniques::DEFERRED_POINT,
				aSize,
				anOffset);
		}
	}
}

void Renderer::RenderDeferredSpotLights(RenderTarget *aRenderTarget, const Vector2f &aSize, const Vector2f anOffset)
{
	const int nbrOfLights = myScene->myLights.Count();
	for(int index = 0; index < nbrOfLights; index++)
	{
		Light *currentLight = myScene->myLights[index];
		if(currentLight->GetType() == Light::SPOT_LIGHT_TYPE)
		{
			Engine::GetInstance()->GetEffectInput().UpdateSpotLight(currentLight->GetLightColor(),
				currentLight->GetCurrentLightDir(),
				currentLight->GetPosition(),
				currentLight->GetMaxDistance(),
				currentLight->GetInnerFallofAngle(),
				currentLight->GetOuterFallofAngle());
			
			currentLight->SetProjectionTexture();

 			Matrix44f view = currentLight->GetViewMatrix();
			Matrix44f projection = currentLight->GetProjectionMatrix();
 			Engine::GetInstance()->GetEffectInput().SetMatrixArray( "LightView", &view, 1 );
 			Engine::GetInstance()->GetEffectInput().SetMatrixArray( "LightProjection", &projection, 1 );
			Engine::GetInstance()->GetEffectInput().SetSecondaryPostProcesingTexture(myNormalMap.GetShaderResourceView());
			Engine::GetInstance()->GetEffectInput().SetThirdPostProcesingTexture(myAlbedoMap.GetShaderResourceView());

			ID3D10RenderTargetView *tempView = aRenderTarget->GetRenderTargetView();
			myFullscreenHelper.Process(myDepthMap.GetShaderResourceView(),
				tempView, 
				EffectTechniques::DEFERRED_SPOT,
				aSize,
				anOffset);
		}
	}
}

void Renderer::RenderFinalFrameTexture(RenderTarget &aTextureToRender, const Vector2f &aSize, const Vector2f anOffset)
{
	myFullscreenHelper.Process(aTextureToRender.GetShaderResourceView(),
		Engine::GetInstance()->GetBackBuffer(), EffectTechniques::QUAD,
		aSize,
		anOffset); 
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

ID3D10ShaderResourceView* Renderer::GetGeneratedCubeMap()
{
	return myCubeMap.GetShaderResourceView();
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

