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
	myDepthAlbedoTechnique = myEffect->GetTechniqueByName("Render_DepthAlbedo");
	myDeferredDirectionalTechnique = myEffect->GetTechniqueByName( "Render_DeferredDirectional" );
	myDeferredAmbientTechnique = myEffect->GetTechniqueByName("Render_DeferredAmbient");
	myDeferredPointTechnique = myEffect->GetTechniqueByName( "Render_DeferredPoint" );
	myDeferredSpotTechnique = myEffect->GetTechniqueByName( "Render_DeferredSpot" );
	myDeferredPointScissoredTechnique = myEffect->GetTechniqueByName( "Render_DeferredPointScissored" );
	myDeferredSpotScissoredTechnique = myEffect->GetTechniqueByName( "Render_DeferredSpotScissored" );
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
		return(myNormalTechnique);
	case EffectTechniques::ADD_DIRECTIONAL	:
		return(myAdditativeDirectionalTechnique);
	case EffectTechniques::ADD_POINT	:
		return(myAdditativePointTechnique);
	case EffectTechniques::ADD_AREA	:
		return(myAdditativeAreaTechnique);
	case EffectTechniques::AMBIENT	:
		return(myAmbientTechnique);
	case EffectTechniques::SHADOW_VOLUME :
		return(myShadowVolumeTechnique);
	case EffectTechniques::SHADOW_VOLUME_SHOW :
		return(myShadowVolume_SHOWTechnique);
	case EffectTechniques::QUAD :
		return(myQuadTechnique);
	case EffectTechniques::QUAD_DOWNSAMPLE :
		return(myQuadDownsampleTechnique);
	case EffectTechniques::QUAD_TONEMAPPING :
		return(myQuadToneMappingTechnique);
	case EffectTechniques::PP_MOTION_BLUR :
		return(myPostProcessMotionBlurTechnique);
	case EffectTechniques::STREAK :
		return(myStreakTechnique);
	case EffectTechniques::FROM_LIGHT :
		return(myFromLightTechnique);
	case EffectTechniques::CUBE_MAP :
		//return(myCubeMapTechnique);
		return myEffect->GetTechniqueByName( "Render_CubeShadowBuffer" );
	case EffectTechniques::DEPTH :
		return (myDepthTechnique);
	case EffectTechniques::RECONSTRUCT_WORLD :
		return (myReconstructWorldTechnique);
	case EffectTechniques::DEPTH_NORMAL_ALBEDO :
		return (myDepthNormalTechnique);
	case EffectTechniques::DEPTH_ALBEDO:
		return (myDepthAlbedoTechnique);
	case EffectTechniques::DEFERRED_DIRECTIONAL :
		return (myDeferredDirectionalTechnique);
	case EffectTechniques::DEFERRED_AMBIENT:
		return (myDeferredAmbientTechnique);
	case EffectTechniques::DEFERRED_POINT :
		return (myDeferredPointTechnique);
	case EffectTechniques::DEFERRED_SPOT:
		return (myDeferredSpotTechnique);
	case EffectTechniques::DEFERRED_POINT_SCISSORED :
		return (myDeferredPointScissoredTechnique);
	case EffectTechniques::DEFERRED_SPOT_SCISSORED:
		return (myDeferredSpotScissoredTechnique);
	case EffectTechniques::DEFERRED_POINT_MESH:
		return (myDeferredPointMeshTechnique);
	case EffectTechniques::SSAO:
		return (mySSAOTechnique);
	case EffectTechniques::DEPTH_FROM_LIGHT:
		return (myEffect->GetTechniqueByName("Render_DepthFromLight"));
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
