#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "RenderTarget.h"
#include "RenderProcessTarget.h"
#include <d3d10.h>
#include "FullscreenHelper.h"

class Scene;

class Renderer
{
public:
	Renderer(void);
	~Renderer(void);

	bool Init();
	void Render();
	void RenderToCube( const CommonUtilities::StaticArray< Matrix44f, 6 >& someToView );

	void ClearRenderTargets( float aClearColor[4] );
	void Cleanup();

	void SetScene( Scene* aScene );
	void SetHDRFlag( const bool aFlag );
	void SetInstancingFlag( const bool aFlag );

	ID3D10ShaderResourceView* GetGeneratedCubeMap();
	FullscreenHelper& GetFullScreenHelper();

	bool myToggleAdvancedCulling;
	bool myToggleQuadOrFullScreen;
	bool myTogglePointOrSpotLight;
private:
	void Downsample();
	bool SetDownsampleVariables(Vector2f aScreenDimensions);
	bool InitShadowDepthStencil();

	void RenderDepthNormalAlbedo();
	void RenderDeferredDirectionalLights(RenderTarget *aRenderTarget, const Vector2f &aSize, const Vector2f anOffset);
	void RenderDeferredPointLights(RenderTarget *aRenderTarget, const Vector2f &aSize, const Vector2f anOffset);
	void RenderDeferredSpotLights(RenderTarget *aRenderTarget, const Vector2f &aSize, const Vector2f anOffset);
	void RenderFinalFrameTexture(RenderTarget &aTexture, const Vector2f &aSize, const Vector2f anOffset);
	void RenderSSAO(RenderTarget &aTexture, const Vector2f &aSize, const Vector2f anOffset);
	void CopyAlbedo();
	Scene* myScene;

	FullscreenHelper myFullscreenHelper;

	RenderTarget myRenderTargetTexture;
	RenderTarget myDownsampledTexture2;
	RenderTarget myDownsampledTexture4;
	RenderTarget myDownsampledTexture8;
	RenderTarget myDownsampledTexture16;
	RenderTarget myDownsampledTexture32;
	RenderTarget myDownsampledTexture64;
	RenderTarget myDownsampledTexture128;
	RenderTarget myDownsampledTexture256;
	RenderTarget myDownsampledTexture512;
	RenderTarget myDownsampledOnePixel;
	RenderTarget myShadowBuffer;
	RenderTarget myCubeMap;
	
	RenderTarget myDepthMap;
	RenderTarget myNormalMap;
	RenderTarget myAlbedoMap;
	RenderTarget myFinalScene;
	RenderTarget myAmbientOcclusionMap;

	RenderProcessTarget myRenderProcessTarget;

	ID3D10Texture2D* myShadowZBuffer;
	ID3D10DepthStencilView* myShadowDepthStencilView;
	Texture *myNoiseTexture; 

	bool myMotionBlurFlag;
	bool myHDRFlag;
	bool myInstancingFlag;
	
};

#endif