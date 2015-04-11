#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <windows.h>
#include <d3d10.h>
#include <d3dx10.h>
#include "Scene.h"
#include "TextureContainer.h"
#include "DXSDKMeshFactory.h"
#include "EffectInput.h"
#include "EffectContainer.h"
#include "ModelContainer.h"
#include "Renderer.h"

#include "SetupInfo.h"

class Engine
{
public:
	static bool Create( int nCmdShow, SetupInfo aSetupInfo, WNDPROC aWndProc, HINSTANCE aHInstance );
	static void ShutDown();
	static Engine* GetInstance();

	void SwitchBuffers();
	void RenderScene();
	HWND& GetWindowHandler();
	ID3D10Device* GetDevice() const;

	Scene& GetScene();
	FullscreenHelper &GetFullScreenHelper();
	TextureContainer& GetTextureContainer();
	DXSDKMeshFactory& GetMeshFactory();
	EffectInput& GetEffectInput();
	EffectContainer& GetEffectContainer();
	ModelContainer& GetModelContainer();
	Renderer &GetRenderer();

	IDXGISwapChain* GetSwapChain();

	const int GetScreeenWidth() const;
	const int GetScreenHeight() const;
	
	ID3D10DepthStencilView* GetDepthStencil();
	ID3D10RenderTargetView* GetBackBuffer();

	void SetTitle( const std::string aString );

private:
	bool WindowSetup(WNDPROC aWndProc,const SetupInfo& aInfoArgument);

	Engine();
	~Engine(void);	
	void CleanupDevice();
	bool Init( WNDPROC aWinProc, int nCmdShow, SetupInfo aSetupInfo, HINSTANCE aHInstance );
	bool D3DSetup();
	HRESULT D3DDeviceSetup( UINT width, UINT height, UINT createDeviceFlags );
	bool D3DViewPortSetup( UINT width, UINT height );
	bool D3DStencilBufferSetup();
	bool InitZBuffer();

	
	static Engine* ourInstance;
	Scene* myScene;
	Renderer myRenderer;
	TextureContainer myTextureContainer;
	DXSDKMeshFactory myMeshFactory;
	EffectInput myEffectInput;
	EffectContainer myEffectContainer;
	ModelContainer myModelContainer;

	D3D10_DRIVER_TYPE       myDriverType;
	ID3D10Device*           my3DDevice;
	IDXGISwapChain*         mySwapChain;
	
	HINSTANCE               myHInstance; // is currently not used
	HWND                    myWindowHandler;
	SetupInfo				myInfoArgument;

	ID3D10Texture2D* myZBuffer;
	ID3D10Texture2D* myStencilBuffer;
	ID3D10DepthStencilView* myDepthBufferView;
	ID3D10DepthStencilView* myDepthStencilView;

	ID3D10RenderTargetView* myBackBufferRenderTargetView;
};

#endif