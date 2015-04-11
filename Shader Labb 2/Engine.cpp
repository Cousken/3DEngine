#include "Engine.h"
#include "Resource.h"
#include <string>
#include <assert.h>

Engine* Engine::ourInstance = NULL;

Engine::Engine( )
{
	myHInstance = NULL;
	myWindowHandler = NULL;
	myDriverType = D3D10_DRIVER_TYPE_NULL;
	my3DDevice = NULL;
	mySwapChain = NULL;
	myBackBufferRenderTargetView = NULL;
}

Engine::~Engine(void)
{
}

bool Engine::Create( int nCmdShow, SetupInfo aSetupInfo, WNDPROC aWinProc, HINSTANCE aHInstance)
{
	ourInstance = new Engine();

	return ourInstance->Init(aWinProc, nCmdShow, aSetupInfo, aHInstance);

}

void Engine::ShutDown()
{
	ourInstance->CleanupDevice();
	delete(ourInstance);
	ourInstance = 0;
}

//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
bool Engine::D3DSetup()
{	
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect( myWindowHandler, &rc );
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif

	hr = D3DDeviceSetup(width, height, createDeviceFlags);
	if( FAILED( hr ) )
		return false;

	hr = D3DViewPortSetup(width, height);
	if( FAILED( hr ) )
		return false;

	if ( InitZBuffer() == false)
	{
		return false;
	}

// 	if ( D3DStencilBufferSetup() == false)
// 	{
// 		return false;
// 	}

	return true;
}


void Engine::SwitchBuffers()
{
	// Just clear the backbuffer
	mySwapChain->Present( 0, 0 );
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; //red,green,blue,alpha
	my3DDevice->ClearRenderTargetView( myBackBufferRenderTargetView, ClearColor );
	myRenderer.ClearRenderTargets( ClearColor );

	//
	// Clear the depth buffer to 1.0 (max depth)
	//
	Engine::GetInstance()->GetDevice()->ClearDepthStencilView( myDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0 );
	Engine::GetInstance()->GetDevice()->OMSetRenderTargets( 1, &myBackBufferRenderTargetView, myDepthStencilView );
}

void Engine::CleanupDevice()
{
	if( my3DDevice ) my3DDevice->ClearState();

	if( myBackBufferRenderTargetView ) myBackBufferRenderTargetView->Release();
	if( mySwapChain ) mySwapChain->Release();
	if( my3DDevice ) my3DDevice->Release();
}

Engine* Engine::GetInstance()
{
	assert(ourInstance != 0 && "This Singleton hasn't been created yet!");
	return ourInstance;
}

bool Engine::WindowSetup(WNDPROC aWndProc,const SetupInfo& aInfoArgument)
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof( WNDCLASSEX );
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = aWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance =  myHInstance;
	wcex.hIcon = LoadIcon(  myHInstance, ( LPCTSTR )IDI_APPLICATION );
	wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
	wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = aInfoArgument.mySessionName.c_str();
	wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_APPLICATION );
	if( !RegisterClassEx( &wcex ) )
		return (false);

	// Create window
	RECT rc = { 0, 0, aInfoArgument.myResolutionWidth, aInfoArgument.myResolutionHeight };
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
	myWindowHandler = CreateWindow( aInfoArgument.mySessionName.c_str(), aInfoArgument.mySessionName.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL,  myHInstance,
		NULL );
	if( !myWindowHandler )
		return (false);

	ShowWindow(myWindowHandler, true);

	return (true);

}


bool Engine::Init( WNDPROC aWinProc, int nCmdShow, SetupInfo aSetupInfo, HINSTANCE aHInstance )
{
	myInfoArgument = aSetupInfo;
	myHInstance = aHInstance;
	myScene = new Scene();

	if ( FAILED( WindowSetup(aWinProc, aSetupInfo)))
	{
		return false;
	}

	if( Engine::GetInstance()->D3DSetup() == false )
	{
		Engine::GetInstance()->CleanupDevice();
		return false;
	}

	if ( myEffectInput.Init("Labb.fxh") == false )
	{
		return false;
	}

	if (myRenderer.Init() == false )
	{
		return false;
	}	
	myRenderer.SetScene(myScene);

	return true;
}

