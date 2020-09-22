#ifndef _FULLSCREEN_HELPER_H_
#define _FULLSCREEN_HELPER_H_

#include "BufferWrapper.h"
#include "Effect.h"
#include "CU_GrowingArray.h"
#include <d3d10.h>
#include "EffectTechnique.h"

class Instance;
class Camera;
class RenderTarget;

class FullscreenHelper
{
public:
	FullscreenHelper(void);
	~FullscreenHelper(void);

	void Process( ID3D10ShaderResourceView* aResourceToRender, ID3D10RenderTargetView* aTargetView, std::string aTechniqueName, const Vector2f& aWidthAndHeight = Vector2f(800.0f, 800.0f), const Vector2f& aLeftCornerXY = Vector2f(0.0f, 0.0f));
	void Process( ID3D10ShaderResourceView* aResourceToRender, ID3D10RenderTargetView* aTargetView, EffectTechniques::TechniqueType aTechnique);
	void Process( ID3D10ShaderResourceView* aResourceToRender, ID3D10RenderTargetView* aTargetView, EffectTechniques::TechniqueType aTechnique, const Vector2f& aWidthAndHeight, const Vector2f& aLeftCornerXY);
	void ProcessMeshCulling(Camera &aCamera, Instance* anInstance, EffectTechniques::TechniqueType aTechnique, ID3D10ShaderResourceView* aResourceToRender, ID3D10RenderTargetView* aTargetView, const bool anIsCameraInsideFlag);
	void ResetPostProcessingTextures();
	void DisableScissorRect();
	bool Init();

	Effect* GetEffect();

private:
	bool InitVertexBuffer();
	VertexBufferWrapper myVertexBufferWrapper;
	VertexDataWrapper myVertexDataWrapper;
	Effect* myEffect;
	ID3D10InputLayout* myVertexLayout;
	ID3D10RasterizerState* myRenderAllRasterizerState;
	ID3D10RasterizerState* myRenderBackRasterizerState;
};

#endif