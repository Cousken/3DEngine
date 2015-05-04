#include "Effect.h"
#include "Engine.h"
#include <D3DX10.h>
#include "EffectInput.h"

Effect::Effect()
{
	myEffectFile						= "";
	myEffect							= NULL;
	myTechnique							= NULL;

	myNamedTechnique					= NULL;
	myAmbientTechnique					= NULL;
	myNormalTechnique					= NULL;
	myAdditativeDirectionalTechnique	= NULL;
	myAdditativePointTechnique			= NULL;
	myAdditativeAreaTechnique			= NULL;
	myShadowVolumeTechnique				= NULL;
	myShadowVolume_SHOWTechnique		= NULL;
	myQuadTechnique						= NULL;
	myQuadDownsampleTechnique			= NULL;
	myQuadToneMappingTechnique			= NULL;
	myPostProcessMotionBlurTechnique	= NULL;
	myStreakTechnique					= NULL;
	myFromLightTechnique				= NULL;

	myDepthTechnique					= NULL;
	myReconstructWorldTechnique			= NULL;
	myDepthNormalTechnique				= NULL;
	myDeferredDirectionalTechnique		= NULL;
	myDeferredPointTechnique			= NULL;
	myDeferredSpotTechnique				= NULL;
	mySSAOTechnique						= NULL;

	myTime								= NULL;
	myStartAlpha						= NULL;
	mySizeDelta							= NULL;
	myAlphaDelta						= NULL;

	myDownSampleOffset					= NULL;
	myBloomSize							= NULL;
	myBloomTextureOffset				= NULL;
	myBloomColorValues					= NULL;

	myBoneMatrixArrayVariable			= NULL;
}

Effect::~Effect()
{
	if( myEffect != NULL )
		myEffect->Release();

}

bool Effect::InitEffect( const std::string &aEffectFile )
{
	myEffectFile = aEffectFile;

	unsigned int dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
	ID3D10Blob *errors = NULL;
#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3D10_SHADER_DEBUG;
#endif

	int childFlag = 0;


	childFlag = D3D10_EFFECT_COMPILE_CHILD_EFFECT;
	
	ID3D10EffectPool* effectPool= Engine::GetInstance()->GetEffectInput().GetEffectPool();
	HRESULT result = D3DX10CreateEffectFromFile( myEffectFile.c_str(), NULL, NULL, "fx_4_0", dwShaderFlags, childFlag,
		Engine::GetInstance()->GetDevice(), effectPool , NULL, 
												 &myEffect, &errors, NULL );

	if( FAILED( result ) )
	{
		if( errors != NULL )
		{
			char *error = NULL;
			error = (char *)errors->GetBufferPointer();
			MessageBox( Engine::GetInstance()->GetWindowHandler(), error, "Shader Error", MB_OK );
		}
		else
		{
			MessageBox( Engine::GetInstance()->GetWindowHandler(), "The FX file cannot be located or a silent error occured...", "Shader Error", MB_OK );
		}
		return false;
	}

	myTechnique = myEffect->GetTechniqueByName( "Render" );
	myNormalTechnique = myEffect->GetTechniqueByName( "Render" );
	myAdditativeDirectionalTechnique = myEffect->GetTechniqueByName( "Render_Additative_Directional" );
	myAdditativePointTechnique = myEffect->GetTechniqueByName( "Render_Additative_Point" );
	myAdditativeAreaTechnique = myEffect->GetTechniqueByName( "Render_Additative_Area" );
	myShadowVolumeTechnique = myEffect->GetTechniqueByName( "Render_ShadowVolume" );
	myShadowVolume_SHOWTechnique= myEffect->GetTechniqueByName( "Render_ShadowVolume_SHOW" );
	myAmbientTechnique = myEffect->GetTechniqueByName( "Render_Ambient" );
	myQuadTechnique = myEffect->GetTechniqueByName( "Render_Quad" );
	myQuadDownsampleTechnique = myEffect->GetTechniqueByName( "Render_Quad_Downsample" );
	myQuadToneMappingTechnique = myEffect->GetTechniqueByName( "Render_Quad_ToneMapping" );
	myPostProcessMotionBlurTechnique = myEffect->GetTechniqueByName( "PP_Motion_Blur" );
	myStreakTechnique = myEffect->GetTechniqueByName( "Streaks" );
	myFromLightTechnique = myEffect->GetTechniqueByName( "FromLight" );
	myCubeMapTechnique = myEffect->GetTechniqueByName( "Render_CubeMap" );
	
	myDepthTechnique = myEffect->GetTechniqueByName( "Render_Depth" );
	myReconstructWorldTechnique = myEffect->GetTechniqueByName( "Reconstruct_World" );
	myDepthNormalTechnique = myEffect->GetTechniqueByName( "Render_DepthNormal" );
	myDeferredDirectionalTechnique = myEffect->GetTechniqueByName( "Render_DeferredDirectional" );
	myDeferredPointTechnique = myEffect->GetTechniqueByName( "Render_DeferredPoint" );
	myDeferredSpotTechnique = myEffect->GetTechniqueByName( "Render_DeferredSpot" );
	myDeferredPointMeshTechnique = myEffect->GetTechniqueByName( "Render_DeferredPointMesh" );
	mySSAOTechnique = myEffect->GetTechniqueByName("Render_SSAO");

	myBoneMatrixArrayVariable = myEffect->GetVariableByName("BoneMatrixArray")->AsMatrix();

	return true;
}

