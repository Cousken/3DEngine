#ifndef _RENDER_TARGET_H_
#define _RENDER_TARGET_H_

#include <D3D10.h>

class RenderTarget
{
public:
	RenderTarget(void);
	~RenderTarget(void);

	ID3D10RenderTargetView* GetRenderTargetView();
	ID3D10RenderTargetView** GetRenderTargetViewDoublePointer();
	ID3D10ShaderResourceView* GetShaderResourceView();

	bool Init( DXGI_FORMAT aTextrueFormat = DXGI_FORMAT_R8G8B8A8_UNORM, const float aDownsampleFactor = 1.0f );
	bool Init( DXGI_FORMAT aTextrueFormat, const int aWidthAndHeight );
	bool InitForOnePixel();
	bool InitCube( DXGI_FORMAT aTextrueFormat = DXGI_FORMAT_R32G32B32A32_FLOAT );
	void Release();

//private:
	ID3D10Texture2D* myTexture;
	ID3D10RenderTargetView* myRenderTargetView;
	ID3D10ShaderResourceView* myShaderResourceView;
};

#endif