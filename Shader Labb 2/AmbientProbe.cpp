#include "AmbientProbe.h"
#include <D3D10.h>
#include "Engine.h"

const DXGI_FORMAT formatToUse = DXGI_FORMAT_R32G32B32A32_FLOAT;//DXGI_FORMAT_R8G8B8A8_UNORM

AmbientProbe::AmbientProbe()
{
	for(int index = 0; index < 6; index++)
	{
		myCubeMapSides[index] = NULL;
		myReflectionCubeMapSides[index] = NULL;
		myCubeMapSidesColor[index] = NULL;
		myCubeMapSidesDepth[index] = NULL;
		myCrossMapColor[index] = NULL;
		myCrossMapDepth[index] = NULL;
	}

	myCrossMapCopy = NULL;
	myCrossMapCopy2 = NULL;
	myTemporaryNormalMap = NULL;
	myTexture = NULL;
	myReflectionTexture = NULL;
	myShaderResourceView = NULL;
	myReflectionShaderResourceView = NULL;
	myTemporaryColorMap = NULL;
}

AmbientProbe::~AmbientProbe()
{
}

void AmbientProbe::Init()
{
	CreateTexture2D();
	CreateShaderResourceView();
	CreateRenderTargetView();

	myCrossMapCopy = new RenderTarget();
	myCrossMapCopy->Init(formatToUse, 60);

	myCrossMapCopy2 = new RenderTarget();
	myCrossMapCopy2->Init(formatToUse, 60);
	
	myTemporaryNormalMap = new RenderTarget();
	myTemporaryNormalMap->Init(DXGI_FORMAT_R32G32B32A32_FLOAT);

	myTemporaryColorMap = new RenderTarget();
	myTemporaryColorMap->Init(formatToUse);
}

void AmbientProbe::CreateTexture2D()
{
	D3D10_TEXTURE2D_DESC renderTarget;
	renderTarget.Width = 20;
	renderTarget.Height = 20;
	renderTarget.ArraySize = 6;
	renderTarget.SampleDesc.Count = 1;
	renderTarget.SampleDesc.Quality = 0;
	renderTarget.Format = formatToUse;//DXGI_FORMAT_R16G16B16A16_FLOAT
	renderTarget.Usage = D3D10_USAGE_DEFAULT;
	renderTarget.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
	renderTarget.CPUAccessFlags = 0;
	renderTarget.MiscFlags = D3D10_RESOURCE_MISC_GENERATE_MIPS | D3D10_RESOURCE_MISC_TEXTURECUBE;
	renderTarget.MipLevels = 5;
	HRESULT hr = Engine::GetInstance()->GetDevice()->CreateTexture2D(&renderTarget, NULL, &myTexture );
	hr = Engine::GetInstance()->GetDevice()->CreateTexture2D(&renderTarget, NULL, &myReflectionTexture);
	if(FAILED(hr))
	{
		assert(0 && "HR FAILED");
	}
	
}

void AmbientProbe::CreateShaderResourceView()
{
	D3D10_SHADER_RESOURCE_VIEW_DESC srv;
	srv.Format = formatToUse;
	srv.ViewDimension = D3D10_SRV_DIMENSION_TEXTURECUBE;
	srv.TextureCube.MipLevels = 5;
	srv.TextureCube.MostDetailedMip = 0;
	HRESULT hr = Engine::GetInstance()->GetDevice()->CreateShaderResourceView(myTexture, &srv, &myShaderResourceView);
	hr = Engine::GetInstance()->GetDevice()->CreateShaderResourceView(myReflectionTexture, &srv, &myReflectionShaderResourceView);
	if(FAILED(hr))
	{
		assert(0 && "HR FAILED");
	}
}

void AmbientProbe::CreateRenderTargetView()
{
	D3D10_RENDER_TARGET_VIEW_DESC targets;
	targets.Format = formatToUse;
	targets.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2DARRAY;
	targets.Texture2DArray.ArraySize = 1;
	targets.Texture2DArray.MipSlice = 0;
	
	for (int index = 0; index < 6; index++)
	{
		targets.Texture2DArray.FirstArraySlice = index;

		ID3D10RenderTargetView* renderTargetView = NULL;
		HRESULT hr = Engine::GetInstance()->GetDevice()->CreateRenderTargetView(myTexture, &targets, &renderTargetView);
		if(FAILED(hr))
		{
			assert(0 && "HR FAILED");
		}
		float clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
		Engine::GetInstance()->GetDevice()->ClearRenderTargetView(renderTargetView, clearColor);
		myCubeMapSides[index] = new RenderTarget();
		myCubeMapSides[index]->myTexture = myTexture;
		myCubeMapSides[index]->myShaderResourceView = myShaderResourceView;
		myCubeMapSides[index]->myRenderTargetView = renderTargetView;

		ID3D10RenderTargetView* reflectionRenderTargetView = NULL;
		hr = Engine::GetInstance()->GetDevice()->CreateRenderTargetView(myReflectionTexture, &targets, &reflectionRenderTargetView);
		if(FAILED(hr))
		{
			assert(0 && "HR FAILED");
		}
		Engine::GetInstance()->GetDevice()->ClearRenderTargetView(reflectionRenderTargetView, clearColor);
		myReflectionCubeMapSides[index] = new RenderTarget();
		myReflectionCubeMapSides[index]->myTexture = myReflectionTexture;
		myReflectionCubeMapSides[index]->myShaderResourceView = myReflectionShaderResourceView;
		myReflectionCubeMapSides[index]->myRenderTargetView = reflectionRenderTargetView;

		myCubeMapSidesColor[index] = new RenderTarget();
		myCubeMapSidesColor[index]->Init(formatToUse);
		myCubeMapSidesDepth[index] = new RenderTarget();
		myCubeMapSidesDepth[index]->Init(formatToUse);
		myCrossMapColor[index] = new RenderTarget();
		myCrossMapColor[index]->Init(formatToUse, 60);
		myCrossMapDepth[index] = new RenderTarget();
		myCrossMapDepth[index]->Init(formatToUse, 60);
	}
}