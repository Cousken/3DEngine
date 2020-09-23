#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "RenderTarget.h"
#include "RenderProcessTarget.h"
#include <d3d10.h>
#include "FullscreenHelper.h"
#include "Application.h"
#include "AmbientProbe.h"
#include "debugHeader.h"

class Scene;

class Renderer
{
public:
	Renderer(void);
	~Renderer(void);

	bool Init();
	void Render();
	void RenderToCube( Vector3f aPosition, int aCubeMapIndex );
	void ResetStuff();
	void RenderCross(AmbientProbe::CubeMapSideTypes aSideToRenderCrossAround, int anAmbientProbeIndex, Vector3f aPosition);
	void DownsampleCross(ID3D10ShaderResourceView* aCrossToDownsample);
	void RenderDepthOfField(RenderTarget& aRenderTarget);

	void ClearRenderTargets( float aClearColor[4] );
	void Cleanup();

	void SetScene( Scene* aScene );
	void SetHDRFlag( const bool aFlag );
	void SetInstancingFlag( const bool aFlag );

	ID3D10ShaderResourceView* GetGeneratedCubeMap(const int anAmbientProbeIndex);
	ID3D10ShaderResourceView* GetGeneratedReflectionMap(const int anAmbientProbeIndex);
	FullscreenHelper& GetFullScreenHelper();

	void WriteAllToDisk();

	bool myToggleAdvancedCulling;
	bool myToggleQuadOrFullScreen;
	bool myTogglePointOrSpotLight;
	Instance* mySphereMesh;
	Instance* myConeMesh;
//private:
	void Downsample();
	bool SetDownsampleVariables(Vector2f aScreenDimensions);
	bool InitShadowDepthStencil();

	void RenderDepthNormalAlbedo();
	void RenderDeferredDirectionalLights(RenderTarget *aRenderTarget, RenderTarget& aDepthMap, const Vector2f &aSize, const Vector2f anOffset);
	void RenderDeferredPointLights(RenderTarget *aRenderTarget, const Vector2f &aSize, const Vector2f anOffset);
	void RenderDeferredSpotLights(RenderTarget *aRenderTarget, RenderTarget& aDepthMap, const Vector2f &aSize, const Vector2f anOffset, const bool anEnableSpecFlag = true);

	void RenderSSAO(RenderTarget &aTexture, const Vector2f &aSize, const Vector2f anOffset);
	void WriteTextureToDisk(RenderTarget& aTexture, std::string aFilename);
	
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
	RenderTarget myDownSampledCross400;
	RenderTarget myDownSampledCross200;
	RenderTarget myDownSampledCross100;
	RenderTarget myDownSampledCross60;
	AmbientProbe myAmbientProbes[Application::myNumberOfAmbientProbes];
	RenderTarget myTemporaryCubeSides[6];
	RenderTarget myTemporaryReflectionCubeSides[6];
	RenderTarget myTemporaryReflectionTexture;
	RenderTarget myDownsampledAlbedo;
	RenderTarget myDownsampledAlbedo2;
	RenderTarget myShadowMapCopy;
	RenderTarget myShadowMapCopy2;
	
	RenderTarget myDepthMap;
	RenderTarget myLinearDepthMap;
	RenderTarget myNormalMap;
	RenderTarget myAlbedoMap;
	RenderTarget myAmbientMap;
	RenderTarget myFinalScene;
	RenderTarget myAlbedoMap2;
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