ID3D10Effect *Effect::GetEffect()
{
	return myEffect;
}


std::string Effect::GetFileName() const
{
	return myEffectFile;
}

ID3D10EffectTechnique * Effect::GetTechnique( EffectTechniques::TechniqueType aTechinqueType /*= EffectTechniques::NORMAL*/ )
{
	switch (aTechinqueType)
	{
	case EffectTechniques::NORMAL	:
		if (myNormalTechnique == NULL) assert(0);
		return(myNormalTechnique);
	case EffectTechniques::ADD_DIRECTIONAL	:
		if (myAdditativeDirectionalTechnique == NULL) assert(0);
		return(myAdditativeDirectionalTechnique);
	case EffectTechniques::ADD_POINT	:
		if (myAdditativePointTechnique == NULL) assert(0);
		return(myAdditativePointTechnique);
	case EffectTechniques::ADD_AREA	:
		if (myAdditativeAreaTechnique == NULL) assert(0);
		return(myAdditativeAreaTechnique);
	case EffectTechniques::AMBIENT	:
		if (myAmbientTechnique == NULL) assert(0);
		return(myAmbientTechnique);
	case EffectTechniques::SHADOW_VOLUME :
		if (myShadowVolumeTechnique == NULL) assert(0);
		return(myShadowVolumeTechnique);
	case EffectTechniques::SHADOW_VOLUME_SHOW :
		if (myShadowVolume_SHOWTechnique == NULL) assert(0);
		return(myShadowVolume_SHOWTechnique);
	case EffectTechniques::QUAD :
		if (myQuadTechnique == NULL) assert(0);
		return(myQuadTechnique);
	case EffectTechniques::QUAD_DOWNSAMPLE :
		if (myQuadDownsampleTechnique == NULL) assert(0);
		return(myQuadDownsampleTechnique);
	case EffectTechniques::QUAD_TONEMAPPING :
		if (myQuadToneMappingTechnique == NULL) assert(0);
		return(myQuadToneMappingTechnique);
	case EffectTechniques::PP_MOTION_BLUR :
		if (myPostProcessMotionBlurTechnique == NULL) assert(0);
		return(myPostProcessMotionBlurTechnique);
	case EffectTechniques::STREAK :
		if (myStreakTechnique == NULL) assert(0);
		return(myStreakTechnique);
	case EffectTechniques::FROM_LIGHT :
		if (myFromLightTechnique == NULL) assert(0);
		return(myFromLightTechnique);
	case EffectTechniques::CUBE_MAP :
		if (myCubeMapTechnique == NULL) assert(0);
		return(myCubeMapTechnique);
	case EffectTechniques::DEPTH :
		if (myDepthTechnique) assert(0);
		return (myDepthTechnique);
	case EffectTechniques::RECONSTRUCT_WORLD :
		if (myReconstructWorldTechnique == NULL) assert(0);
		return (myReconstructWorldTechnique);
	case EffectTechniques::DEPTH_NORMAL_ALBEDO :
		if (myDepthNormalTechnique == NULL) assert(0);
		return (myDepthNormalTechnique);
	case EffectTechniques::DEFERRED_DIRECTIONAL :
		if (myDeferredDirectionalTechnique == NULL) assert(0);
		return (myDeferredDirectionalTechnique);
	case EffectTechniques::DEFERRED_POINT :
		if (myDeferredPointTechnique == NULL) assert(0);
		return (myDeferredPointTechnique);
	case EffectTechniques::DEFERRED_SPOT:
		if (myDeferredSpotTechnique == NULL) assert(0);
		return (myDeferredSpotTechnique);
	case EffectTechniques::DEFERRED_POINT_MESH:
		if (myDeferredPointMeshTechnique == NULL) assert(0);
		return (myDeferredPointMeshTechnique);
	case EffectTechniques::SSAO:
		if (mySSAOTechnique == NULL) assert(0);
		return (mySSAOTechnique);
	default:
		assert(0);
	};

	return(NULL);
}

void Effect::SetBoneMatrixVariable( CommonUtilities::StaticArray< Matrix44f, 255 > aBoneMatrixArray )
{
	if( myBoneMatrixArrayVariable != NULL )
	{
		myBoneMatrixArrayVariable->SetMatrixArray(reinterpret_cast<float*>((&aBoneMatrixArray[0])),0,255);
	}
}
