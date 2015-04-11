#ifndef _FULLSCREEN_HELPER_H_
#define _FULLSCREEN_HELPER_H_

#include "BufferWrapper.h"
#include "Effect.h"
#include "CU_GrowingArray.h"
#include <d3d10.h>
#include "EffectTechnique.h"

class Instance;
class Camera;

class FullscreenHelper
{
public:
	FullscreenHelper(void);
	~FullscreenHelper(void);

	void Process( ID3D10ShaderResourceView* aResourceToRender, ID3D10RenderTargetView* aTargetView, EffectTechniques::TechniqueType aTechnique);
	void Process( ID3D10ShaderResourceView* aResourceToRender, ID3D10RenderTargetView* aTargetView, EffectTechniques::TechniqueType aTechnique, const Vector2f& aWidthAndHeight, const Vector2f& aLeftCornerXY);
	void ProcessSphere(ID3D10ShaderResourceView* aResourceToRender, ID3D10RenderTargetView* aTargetView, EffectTechniques::TechniqueType aTechnique, const Vector2f& aWidthAndHeight, const Vector2f& aLeftCornerXY, Vector3f aPosition, Camera &aCamera);
	bool Init();

	void SetDeferredRenderingSphere(Instance *aSphere);

	Effect* GetEffect();

private:
	bool InitVertexBuffer();
	VertexBufferWrapper myVertexBufferWrapper;
	VertexDataWrapper myVertexDataWrapper;
	Effect* myEffect;
	ID3D10InputLayout* myVertexLayout;

	Instance *myDeferredRenderingSphere;
};

#endif