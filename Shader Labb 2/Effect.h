#ifndef PEDOBEAR_EFFECT_HEADER_
#define PEDOBEAR_EFFECT_HEADER_

#include <D3D10.h>
#include <string>
#include "Texture.h"
#include "Vector.h"
#include "EffectTechnique.h"
#include "StaticArray.h"
#include "Matrix.h"

class EffectInput;

class Effect
{
public:
	Effect();
	~Effect();

	bool InitEffect( const std::string &anEffectfile );


	ID3D10Effect *GetEffect();
	ID3D10EffectTechnique *GetTechnique(EffectTechniques::TechniqueType aTechinqueType = EffectTechniques::NORMAL);
	std::string GetFileName() const;

	void SetTime(float aTime);
	void SetStartAlpha(float aAlpa);
	void SetSizeDelta(float aDelta);
	void SetAlphaDelta(float aDelta);
	void SetBloomSize(int aSize);
	void SetBloomTextureOffset(Vector4f* someTextureOffsets);
	void SetBloomColorValues(Vector4f* someColorValues);
	void SetDownSampleOffset(Vector2f* someColorValues);

	void SetBoneMatrixVariable( CommonUtilities::StaticArray< Matrix44f, 255 > aBoneMatrixArray );
	
private:
	std::string myEffectFile;
	ID3D10Effect *myEffect;
	ID3D10EffectTechnique *myTechnique;

	ID3D10EffectTechnique*  myNamedTechnique;
	ID3D10EffectTechnique*  myAmbientTechnique;
	ID3D10EffectTechnique*  myNormalTechnique;
	ID3D10EffectTechnique*  myAdditativeDirectionalTechnique;
	ID3D10EffectTechnique*  myAdditativePointTechnique;
	ID3D10EffectTechnique*  myAdditativeAreaTechnique;
	ID3D10EffectTechnique*  myShadowVolumeTechnique;
	ID3D10EffectTechnique*  myShadowVolume_SHOWTechnique;
	ID3D10EffectTechnique*	myQuadTechnique;
	ID3D10EffectTechnique*	myQuadDownsampleTechnique;
	ID3D10EffectTechnique*	myQuadToneMappingTechnique;
	ID3D10EffectTechnique*  myPostProcessMotionBlurTechnique;
	ID3D10EffectTechnique*	myStreakTechnique;
	ID3D10EffectTechnique*	myFromLightTechnique;
	ID3D10EffectTechnique*	myCubeMapTechnique;

	ID3D10EffectTechnique* myDepthTechnique;
	ID3D10EffectTechnique* myReconstructWorldTechnique;
	ID3D10EffectTechnique* myDepthNormalTechnique;
	ID3D10EffectTechnique* myDepthAlbedoTechnique;
	ID3D10EffectTechnique* myDeferredDirectionalTechnique;
	ID3D10EffectTechnique* myDeferredAmbientTechnique;
	ID3D10EffectTechnique* myDeferredPointTechnique;
	ID3D10EffectTechnique* myDeferredSpotTechnique;
	ID3D10EffectTechnique* myDeferredPointScissoredTechnique;
	ID3D10EffectTechnique* myDeferredSpotScissoredTechnique;
	ID3D10EffectTechnique* myDeferredPointMeshTechnique;
	ID3D10EffectTechnique* mySSAOTechnique;

	ID3D10EffectScalarVariable* myTime;
	ID3D10EffectScalarVariable* myStartAlpha;
	ID3D10EffectScalarVariable* mySizeDelta;
	ID3D10EffectScalarVariable* myAlphaDelta;

	ID3D10EffectVectorVariable* myDownSampleOffset;
	ID3D10EffectScalarVariable* myBloomSize;
	ID3D10EffectVectorVariable* myBloomTextureOffset;
	ID3D10EffectVectorVariable* myBloomColorValues;

	ID3D10EffectMatrixVariable* myBoneMatrixArrayVariable;

	int myCurrentBloomSize;
};


#endif // PEDOBEAR_EFFECT_HEADER_