bool Engine::D3DViewPortSetup( UINT width, UINT height )
{
	HRESULT hr = S_OK;

	// Create a render target view
	ID3D10Texture2D* pBackBuffer;
	hr = mySwapChain->GetBuffer( 0, __uuidof( ID3D10Texture2D ), ( LPVOID* )&pBackBuffer );
	if( FAILED( hr ) )
		return false;
	ID3D10RenderTargetView* tempRenderTarget;
	hr = my3DDevice->CreateRenderTargetView( pBackBuffer, NULL, &tempRenderTarget );
	pBackBuffer->Release();
	if( FAILED( hr ) )
		return false;
	myBackBufferRenderTargetView = tempRenderTarget;

	// Setup the viewport
	D3D10_VIEWPORT vp;
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	my3DDevice->RSSetViewports( 1, &vp );

	if( SUCCEEDED( hr ) )
	{
		return true;
	}
	else
	{
		false;
	}
}

HRESULT Engine::D3DDeviceSetup( UINT width, UINT height, UINT createDeviceFlags )
{
	HRESULT hr = S_OK;

	D3D10_DRIVER_TYPE driverTypes[] =
	{
		D3D10_DRIVER_TYPE_HARDWARE,
		D3D10_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = sizeof( driverTypes ) / sizeof( driverTypes[0] );

	DXGI_SWAP_CHAIN_DESC swapChain;
	ZeroMemory( &swapChain, sizeof( swapChain ) );
	swapChain.BufferCount = 1;
	swapChain.BufferDesc.Width = width;
	swapChain.BufferDesc.Height = height;
	swapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	swapChain.BufferDesc.RefreshRate.Numerator = 60;
	swapChain.BufferDesc.RefreshRate.Denominator = 1;
	swapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChain.OutputWindow = myWindowHandler;
	swapChain.SampleDesc.Count = 1;
	swapChain.SampleDesc.Quality = 0;
	swapChain.Windowed = TRUE;

	for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
	{
		myDriverType = driverTypes[driverTypeIndex];
		hr = D3D10CreateDeviceAndSwapChain( NULL, myDriverType, NULL, createDeviceFlags,
			D3D10_SDK_VERSION, &swapChain, &mySwapChain, &my3DDevice );

		if( SUCCEEDED( hr ) )
			break;
	}

	return hr;
}

ID3D10Device* Engine::GetDevice() const
{
	return my3DDevice;
}

void Engine::RenderScene()
{

}

HWND& Engine::GetWindowHandler()
{
	return myWindowHandler;
}

Scene& Engine::GetScene()
{
	return (*myScene);
}

TextureContainer& Engine::GetTextureContainer()
{
	return myTextureContainer;
}

DXSDKMeshFactory& Engine::GetMeshFactory()
{
	return myMeshFactory;
}

const int Engine::GetScreeenWidth() const
{
	return myInfoArgument.myResolutionWidth;
}

const int Engine::GetScreenHeight() const
{
	return myInfoArgument.myResolutionHeight;
}

bool Engine::InitZBuffer()
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
	hr = Engine::GetInstance()->GetDevice()->CreateTexture2D( &descDepth, NULL, &myZBuffer );
	if( FAILED(hr) )
		return false;

	// Create the depth stencil view
	ID3D10DepthStencilView* tempDepthStencilView;
	D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = Engine::GetInstance()->GetDevice()->CreateDepthStencilView( myZBuffer, &descDSV, &tempDepthStencilView );
	if( FAILED(hr) )
		return false;

	myDepthStencilView = tempDepthStencilView;

	return true;
}

EffectInput& Engine::GetEffectInput()
{
	return myEffectInput;
}

EffectContainer& Engine::GetEffectContainer()
{
	return myEffectContainer;
}

ModelContainer& Engine::GetModelContainer()
{
	return myModelContainer;
}

Renderer& Engine::GetRenderer()
{
	return myRenderer;
}

ID3D10DepthStencilView* Engine::GetDepthStencil()
{
	return myDepthStencilView;
}

ID3D10RenderTargetView* Engine::GetBackBuffer()
{
	return myBackBufferRenderTargetView;
}

void Engine::SetTitle( const std::string aString )
{
	SetWindowText( myWindowHandler, aString.c_str() );
}
