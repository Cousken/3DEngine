#include "RenderTarget.h"
#include "Engine.h"

RenderTarget::RenderTarget( void )
{
	myTexture = NULL;
	myRenderTargetView = NULL;
	myShaderResourceView = NULL;
}


RenderTarget::~RenderTarget(void)
{
}

bool RenderTarget::Init( DXGI_FORMAT aTextrueFormat /*= DXGI_FORMAT_R8G8B8A8_UNORM*/, const float aDownsampleFactor /*= 1.0f */ )
{
	HRESULT hr = S_OK;
	D3D10_TEXTURE2D_DESC tempBufferInfo;
	tempBufferInfo.Width = Engine::GetInstance()->GetScreeenWidth() / aDownsampleFactor;
	tempBufferInfo.Height = Engine::GetInstance()->GetScreenHeight() / aDownsampleFactor;
	tempBufferInfo.MipLevels = 1;
	tempBufferInfo.ArraySize = 1;
	tempBufferInfo.Format = aTextrueFormat;
	tempBufferInfo.SampleDesc.Count = 1;
	tempBufferInfo.SampleDesc.Quality = 0;
	tempBufferInfo.Usage = D3D10_USAGE_DEFAULT;
	tempBufferInfo.BindFlags =  D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
	tempBufferInfo.CPUAccessFlags = 0;
	tempBufferInfo.MiscFlags = 0;

	hr = Engine::GetInstance()->GetDevice()->CreateTexture2D( &tempBufferInfo, NULL, &myTexture );
	if( FAILED( hr ) )
		assert(0);
	hr = Engine::GetInstance()->GetDevice()->CreateRenderTargetView( myTexture, NULL, &myRenderTargetView );
	if( FAILED( hr ) )
		assert(0);
	hr = Engine::GetInstance()->GetDevice()->CreateShaderResourceView( myTexture, NULL, &myShaderResourceView );
	if( FAILED( hr ) )
		assert(0);


	if( FAILED(hr) )
		return false;

	return true;
}

bool RenderTarget::Init(DXGI_FORMAT aTextrueFormat, const int aWidthAndHeight)
{
	HRESULT hr = S_OK;
	D3D10_TEXTURE2D_DESC tempBufferInfo;
	tempBufferInfo.Width = aWidthAndHeight;
	tempBufferInfo.Height = aWidthAndHeight;
	tempBufferInfo.MipLevels = 1;
	tempBufferInfo.ArraySize = 1;
	tempBufferInfo.Format = aTextrueFormat;
	tempBufferInfo.SampleDesc.Count = 1;
	tempBufferInfo.SampleDesc.Quality = 0;
	tempBufferInfo.Usage = D3D10_USAGE_DEFAULT;
	tempBufferInfo.BindFlags =  D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
	tempBufferInfo.CPUAccessFlags = 0;
	tempBufferInfo.MiscFlags = 0;

	hr = Engine::GetInstance()->GetDevice()->CreateTexture2D(&tempBufferInfo, NULL, &myTexture);
	if( FAILED( hr ) )
		assert(0);
	hr = Engine::GetInstance()->GetDevice()->CreateRenderTargetView(myTexture, NULL, &myRenderTargetView);
	if( FAILED( hr ) )
		assert(0);
	hr = Engine::GetInstance()->GetDevice()->CreateShaderResourceView(myTexture, NULL, &myShaderResourceView);
	if( FAILED( hr ) )
		assert(0);

	if( FAILED(hr) )
		return false;

	return true;
}

void RenderTarget::Release()
{
	if( myRenderTargetView ) myRenderTargetView->Release();
	if( myShaderResourceView ) myShaderResourceView->Release();
	if( myTexture ) myTexture->Release();
}

ID3D10RenderTargetView* RenderTarget::GetRenderTargetView()
{
	return myRenderTargetView;
}

ID3D10RenderTargetView** RenderTarget::GetRenderTargetViewDoublePointer()
{
	return &myRenderTargetView;
}

ID3D10ShaderResourceView* RenderTarget::GetShaderResourceView()
{
	return myShaderResourceView;
}

bool RenderTarget::InitForOnePixel()
{
	HRESULT hr = S_OK;
	D3D10_TEXTURE2D_DESC tempBufferInfo;
	tempBufferInfo.Width = 1;
	tempBufferInfo.Height = 1;
	tempBufferInfo.MipLevels = 1;
	tempBufferInfo.ArraySize = 1;
	tempBufferInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tempBufferInfo.SampleDesc.Count = 1;
	tempBufferInfo.SampleDesc.Quality = 0;
	tempBufferInfo.Usage = D3D10_USAGE_DEFAULT;
	tempBufferInfo.BindFlags =  D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
	tempBufferInfo.CPUAccessFlags = 0;
	tempBufferInfo.MiscFlags = 0;

	hr = Engine::GetInstance()->GetDevice()->CreateTexture2D( &tempBufferInfo, NULL, &myTexture );
	if( FAILED( hr ) )
		assert(0);
	hr = Engine::GetInstance()->GetDevice()->CreateRenderTargetView( myTexture, NULL, &myRenderTargetView );
	if( FAILED( hr ) )
		assert(0);
	hr = Engine::GetInstance()->GetDevice()->CreateShaderResourceView( myTexture, NULL, &myShaderResourceView );
	if( FAILED( hr ) )
		assert(0);


	if( FAILED(hr) )
		return false;

	return true;
}

bool RenderTarget::InitCube( DXGI_FORMAT aTextrueFormat )
{
	D3D10_TEXTURE2D_DESC tempBufferInfo;
	tempBufferInfo.Width = Engine::GetInstance()->GetScreeenWidth();
    tempBufferInfo.Height = Engine::GetInstance()->GetScreenHeight();
    tempBufferInfo.MipLevels = 1;
    tempBufferInfo.ArraySize = 6;
	tempBufferInfo.Format =  aTextrueFormat;
    tempBufferInfo.SampleDesc.Count = 1;
    tempBufferInfo.SampleDesc.Quality = 0;
    tempBufferInfo.Usage = D3D10_USAGE_DEFAULT;
    tempBufferInfo.BindFlags =  D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;

    tempBufferInfo.CPUAccessFlags = 0;
    tempBufferInfo.MiscFlags = D3D10_RESOURCE_MISC_TEXTURECUBE;

	HRESULT hr = Engine::GetInstance()->GetDevice()->CreateTexture2D( &tempBufferInfo, NULL, &myTexture );
	assert( !FAILED( hr ) );
    if( FAILED( hr ) )
		return false;

	hr = Engine::GetInstance()->GetDevice()->CreateRenderTargetView( myTexture, NULL, &myRenderTargetView );
	assert( !FAILED( hr ) );
	if( FAILED( hr ) )
	   return false;

	hr = Engine::GetInstance()->GetDevice()->CreateShaderResourceView( myTexture, NULL, &myShaderResourceView );
	assert( !FAILED( hr ) );
	if( FAILED( hr ) )
	   return false;

	return true;
}